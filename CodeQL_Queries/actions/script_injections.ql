/**
 * @name Command injection from user-controlled Actions context
 * @description Using user-controlled GitHub Actions contexts in a command line may allow a malicious
 *              user to change the meaning of the command.
 * @id java/actions/command-injection
 * @kind problem
 * @problem.severity error
 */

import javascript

/**
 * Libraries for modelling GitHub Actions workflow files written in YAML.
 * See https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions.
 */
module Actions {
  /** A YAML node in a GitHub Actions workflow file. */
  class Node extends YAMLNode {
    Node() { this.getLocation().getFile().getRelativePath().matches(".github/workflows/%") }
  }

  /**
   * An Actions workflow. This is a mapping at the top level of an Actions YAML workflow file.
   * See https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions.
   */
  class Workflow extends Node, YAMLDocument, YAMLMapping {
    /** Gets the `jobs` mapping from job IDs to job definitions in this workflow. */
    YAMLMapping getJobs() { result = this.lookup("jobs") }

    /** Gets the name of the workflow file. */
    string getFileName() { result = this.getFile().getBaseName() }

    /** Gets the `on:` in this workflow. */
    On getOn() { result = this.lookup("on") }

    /** Gets the job within this workflow with the given job ID. */
    Job getJob(string jobId) { result.getWorkflow() = this and result.getId() = jobId }
  }

  /**
   * An Actions job within a workflow.
   * See https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#jobs.
   */
  class Job extends YAMLNode, YAMLMapping {
    string jobId;
    Workflow workflow;

    Job() { this = workflow.getJobs().lookup(jobId) }

    /**
     * Gets the ID of this job, as a string.
     * This is the job's key within the `jobs` mapping.
     */
    string getId() { result = jobId }

    /**
     * Gets the ID of this job, as a YAML scalar node.
     * This is the job's key within the `jobs` mapping.
     */
    YAMLString getIdNode() { workflow.getJobs().maps(result, this) }

    /** Gets the human-readable name of this job, if any, as a string. */
    string getName() { result = this.getNameNode().getValue() }

    /** Gets the human-readable name of this job, if any, as a YAML scalar node. */
    YAMLString getNameNode() { result = this.lookup("name") }

    /** Gets the step at the given index within this job. */
    Step getStep(int index) { result.getJob() = this and result.getIndex() = index }

    /** Gets the sequence of `steps` within this job. */
    YAMLSequence getSteps() { result = this.lookup("steps") }

    /** Gets the workflow this job belongs to. */
    Workflow getWorkflow() { result = workflow }

    /** Gets the value of the `if` field in this job, if any. */
    JobIf getIf() { result.getJob() = this }
  }

  class JobIf extends YAMLNode, YAMLScalar {
    Job job;

    JobIf() {
      job.lookup("if") = this
    }

    /** Gets the step this field belongs to. */
    Job getJob() { result = job }
  }

  /**
   * A step within an Actions job.
   * See https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#jobsjob_idsteps.
   */
  class Step extends YAMLNode, YAMLMapping {
    int index;
    Job job;

    Step() { this = job.getSteps().getElement(index) }

    /** Gets the 0-based position of this step within the sequence of `steps`. */
    int getIndex() { result = index }

    /** Gets the job this step belongs to. */
    Job getJob() { result = job }

    /** Gets the value of the `uses` field in this step, if any. */
    Uses getUses() { result.getStep() = this }

    /** Gets the value of the `run` field in this step, if any. */
    Run getRun() { result.getStep() = this }

    /** Gets the value of the `if` field in this step, if any. */
    StepIf getIf() { result.getStep() = this }
  }

  class StepIf extends YAMLNode, YAMLScalar {
    Step step;

    StepIf() {
      step.lookup("if") = this
    }

    /** Gets the step this field belongs to. */
    Step getStep() { result = step }
  }

