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
exports.isFirstSubmission = exports.createInternalIssue = exports.generateInternalIssueContentFromPayload = exports.BOUNTY_LABELS = void 0;
const core = __importStar(require("@actions/core"));
const github = __importStar(require("@actions/github"));
const issues_1 = require("./issues");
exports.BOUNTY_LABELS = ['All For One', 'The Bug Slayer'];
const COMMENT_TASK_LIST_AFO = `## Task List
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
`;
const COMMENT_TASK_LIST_BS = `## Task List
- [ ] Security Lab assessment:
  - [ ] Acceptance
  - [ ] Confirm the CVE 
  - [ ] Assess the Vulnerability Impact, the Vulnerability Scope
  - [ ] Get the CodeQL scores (False Positive ratio, Code Maturity and the Documentation) from the previous query rating
  - [ ] PR is merged? Finalize the score
- [ ] Bounty Payment`;
const COMMENT_TASK_LIST = {
    'All For One': COMMENT_TASK_LIST_AFO,
    'The Bug Slayer': COMMENT_TASK_LIST_BS
};
const COMMENT_SCORING = `## Scoring
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
`;
const COMMENT_FIRST_SUBMISSION = `## :tada: First submission for this user :tada:`;
const getIssueFromRef = async (issueRef) => {
    if (!issueRef)
        return undefined;
    const token = process.env['GITHUB_TOKEN'];
    if (token === undefined)
        return undefined;
    const octokit = new github.GitHub(token);
    const issueResponse = await octokit.issues.get({
        owner: github.context.repo.owner,
        repo: github.context.repo.repo,
        issue_number: Number(issueRef),
    });
    return issueResponse.data;
};
exports.generateInternalIssueContentFromPayload = async (payload, issueRef) => {
    const issue = await getIssueFromRef(issueRef) || (payload === null || payload === void 0 ? void 0 : payload.issue);
    let result = { title: 'none', body: 'none', labels: [], bountyType: 'All For One' };
    let bountyIssue = false;
    if (!issue || !issue.user || !issue.html_url) {
        core.debug("Invalid issue payload");
        return undefined;
    }
    issue.labels.forEach((element) => {
        result.labels.push(element.name);
        if (!bountyIssue) {
            bountyIssue = exports.BOUNTY_LABELS.includes(element.name);
            if (bountyIssue) {
                result.bountyType = element.name;
            }
        }
    });
    if (!bountyIssue) {
        core.debug("Not a bounty");
        return undefined;
    }
    result.title = `[${result.bountyType}] ${issue.title}`,
        // In order to differentiate immediately the issues from others in the repo
        // And with the current situation, the robot with Read access cannot add labels to the issue
        result.body = `Original external [issue](${issue.html_url})

Submitted by [${issue.user.login}](${issue.user.html_url})

${issue.body ? issue.body : ""}`;
    return result;
};
exports.createInternalIssue = async (payload, issue) => {
    const internalRepoAccessToken = process.env['INT_REPO_TOKEN'];
    const token = process.env['GITHUB_TOKEN'];
    let internal_ref = undefined;
    if (!internalRepoAccessToken) {
        core.debug("No valid token for creating issues on the internal repo");
        return;
    }
    try {
        const octokit = new github.GitHub(internalRepoAccessToken);
        const internalRepo = core.getInput('internal_repo') || '/';
        const [owner, repo] = internalRepo.split('/');
        const issueResponse = await octokit.issues.create({
            owner,
            repo,
            title: issue.title,
            body: issue.body,
            labels: issue.labels
        });
        internal_ref = issueResponse.data.number;
        core.debug(`issue created: ${internal_ref}`);
        const issueCommentResponse1 = await octokit.issues.createComment({
            owner,
            repo,
            issue_number: internal_ref,
            body: COMMENT_TASK_LIST[issue.bountyType],
        });
        core.debug(`comment created ${issueCommentResponse1.data.url}`);
        const issueCommentResponse2 = await octokit.issues.createComment({
            owner,
            repo,
            issue_number: internal_ref,
            body: COMMENT_SCORING,
        });
        core.debug(`comment created ${issueCommentResponse2.data.url}`);
        if (await exports.isFirstSubmission(payload, token)) {
            const issueCommentResponse3 = await octokit.issues.createComment({
                owner,
                repo,
                issue_number: internal_ref,
                body: COMMENT_FIRST_SUBMISSION,
            });
            core.debug(`comment created ${issueCommentResponse3.data.url}`);
        }
    }
    catch (error) {
        core.debug(error.message);
    }
    return internal_ref;
};
const commentOriginalIssue = async (payload, internal_issue) => {
    const repository = payload.repository;
    const external_issue = payload.issue ? payload.issue.number : 0;
    const token = process.env['GITHUB_TOKEN'];
    if (!token) {
        core.debug("No valid token for this repo");
        return;
    }
    if (!repository || external_issue <= 0) {
        core.debug("Invalid payload");
        return;
    }
    try {
        const octokit = new github.GitHub(token);
        const issueCommentResponseOriginal = await octokit.issues.createComment({
            owner: repository.owner.login,
            repo: repository.name,
            issue_number: external_issue,
            body: `Thanks for submitting this bounty :heart:!
            Your submission is tracked internally with the issue reference ${internal_issue}.`,
        });
        core.debug(`comment created ${issueCommentResponseOriginal.data.url}`);
    }
    catch (error) {
        core.debug(error.message);
    }
};
const checkDuplicates = async (payload) => {
    var _a;
    const internalRepoAccessToken = process.env['INT_REPO_TOKEN'];
    const internalRepo = core.getInput('internal_repo') || '/';
    const [owner, repo] = internalRepo.split('/');
    const internalIssues = await issues_1.getIssueList(owner, repo, internalRepoAccessToken, false, false);
    if (!internalIssues) {
        core.debug('Internal error. Cannot check for duplicates. Aborting');
        return true;
    }
    else {
        const ref = issues_1.internalIssueAlreadyCreated((_a = payload.issue) === null || _a === void 0 ? void 0 : _a.html_url, internalIssues);
        if (ref) {
            core.debug(`This issue has already been duplicated with reference ${ref}`);
            return true;
        }
    }
    return false;
};
exports.isFirstSubmission = async (payload, token) => {
    var _a;
    const repository = payload.repository;
    if (!repository)
        return false;
    const allSubmissions = await issues_1.getIssueList(repository.owner.login, repository.name, token, false, true);
    return !issues_1.isUserAlreadyParticipant((_a = payload.issue) === null || _a === void 0 ? void 0 : _a.user.login, allSubmissions);
};
const run = async () => {
    const internalIssue = await exports.generateInternalIssueContentFromPayload(github.context.payload, core.getInput('specific_issue'));
    if (!internalIssue)
        return;
    const existingIssue = core.getInput('existingIssue') || true;
    if (existingIssue && await checkDuplicates(github.context.payload))
        return;
    const internal_ref = await exports.createInternalIssue(github.context.payload, internalIssue);
    if (!internal_ref)
        return;
    if (!existingIssue) {
        commentOriginalIssue(github.context.payload, internal_ref);
    }
};
run();
//# sourceMappingURL=replicate.js.map