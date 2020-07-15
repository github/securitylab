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
exports.internalIssueAlreadyCreated = exports.isUserAlreadyParticipant = exports.getIssueList = void 0;
const core = __importStar(require("@actions/core"));
const github = __importStar(require("@actions/github"));
const replicate = __importStar(require("./replicate"));
exports.getIssueList = async (owner, repo, token, open, checkBountyLabels, per_page) => {
    if (!token) {
        core.debug("No valid token for creating issues on the internal repo");
        return;
    }
    try {
        let result = [];
        const octokit = new github.GitHub(token);
        const issueState = open ? 'open' : 'all';
        // const labelFilter: string = replicate.BOUNTY_LABELS.join(',')
        const issues = await octokit.issues.listForRepo({
            owner,
            repo,
            state: issueState,
            per_page: per_page ? per_page : 100 // TODO: implement proper pagination
            // labels: labelFilter -- Does not work properly
        });
        issues.data.forEach(issue => {
            var _a;
            const bountyLabel = checkBountyLabels ? issue.labels.some(label => {
                return replicate.BOUNTY_LABELS.includes(label.name);
            }) : undefined;
            if (!checkBountyLabels || bountyLabel) {
                let item = {
                    title: issue.title,
                    author: (_a = issue.user) === null || _a === void 0 ? void 0 : _a.login,
                    body: issue.body ? issue.body : '',
                    number: issue.number,
                    html_url: issue.html_url
                };
                result.push(item);
            }
        });
        return result;
    }
    catch (error) {
        core.debug(error.message);
        return undefined;
    }
};
exports.isUserAlreadyParticipant = (user, externalSubmissions) => {
    if (!externalSubmissions)
        return false;
    const check = externalSubmissions.some(element => {
        return (element.author === user);
    });
    return check;
};
function escapeRegExp(text) {
    return text.replace(/[-\/\\^$*+?.()|[\]{}]/g, '\\$&');
}
exports.internalIssueAlreadyCreated = (externalSubmissionUrl, internalIssues) => {
    const searchString = new RegExp(escapeRegExp(`Original external [issue](${externalSubmissionUrl})`));
    let ref = undefined;
    internalIssues.some(element => {
        if (element.body.search(searchString) != -1) {
            ref = element.number;
            return true;
        }
    });
    return ref;
};
//# sourceMappingURL=issues.js.map