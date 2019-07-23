/**
 * @name 03_eating_error_codes_localflow
 */

import cpp
import semmle.code.cpp.dataflow.DataFlow

// int r = f();
// unsigned int x = r;

from Function f, FunctionCall call, ReturnStmt ret, DataFlow::Node source, DataFlow::Node sink
where call.getTarget() = f
and ret.getEnclosingFunction() = f
and ret.getExpr().getValue().toInt() < 0
and source.asExpr() = call
and DataFlow::localFlow(source, sink)
and sink.asExpr().getFullyConverted().getType().getUnderlyingType().(IntegralType).isUnsigned()
select source, sink