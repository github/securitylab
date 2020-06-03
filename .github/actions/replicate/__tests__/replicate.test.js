"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    Object.defineProperty(o, k2, { enumerable: true, get: function() { return m[k]; } });
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (Object.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
const core = __importStar(require("@actions/core"));
const replicate = __importStar(require("../replicate"));
const issues = __importStar(require("../issues"));
const TEST_ISSUE_1 = 1;
const TEST_REPOSITORY = {
    full_name: 'myuser/myrepo',
    name: 'myrepo',
    owner: {
        login: 'myuser',
        name: 'My User'
    }
};
const TEST_INVALID_PAYLOAD_NOUSER = {
    repository: TEST_REPOSITORY,
    issue: {
        number: TEST_ISSUE_1,
        html_url: 'https://github.com/test_owner/test_repo/issues/1',
    }
};
const TEST_INVALID_PAYLOAD_NOURL = {
    repository: TEST_REPOSITORY,
    issue: {
        number: TEST_ISSUE_1,
        user: {
            login: 'issue_user',
            html_url: 'https://github.com/users/issue_user'
        }
    }
};
const TEST_INVALID_PAYLOAD_NOISSUE = {
    repository: TEST_REPOSITORY,
};
const TEST_LABEL_ALLFORONE = { name: 'All For One' };
const TEST_LABEL_NOTBOUNTY_1 = { name: 'not-a-bounty-label' };
const TEST_LABEL_NOTBOUNTY_2 = { name: 'not-a-bounty-label-either' };
const TEST_PAYLOAD_NOTBOUNTY = {
    repository: TEST_REPOSITORY,
    issue: {
        number: TEST_ISSUE_1,
        html_url: 'https://github.com/test_owner/test_repo/issues/1',
        user: {
            login: 'issue_user',
            html_url: 'https://github.com/users/issue_user'
        },
        labels: [TEST_LABEL_NOTBOUNTY_1, TEST_LABEL_NOTBOUNTY_2],
    }
};
const TEST_PAYLOAD = {
    repository: TEST_REPOSITORY,
    issue: {
        number: TEST_ISSUE_1,
        html_url: 'https://github.com/test_owner/test_repo/issues/1',
        user: {
            login: 'ghsecuritylab',
            html_url: 'https://github.com/ghsecuritylab'
        },
        title: 'Issue Title',
        labels: [TEST_LABEL_ALLFORONE, TEST_LABEL_NOTBOUNTY_1],
        body: `# This is the issue title
This is the issue body first line
This is the issue body second line
`
    }
};
const TEST_GENERATED_ISSUE = {
    title: '[All For One] Issue Title',
    labels: ['All For One', 'not-a-bounty-label'],
    bountyType: 'All For One',
    body: `Original external [issue](https://github.com/test_owner/test_repo/issues/1)

Submitted by [ghsecuritylab](https://github.com/ghsecuritylab)

# This is the issue title
This is the issue body first line
This is the issue body second line
`
};
describe('log errors when generating issue content', () => {
    it('outputs a message for invalid issue in payload', async () => {
        const debugMock = jest.spyOn(core, 'debug');
        const issue = await replicate.generateInternalIssueContentFromPayload(TEST_INVALID_PAYLOAD_NOURL);
        expect(debugMock).toHaveBeenCalledWith('Invalid issue payload');
        expect(issue).toBeUndefined();
        const issue2 = await replicate.generateInternalIssueContentFromPayload(TEST_INVALID_PAYLOAD_NOUSER);
        expect(debugMock).toHaveBeenCalledWith('Invalid issue payload');
        expect(issue2).toBeUndefined();
        const issue3 = await replicate.generateInternalIssueContentFromPayload(TEST_INVALID_PAYLOAD_NOISSUE);
        expect(debugMock).toHaveBeenCalledWith('Invalid issue payload');
        expect(issue3).toBeUndefined();
    });
});
describe('excludes non bounty issues', () => {
    it('creates the proper issue', async () => {
        const debugMock = jest.spyOn(core, 'debug');
        const issue = await replicate.generateInternalIssueContentFromPayload(TEST_PAYLOAD_NOTBOUNTY);
        expect(debugMock).toHaveBeenCalledWith('Not a bounty');
        expect(issue).toBeUndefined();
    });
});
describe('generates proper content', () => {
    it('creates the proper issue', async () => {
        const issue = await replicate.generateInternalIssueContentFromPayload(TEST_PAYLOAD);
        expect(issue).toBeDefined();
        expect(issue).toEqual(TEST_GENERATED_ISSUE);
    });
});
describe('check for duplicates', () => {
    it('can find duplicates', async () => {
        const TEST_REF = 31;
        const TEST_BODY1 = `Original external [issue](https://github.com/owner/repo/issues/1)\n\nThen there is some text`;
        const TEST_BODY2 = `Original external [issue](https://github.com/owner/repo/issues/2)\n\nThen there is some text`;
        const TEST_INTERNAL_ISSUES = [
            { title: 'issue 1', author: 'author1', body: TEST_BODY1, number: 31 },
            { title: 'issue 2', author: 'author2', body: TEST_BODY2, number: 33 }
        ];
        let foundRef = issues.internalIssueAlreadyCreated('https://github.com/owner/repo/issues/1', TEST_INTERNAL_ISSUES);
        expect(foundRef).toEqual(TEST_REF);
        foundRef = issues.internalIssueAlreadyCreated('https://github.com/owner/repo/issues/3', TEST_INTERNAL_ISSUES);
        expect(foundRef).toBeUndefined();
    });
});
//# sourceMappingURL=replicate.test.js.map