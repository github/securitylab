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
const github = __importStar(require("@actions/github"));
const issues_1 = require("../replicate/issues");
const run = async () => {
    const internalRepoAccessToken = process.env['INT_REPO_TOKEN'];
    const internalRepo = core.getInput('internal_repo') || '/';
    const [owner, repo] = internalRepo.split('/');
    const internalIssues = await issues_1.getIssueList(owner, repo, internalRepoAccessToken, false, false);
    if (!internalIssues) {
        core.setFailed(`Internal error. Cannot access the internal repo ${internalRepo}. Aborting`);
        return;
    }
    else {
        const externalIssues = await issues_1.getIssueList(github.context.repo.owner, github.context.repo.repo, process.env['GITHUB_TOKEN'], true, true);
        if (!externalIssues) {
            core.setFailed(`Internal error when retrieving all issues.`);
            return;
        }
        let failed = false;
        externalIssues.forEach(issue => {
            const ref = issues_1.internalIssueAlreadyCreated(issue === null || issue === void 0 ? void 0 : issue.html_url, internalIssues);
            if (!ref) {
                core.debug(`External issue ${issue === null || issue === void 0 ? void 0 : issue.number} is not replicated internally.`);
                failed = true;
            }
        });
        if (failed) {
            core.setFailed("Some submissions are not replicated internally, see execution logs.");
        }
    }
    return;
};
run();
//# sourceMappingURL=check-replication.js.map