import * as core from '@actions/core'
import * as github from '@actions/github'
import { getIssueList, internalIssueAlreadyCreated } from '../replicate/issues'

const run = async (): Promise<void> => {
    const internalRepoAccessToken: string | undefined = process.env['INT_REPO_TOKEN']
    const internalRepo = core.getInput('internal_repo') || '/'
    const [owner, repo] = internalRepo.split('/')
    const internalIssues = await getIssueList(owner, repo, internalRepoAccessToken, false, false)
    if(!internalIssues) {
        core.setFailed(`Internal error. Cannot access the internal repo ${internalRepo}. Aborting`)
        return
    } else {
        const externalIssues = await getIssueList(github.context.repo.owner, github.context.repo.repo, process.env['GITHUB_TOKEN'], true, true)
        if(!externalIssues) {
            core.setFailed(`Internal error when retrieving all issues.`)
            return
        }
        let failed = false
        externalIssues.forEach( issue => {
            const ref = internalIssueAlreadyCreated(issue?.html_url, internalIssues)
            if(!ref) {
                core.debug(`External issue ${issue?.number} is not replicated internally.`)
                failed = true
            }
        })
        if(failed) {
            core.setFailed("Some submissions are not replicated internally, see execution logs.")
        }
    }
    return
}

run()

