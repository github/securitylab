import * as core from '@actions/core'
import * as github from '@actions/github'
import * as replicate from './replicate'

export type Issue_info = {title: string, author: string, body: string, number: number}
type Issue_state = 'open' | 'all' | 'closed' | undefined

export const getIssueList = async (owner: string, repo: string, token: string | undefined, open: boolean) : Promise<Issue_info[] | undefined> => {
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
            // labels: labelFilter -- Does not work properly
        })

        issues.data.forEach(issue => {
            const bountyLabel = issue.labels.some(label => {
                return replicate.BOUNTY_LABELS.includes(label.name)
            })
            if(bountyLabel){
                let item: Issue_info = {
                    title: issue.title,
                    author: issue.user?.login,
                    body: issue.body? issue.body : '',
                    number: issue.number
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

export const internalIssueAlreadyCreated = (externalSubmissionUrl: string | undefined, internalIssues: Issue_info[]) : number | undefined => {
    const searchString = `/Original external [issue](${externalSubmissionUrl})/`
    let ref: number | undefined = undefined
    internalIssues.some( element => {
        if(element.body.search(searchString) != -1) {
            ref = element.number
            return true
        }
    })
    return ref
}
