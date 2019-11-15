/**
 * @name 11_dataflow_with_additional_flow_step
 * @kind path-problem
 */

import java
import semmle.code.java.dataflow.DataFlow
import semmle.code.java.dataflow.TaintTracking
import DataFlow::PathGraph

predicate isOgnlSink(Expr arg) {
  exists (Method m, MethodAccess ma
  | m.getName() = "compileAndExecute" and
    ma.getMethod() = m and
    arg = ma.getArgument(0))
}

predicate isActionProxySource(MethodAccess ma) {
  exists (Method m, Method n
  | m.getName() = "getNamespace" and
    m.getDeclaringType().getName() = "ActionProxy" and
    n.overrides*(m) and
    ma.getMethod() = n)
}

class OgnlCfg extends DataFlow::Configuration {
  OgnlCfg() { this = "ognl" }

  override predicate isSource(DataFlow::Node source) {
    isActionProxySource(source.asExpr())
  }

  override predicate isSink(DataFlow::Node sink) {
    isOgnlSink(sink.asExpr())
  }

  override predicate isBarrier(DataFlow::Node node) {
    node.getEnclosingCallable().getDeclaringType().getName() = "ValueStackShadowMap"
  }

  override predicate isAdditionalFlowStep(DataFlow::Node node1, DataFlow::Node node2) {
    TaintTracking::localTaintStep(node1, node2)
  }
}

/* The previous query found one good result. The query described in
 * Mo's blog post finds more results than that. What's the difference?
 *
 * The difference is that Mo's query includes more dataflow steps.
 * The default DataFlow configuration is very conservative. It only
 * tracks dataflow when the value is passed unmodified from one
 * method to the next. But if the value is modified in any way, even
 * something very simple like adding 1 to an integer, then it will
 * stop tracking. Often we want to continue tracking the flow in
 * such cases. We can do that by adding extra flow steps in the
 * configuration. In this query, we have added "local taint" edges.
 * Now we get a second result (in `ActionChainResult.java`), which
 * is one of the serious RCE vulnerabilities.
 *
 * The full query adds even more additional dataflow steps, which is
 * why it finds 10 results, rather than just 2.
 */
from OgnlCfg cfg, DataFlow::PathNode source, DataFlow::PathNode sink
where cfg.hasFlowPath(source, sink)
select source, source, sink, "ognl"
