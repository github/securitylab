/**
 * @name 04_eating_error_codes_localflow_rangeanalysis
 */

import cpp
import semmle.code.cpp.dataflow.DataFlow
import semmle.code.cpp.rangeanalysis.SimpleRangeAnalysis

// int r = f();
// if (r < 0) return -1;
// unsigned int x = r;

from Function f, FunctionCall call, ReturnStmt ret, DataFlow::Node source, DataFlow::Node sink
where call.getTarget() = f
and ret.getEnclosingFunction() = f
and ret.getExpr().getValue().toInt() < 0
and source.asExpr() = call
and DataFlow::localFlow(source, sink)
and sink.asExpr().getFullyConverted().getType().getUnderlyingType().(IntegralType).isUnsigned()
and lowerBound(sink.asExpr()) < 0
select source, sink
