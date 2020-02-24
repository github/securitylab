import cpp
import semmle.code.cpp.dataflow.DataFlow

class KMalloc extends Function {
  KMalloc() { getName() = "kmalloc" }
}

from KMalloc fun, FunctionCall source, Expr sink
where
  source = fun.getACallToThisFunction() and
  DataFlow::localExprFlow(source, sink)
select source, sink, sink.getEnclosingStmt()
