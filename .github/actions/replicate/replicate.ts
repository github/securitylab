import * as core from '@actions/core'
import * as github from '@actions/github'
import { WebhookPayload } from '@actions/github/lib/interfaces'
import { getIssueList, internalIssueAlreadyCreated } from './issues'

export const BOUNTY_LABELS = ['All For One', 'The Bug Slayer'] as const
export type BountyType = typeof BOUNTY_LABELS[number]
type CommentMap = {[K in BountyType]: string}
export type Issue = {title: string, body: string, labels: string[], bountyType: BountyType}
type GitHubIssue = { [key: string]: any, number: number, html_url?: string | undefined, body?: string | undefined}

const COMMENT_TASK_LIST_AFO = `## Task List

- **If this is your first time in this process, have a look at that [5 min video](https://drive.google.com/file/d/1Uy3JukURoSk-2Bq7EjyagVdpsyvKI67E)**
- **Visit the [documented process](https://github.com/github/pe-security-lab/blob/main/docs/bug_bounty.md)**

- [ ] CodeQL Initial assessment - In case of rejection, please record your decision in the comment below:
  - [ ] Acceptance
  - [ ] Generate result set and post the URL in the comment
- [ ] Security Lab assessment - In case of rejection, please record your decision in the comment below: 
  - [ ] Acceptance
  - [ ] Score the Vulnerability Impact, the Vulnerability Scope, and the False Positive ratio based on the provided CodeQL result set
  - [ ] Document your assessments in comments below, for the CodeQL team 
  - [ ] Provide feedback to the author in the PR
- [ ] CodeQL assessment:
  - [ ] Provide feedback to the author in the PR
  - [ ] Merge the PR into the experimental folder
  - [ ] Score the Code Maturity and the Documentation
- [ ] Bounty Payment
`

const COMMENT_TASK_LIST_BS = `## Task List
- [ ] Security Lab assessment:
  - [ ] Acceptance
  - [ ] Confirm the CVE 
  - [ ] Assess the Vulnerability Impact, the Vulnerability Scope
  - [ ] Get the CodeQL scores (False Positive ratio, Code Maturity and the Documentation) from the previous query rating
  - [ ] PR is merged? Finalize the score
- [ ] Bounty Payment`

const COMMENT_TASK_LIST: CommentMap = {
    'All For One': COMMENT_TASK_LIST_AFO,
    'The Bug Slayer': COMMENT_TASK_LIST_BS
}

const COMMENT_SCORING = `## Scoring

- **Visit the [scoring guidelines](https://github.com/github/pe-security-lab/blob/main/docs/bug_bounty.md)**
- **Accepted values are: 0 (= NA), or 1 (minimal) to 5 (maximal). Any other value will throw an error**

| Criterion | Score|
|--- | --- |
| Vulnerability Impact | | 
| Vulnerability Scope | | 
| False Positive | | 
| Code Maturity | | 
| Documentation | | 

- [ ] Reject
- [ ] Reject with thank you reward
- [ ] Reject with encouragement swag (Decision: Dev Advocacy)
- [ ] Accept
`

const getIssueFromRef = async (issueRef: string | undefined): Promise<GitHubIssue | undefined> => {
    if(!issueRef)
        return undefined
    const token: string | undefined = process.env['GITHUB_TOKEN']
    if(token === undefined)
        return undefined
    const octokit: github.GitHub = new github.GitHub(token)
    const issueResponse = await octokit.issues.get({
        owner: github.context.repo.owner,
        repo: github.context.repo.repo,
        issue_number: Number(issueRef),
      });
    return issueResponse.data  
}

