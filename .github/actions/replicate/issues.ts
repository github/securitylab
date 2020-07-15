import * as core from '@actions/core'
import * as github from '@actions/github'
import * as replicate from './replicate'

export type Issue_info = {title: string, author: string, body: string, number: number, html_url?: string}
type Issue_state = 'open' | 'all' | 'closed' | undefined

export const getIssueList = async (owner: string, repo: string, token: string | undefined, open: boolean, checkBountyLabels: boolean, per_page?: number) : Promise<Issue_info[] | undefined> => {
    if(!token) {
        core.debug("No valid token for creating issues on the internal repo")
        return
    }
    try {
        let result: Issue_info[] = []
        const octokit = new github.GitHub(token)
        const issueState: Issue_state = open? 'open' : 'all'
        // const labelFilter: string = replicate.BOUNTY_LABELS.join(',')
        const issues = await octokit.issues.listForRepo({
            owner, 
            repo,
            state: issueState,
            per_page: per_page? per_page : 100 // TODO: implement proper pagination
            // labels: labelFilter -- Does not work properly
        })

        issues.data.forEach(issue => {
            const bountyLabel = checkBountyLabels? issue.labels.some(label => {
                return replicate.BOUNTY_LABELS.includes(label.name as replicate.BountyType)
            }) : undefined
            if(!checkBountyLabels || bountyLabel){
                let item: Issue_info = {
                    title: issue.title,
                    author: issue.user?.login,
                    body: issue.body? issue.body : '',
                    number: issue.number,
                    html_url: issue.html_url
                }
                result.push(item)
            }
        });
        return result
    } catch(error) {
        core.debug(error.message)
        return undefined
    }
}

export const isUserAlreadyParticipant = (user: string, externalSubmissions: Issue_info[] | undefined) : boolean => {
    if(!externalSubmissions)
        return false
    const check = externalSubmissions.some( element => {
        return (element.author === user)
    })
    return check
}

function escapeRegExp(text: string) {
    return text.replace(/[-\/\\^$*+?.()|[\]{}]/g, '\\$&')
}

export const internalIssueAlreadyCreated = (externalSubmissionUrl: string | undefined, internalIssues: Issue_info[]) : number | undefined => {
    const searchString = new RegExp(escapeRegExp(`Original external [issue](${externalSubmissionUrl})`))
    let ref: number | undefined = undefined
    internalIssues.some( element => {
        if(element.body.search(searchString) != -1) {
            ref = element.number
            return true
        }
    })
    return ref
}
