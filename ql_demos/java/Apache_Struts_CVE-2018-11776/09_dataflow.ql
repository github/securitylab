/**
 * @name 09_dataflow
 * @kind path-problem
 */

import java
import semmle.code.java.dataflow.DataFlow
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
}

/* First version of the dataflow query. We use isActionProxySource
 * as the source and isOgnlSink as the sink.
 */
from OgnlCfg cfg, DataFlow::PathNode source, DataFlow::PathNode sink
where cfg.hasFlowPath(source, sink)
select source, source, sink, "ognl"