export const generateInternalIssueContentFromPayload = async (payload?: WebhookPayload, issueRef?: string): Promise<Issue | undefined> => {
    const issue = await getIssueFromRef(issueRef) || payload?.issue
    let result: Issue = {title: 'none', body: 'none', labels: [], bountyType: 'All For One'}
    let bountyIssue: boolean = false

    if(!issue || !issue.user || !issue.html_url) {
        core.debug("Invalid issue payload")
        return undefined
    }

    issue.labels.forEach((element:any) => {
        result.labels.push(element.name)
        if(!bountyIssue) {
            bountyIssue = BOUNTY_LABELS.includes(element.name)
            if(bountyIssue) {
                result.bountyType = element.name
            }
        }
    });

    if(!bountyIssue) {
        core.debug("Not a bounty")
        return undefined
    }

    result.title = `[${result.bountyType}] ${issue.title}`,
    // In order to differentiate immediately the issues from others in the repo
    // And with the current situation, the robot with Read access cannot add labels to the issue
    result.body = `Original external [issue](${issue.html_url})

Submitted by [${issue.user.login}](${issue.user.html_url})

${issue.body? issue.body : ""}`

    return result
}

export const createInternalIssue = async (issue: Issue) : Promise<number | undefined> => {
    const internalRepoAccessToken: string | undefined = process.env['INT_REPO_TOKEN']
    let internal_ref: number | undefined = undefined

    if(!internalRepoAccessToken) {
        core.debug("No valid token for creating issues on the internal repo")
        return
    }
    try {
        const octokit: github.GitHub = new github.GitHub(internalRepoAccessToken)
        const internalRepo = core.getInput('internal_repo') || '/'
        const [owner, repo] = internalRepo.split('/')
        const issueResponse = await octokit.issues.create( {
            owner,
            repo,
            title: issue.title,
            body: issue.body,
            labels: issue.labels
        })        
        internal_ref = issueResponse.data.number
        core.debug(`issue created: ${internal_ref}`)
        const labelsResponse = await octokit.issues.addLabels( {
            owner,
            repo,
            issue_number: internal_ref,
            labels: issue.labels
        })
        core.debug(`Labels addition result: ${labelsResponse.status} ${(labelsResponse.status==200)? "OK" : "FAILED"}`)

        const issueCommentResponse1 = await octokit.issues.createComment({
            owner,
            repo,
            issue_number: internal_ref,
            body: COMMENT_TASK_LIST[issue.bountyType],
        })
        core.debug(`comment created ${issueCommentResponse1.data.url}`)

        const issueCommentResponse2 = await octokit.issues.createComment({
            owner,
            repo,
            issue_number: internal_ref,
            body: COMMENT_SCORING,
        })
        core.debug(`comment created ${issueCommentResponse2.data.url}`)

        const issueCard = await octokit.projects.createCard({
            column_id: (issue.labels.includes(BOUNTY_LABELS[1]))? 10205381 : 10205379,
            content_id: internal_ref,
            content_type: 'issue',
        });
        core.debug(`Card creation status: ${issueCard.status}`)

    } catch(error) {
        core.debug(error.message)
    }
    return internal_ref
}

const checkDuplicates = async (payload: WebhookPayload): Promise<boolean> => {
    const internalRepoAccessToken: string | undefined = process.env['INT_REPO_TOKEN']
    const internalRepo = core.getInput('internal_repo') || '/'
    const [owner, repo] = internalRepo.split('/')
    const internalIssues = await getIssueList(owner, repo, internalRepoAccessToken, false, false)
    if(!internalIssues) {
        core.debug('Internal error. Cannot check for duplicates. Aborting')
            return true
    } else {
        const ref = internalIssueAlreadyCreated(payload.issue?.html_url, internalIssues)
        if(ref) {
            core.debug(`This issue has already been duplicated with reference ${ref}`)
            return true
        }
    }
    return false
}

const run = async (): Promise<void> => {
    const internalIssue = await generateInternalIssueContentFromPayload(github.context.payload, core.getInput('specific_issue'))
    if(!internalIssue)
        return

    if(await checkDuplicates(github.context.payload)) 
        return

    const internal_ref = await createInternalIssue(internalIssue)
    if(!internal_ref)
        return
}

run()
