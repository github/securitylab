import * as core from '@actions/core'
import * as replicate from '../replicate'
import * as issues from '../issues'
import { WebhookPayload, PayloadRepository } from '@actions/github/lib/interfaces'

const TEST_ISSUE_1 = 1
const TEST_REPOSITORY: PayloadRepository = {
    full_name: 'myuser/myrepo',
    name: 'myrepo',
    owner: {
        login: 'myuser',
        name: 'My User'
    }
}

const TEST_INVALID_PAYLOAD_NOUSER: WebhookPayload = {
  repository: TEST_REPOSITORY,
  issue: {
      number: TEST_ISSUE_1,
      html_url: 'https://github.com/test_owner/test_repo/issues/1',
  }
}

const TEST_INVALID_PAYLOAD_NOURL: WebhookPayload = {
  repository: TEST_REPOSITORY,
  issue: {
      number: TEST_ISSUE_1,
      user: {
        login: 'issue_user',
        html_url: 'https://github.com/users/issue_user'
      }
  }
}

const TEST_INVALID_PAYLOAD_NOISSUE: WebhookPayload = {
  repository: TEST_REPOSITORY,
}

const TEST_LABEL_ALLFORONE = { name: 'All For One' }
const TEST_LABEL_NOTBOUNTY_1 = { name: 'not-a-bounty-label' }
const TEST_LABEL_NOTBOUNTY_2 = { name: 'not-a-bounty-label-either' }

const TEST_PAYLOAD_NOTBOUNTY: WebhookPayload = {
  repository: TEST_REPOSITORY,
  issue: {
      number: TEST_ISSUE_1,
      html_url: 'https://github.com/test_owner/test_repo/issues/1',
      user: {
        login: 'issue_user',
        html_url: 'https://github.com/users/issue_user'
      },
      labels: [TEST_LABEL_NOTBOUNTY_1,TEST_LABEL_NOTBOUNTY_2],
  }
}

const TEST_PAYLOAD: WebhookPayload = {
  repository: TEST_REPOSITORY,
  issue: {
      number: TEST_ISSUE_1,
      html_url: 'https://github.com/test_owner/test_repo/issues/1',
      user: {
        login: 'ghsecuritylab',
        html_url: 'https://github.com/ghsecuritylab'
      },
      title: 'Issue Title',
      labels: [TEST_LABEL_ALLFORONE,TEST_LABEL_NOTBOUNTY_1],
      body: `# This is the issue title
This is the issue body first line
This is the issue body second line
`
  }
}

const TEST_GENERATED_ISSUE: replicate.Issue = {
  title: '[All For One] Issue Title',
  labels: ['All For One','not-a-bounty-label'],
  bountyType: 'All For One',
  body: `Original external [issue](https://github.com/test_owner/test_repo/issues/1)

Submitted by [ghsecuritylab](https://github.com/ghsecuritylab)

# This is the issue title
This is the issue body first line
This is the issue body second line
`
}

describe('log errors when generating issue content', () => {
  it('outputs a message for invalid issue in payload', async () => {
    const debugMock = jest.spyOn(core, 'debug')
    const issue = await replicate.generateInternalIssueContentFromPayload(TEST_INVALID_PAYLOAD_NOURL)
    expect(debugMock).toHaveBeenCalledWith('Invalid issue payload')
    expect(issue).toBeUndefined()

    const issue2 = await replicate.generateInternalIssueContentFromPayload(TEST_INVALID_PAYLOAD_NOUSER)
    expect(debugMock).toHaveBeenCalledWith('Invalid issue payload')
    expect(issue2).toBeUndefined()

    const issue3 = await replicate.generateInternalIssueContentFromPayload(TEST_INVALID_PAYLOAD_NOISSUE)
    expect(debugMock).toHaveBeenCalledWith('Invalid issue payload')
    expect(issue3).toBeUndefined()

  })
})

describe('excludes non bounty issues', () => {
  it('creates the proper issue', async () => {
    const debugMock = jest.spyOn(core, 'debug')
    const issue = await replicate.generateInternalIssueContentFromPayload(TEST_PAYLOAD_NOTBOUNTY)
    expect(debugMock).toHaveBeenCalledWith('Not a bounty')
    expect(issue).toBeUndefined()
  })
})

describe('generates proper content', () => {
  it('creates the proper issue', async () => {
    const issue = await replicate.generateInternalIssueContentFromPayload(TEST_PAYLOAD)
    expect(issue).toBeDefined()
    expect(issue).toEqual(TEST_GENERATED_ISSUE)
  })
})

describe('check for duplicates', () => {
  it('can find duplicates', async () => {
    const TEST_REF: number = 31
    const TEST_BODY1 = `Original external [issue](https://github.com/owner/repo/issues/1)\n\nThen there is some text`
    const TEST_BODY2 = `Original external [issue](https://github.com/owner/repo/issues/2)\n\nThen there is some text`
    const TEST_INTERNAL_ISSUES: issues.Issue_info[] = [
      {title: 'issue 1', author: 'author1', body: TEST_BODY1, number: 31},
      {title: 'issue 2', author: 'author2', body: TEST_BODY2, number: 33}
    ]
    let foundRef: number | undefined = issues.internalIssueAlreadyCreated('https://github.com/owner/repo/issues/1', TEST_INTERNAL_ISSUES)
    expect(foundRef).toEqual(TEST_REF)
    foundRef = issues.internalIssueAlreadyCreated('https://github.com/owner/repo/issues/3', TEST_INTERNAL_ISSUES)
    expect(foundRef).toBeUndefined()
  })
})