  /**
   * A `uses` field within an Actions job step, which references an action as a reusable unit of code.
   * See https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#jobsjob_idstepsuses.
   *
   * For example:
   * ```
   * uses: actions/checkout@v2
   * ```
   * TODO: Does not currently handle local repository references, e.g. `.github/actions/action-name`.
   */
  class Uses extends YAMLNode, YAMLScalar {
    Step step;
    /** The owner of the repository where the Action comes from, e.g. `actions` in `actions/checkout@v2`. */
    string repositoryOwner;
    /** The name of the repository where the Action comes from, e.g. `checkout` in `actions/checkout@v2`. */
    string repositoryName;
    /** The version reference used when checking out the Action, e.g. `v2` in `actions/checkout@v2`. */
    string version;

    Uses() {
      step.lookup("uses") = this and
      // Simple regular expression to split up an Action reference `owner/repo@version` into its components.
      exists(string regexp | regexp = "([^/]+)/([^/@]+)@(.+)" |
        repositoryOwner = this.getValue().regexpCapture(regexp, 1) and
        repositoryName = this.getValue().regexpCapture(regexp, 2) and
        version = this.getValue().regexpCapture(regexp, 3)
      )
    }

    /** Gets the step this field belongs to. */
    Step getStep() { result = step }

    /** Gets the owner and name of the repository where the Action comes from, e.g. `actions/checkout` in `actions/checkout@v2`. */
    string getGitHubRepository() { result = repositoryOwner + "/" + repositoryName }

    /** Gets the version reference used when checking out the Action, e.g. `v2` in `actions/checkout@v2`. */
    string getVersion() { result = version }
  }

  class MappingOrSequenceOrScalar extends YAMLNode {
    MappingOrSequenceOrScalar() {
      this instanceof YAMLMapping
      or
      this instanceof YAMLSequence
      or
      this instanceof YAMLScalar
    }

    YAMLNode getNode(string name) {
      exists(YAMLMapping mapping |
        mapping = this and
        result = mapping.lookup(name)
      )
      or
      exists(YAMLSequence sequence, YAMLNode node |
        sequence = this and
        sequence.getAChildNode() = node and
        node.eval().toString() = name and
        result = node
      )
      or
      exists(YAMLScalar scalar |
        scalar = this and
        scalar.getValue() = name and
        result = scalar
      )
    }

    int getElementCount() {
      exists(YAMLMapping mapping |
        mapping = this and
        result = mapping.getNumChild() / 2
      )
      or
      exists(YAMLSequence sequence |
        sequence = this and
        result = sequence.getNumChild()
      )
      or
      exists(YAMLScalar scalar |
        scalar = this and
        result = 1
      )
    }
  }

  class On extends YAMLNode, MappingOrSequenceOrScalar {
    Workflow workflow;

    On() { workflow.lookup("on") = this }

    Workflow getWorkflow() { result = workflow }
  }

  class With extends YAMLNode, YAMLMapping {
    Step step;

    With() { step.lookup("with") = this }

    /** Gets the step this field belongs to. */
    Step getStep() { result = step }
  }

  class Ref extends YAMLNode, YAMLString {
    With with;

    Ref() { with.lookup("ref") = this }

    With getWith() { result = with }
  }

  /**
   * A `run` field within an Actions job step, which runs command-line programs using an operating system shell.
   * See https://docs.github.com/en/free-pro-team@latest/actions/reference/workflow-syntax-for-github-actions#jobsjob_idstepsrun.
   */
  class Run extends YAMLNode, YAMLString {
    Step step;

    Run() { step.lookup("run") = this }

    /** Gets the step that executes this `run` command. */
    Step getStep() { result = step }

    /**
     * Holds if `${{ e }}` is a GitHub Actions expression evaluated within this `run` command.
     * See https://docs.github.com/en/free-pro-team@latest/actions/reference/context-and-expression-syntax-for-github-actions.
     */
    string getAReferencedExpression() {
      // We use `regexpFind` to obtain *all* matches of `${{...}}`,
      // not just the last (greedy match) or first (reluctant match).
      // TODO: This only handles expression strings that refer to contexts.
      // It does not handle operators within the expression.
      result =
        this.getValue().regexpFind("(?<=\\$\\{\\{\\s*)[A-Za-z0-9_\\.\\-]+(?=\\s*\\}\\})", _, _)
    }
  }
}

