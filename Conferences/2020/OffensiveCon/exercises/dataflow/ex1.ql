import cpp
import semmle.code.cpp.dataflow.DataFlow

class KMalloc extends Function {
  KMalloc() { getName() = "kmalloc" }
}

from KMalloc fun, FunctionCall source
where
  source = fun.getACallToThisFunction() and
  not exists(IfStmt sink | DataFlow::localExprFlow(source, sink.getControllingExpr()))
select source
