/**
 * @name PrivilegeDroppingOutoforder
 * @kind problem
 * @problem.severity recommendation
 * @id cpp/drop-permissions-outoforder
 * @tags security
 *       external/cwe/cwe-273
 */

import cpp
import semmle.code.cpp.dataflow.TaintTracking

class SetuidLikeFunctionCall extends FunctionCall {
  SetuidLikeFunctionCall() {
    // setuid/setresuid with the root user are false positives.
    getTarget().getQualifiedName() = "setuid" or
    getTarget().getQualifiedName() = "setresuid"
  }
}

class CallBeforeSetuidFunctionCall extends FunctionCall {
  CallBeforeSetuidFunctionCall() {
    // setgid/setresgid with the root group are false positives.
    getTarget().getQualifiedName() = "setgid" or
    getTarget().getQualifiedName() = "setresgid" or
    // Compatibility may require skipping initgroups and setgroups return checks.
    // A stricter best practice is to check the result and errnor for EPERM.
    getTarget().getQualifiedName() = "initgroups" or
    getTarget().getQualifiedName() = "setgroups" or
    // Find variants of CVE-2017-11747 where the low-priv user can stop the process.
    // Feel free to extend this with other variants.
    getTarget().getQualifiedName() = "pidfile_create"
  }

  predicate isCalledAfter(SetuidLikeFunctionCall fc) {
    exists(ControlFlowNode cfn |
        cfn = this.getAPredecessor+() or
        // Expressions before enclosing function.
        cfn = callerPredecessor(this.getEnclosingFunction().getACallToThisFunction()) |
        fc = calls(cfn, this)
    )
  }

  private FunctionCall calls(ControlFlowNode cfn, FunctionCall guard) {
    result = controlFlowElementIs(cfn) or
    result = controlFlowElementContains(cfn, guard) or
    result = controlFlowElementCalls(cfn, guard)
  }

  private predicate callsSelf(ControlFlowNode cfn, ControlFlowNode caller) {
    cfn = caller or
    exists (ControlFlowNode subnode |
        subnode = cfn.(Block).getAChild+() or
        subnode = cfn.(FunctionCall).getTarget().getBlock().getAChild+() |
        subnode = caller or callsSelf(subnode, caller)
    )
  }

  private ControlFlowNode callerPredecessor(ControlFlowNode caller) {
    exists(ControlFlowNode cfn |
      cfn = caller.getEnclosingStmt().getAPredecessor+() |
      not callsSelf(cfn, caller) and
      result = cfn
    )
  }

  private FunctionCall controlFlowElementContains(ControlFlowNode cfn, FunctionCall guard) {
    guard.getBasicBlock() != cfn.getBasicBlock() and
    exists (ControlFlowNode subnode |
      subnode = cfn.getBasicBlock().getANode+() |
      result = calls(subnode, guard)
    )
  }

  private FunctionCall controlFlowElementCalls(ControlFlowNode cfn, FunctionCall guard) {
    exists(ControlFlowNode subnode |
      subnode = cfn.(FunctionCall).getTarget().getBlock().getAChild+() |
      result = calls(subnode, guard)
    )
  }

  private FunctionCall controlFlowElementIs(SetuidLikeFunctionCall cfn) {
    result = cfn
  }
}

predicate withinCondition(Expr t) {
  exists(Expr src | src = t.getParent+() | src.isCondition()) or t.isCondition()
}

predicate flowsToCondition(Expr fc) {
  exists(DataFlow::Node source, DataFlow::Node sink |
    TaintTracking::localTaint(source, sink) and
    fc = source.asExpr() and
    withinCondition(sink.asExpr())
  )
}

from
  CallBeforeSetuidFunctionCall func,
  Function funcParent,
  SetuidLikeFunctionCall setuid
where
  func.isCalledAfter(setuid) and
  // Require the call return code to be used in a condition.
  // This introduces false negatives where the return is checked but then
  // errno == EPERM allows execution to continue.
  (not flowsToCondition(func) or not flowsToCondition(setuid)) and
  funcParent = func.getEnclosingFunction()
select func, "This function is called within " + funcParent + ", and potentially after " +
  "setuid/setresuid, and may not succeed. Be sure to check the return code and errno",
  setuid