// TODO: Cannot yet treat these as DataFlow::Nodes, because YAMLValue is inconvertible to Expr.
/**
 * Holds if `child` is the qualified name of a GitHub Actions context nested as
 * a property of the GitHub Actions context with qualified name `parent`.
 * For example, `github.event.issue.body` is a child of `github.event.issue`.
 */
bindingset[child]
predicate nestedContext(string parent, string child) {
  parent = child.regexpCapture("([A-Za-z0-9_\\.\\-]+)\\.[A-Za-z0-9_\\-]+", 1)
}

bindingset[context]
private predicate isExternalUserControlledIssue(string context) {
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*issue\\s*\\.\\s*title\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*issue\\s*\\.\\s*body\\b")
}

bindingset[context]
private predicate isExternalUserControlledPullRequest(string context) {
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*pull_request\\s*\\.\\s*title\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*pull_request\\s*\\.\\s*body\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*pull_request\\s*\\.\\s*head\\s*\\.\\s*label\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*pull_request\\s*\\.\\s*head\\s*\\.\\s*repo\\s*\\.\\s*default_branch\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*pull_request\\s*\\.\\s*head\\s*\\.\\s*ref\\b")
}

bindingset[context]
private predicate isExternalUserControlledReview(string context) {
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*review\\s*\\.\\s*body\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*review_comment\\s*\\.\\s*body\\b")
}

bindingset[context]
private predicate isExternalUserControlledComment(string context) {
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*comment\\s*\\.\\s*body\\b")
}

bindingset[context]
private predicate isExternalUserControlledGollum(string context) {
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*pages(?:\\[[0-9]\\]|\\s*\\.\\s*\\*)+\\s*\\.\\s*page_name\\b")
}

/**
 * Holds if `context` is a GitHub Actions context object containing values
 * that may be controlled by an external user with public access to the repository.
 */
bindingset[context]
private predicate isExternalUserControlledCommit(string context) {
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*commits(?:\\[[0-9]\\]|\\s*\\.\\s*\\*)+\\s*\\.\\s*message\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*head_commit\\s*\\.\\s*message\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*head_commit\\s*\\.\\s*author\\s*\\.\\s*email\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*head_commit\\s*\\.\\s*author\\s*\\.\\s*name\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*commits(?:\\[[0-9]\\]|\\s*\\.\\s*\\*)+\\s*\\.\\s*author\\s*\\.\\s*email\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*event\\s*\\.\\s*commits(?:\\[[0-9]\\]|\\s*\\.\\s*\\*)+\\s*\\.\\s*author\\s*\\.\\s*name\\b") or
  context.regexpMatch("\\bgithub\\s*\\.\\s*head_ref\\b")
}

from Actions::Run run, string context, Actions::On on
where
  run.getAReferencedExpression() = context and
  run.getStep().getJob().getWorkflow().getOn() = on and (
    (
      exists(on.getNode("issues")) and
      isExternalUserControlledIssue(context)
    ) or
    (
      exists(on.getNode("pull_request_target")) and
      isExternalUserControlledPullRequest(context)
    ) or
    (
     (exists(on.getNode("pull_request_review_comment")) or exists(on.getNode("pull_request_review"))) and
     isExternalUserControlledReview(context)
    ) or
    (
      (exists(on.getNode("issue_comment")) or exists(on.getNode("pull_request_target"))) and
      isExternalUserControlledComment(context)
    ) or
    (
      exists(on.getNode("gollum")) and
      isExternalUserControlledGollum(context)
    ) or
    (
      exists(on.getNode("pull_request_target")) and
      isExternalUserControlledCommit(context)
    )
  )
select run, "Potential command injection from the " + context + " context, which may be controlled by an external user."
