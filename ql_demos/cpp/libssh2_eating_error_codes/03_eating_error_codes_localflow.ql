/**
 * @name 03_eating_error_codes_localflow
 */

import cpp
import semmle.code.cpp.dataflow.DataFlow

// The previous query only handled cases where the result of the function
// call is immediately cast to unsigned. So it will fail to detect examples
// like this, where the cast doesn't happen immediately:
//
//   int r = f();
//   unsigned int x = r;
//
// In this query, we add local dataflow so that we can also handle such
// cases.
from FunctionCall call, ReturnStmt ret, DataFlow::Node source, DataFlow::Node sink
where
  ret.getExpr().getValue().toInt() < 0 and
  call.getTarget() = ret.getEnclosingFunction() and
  source.asExpr() = call and
  DataFlow::localFlow(source, sink) and
  sink.asExpr().getFullyConverted().getType().getUnderlyingType().(IntegralType).isUnsigned()
select source, sink
