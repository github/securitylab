/**
 * @name 04_eating_error_codes_localflow_rangeanalysis
 */

import cpp
import semmle.code.cpp.dataflow.DataFlow
import semmle.code.cpp.rangeanalysis.SimpleRangeAnalysis

// The previous query produced some weird results. The problem is that it
// treats any expression with an unsigned type as a potential sink. What we
// really want is to find where the cast from signed to unsigned happens,
// because that's where the integer overflow occurs. So we want the sink to
// be a potentially negative expression that gets cast to unsigned.
//
// Note that by using range analysis, we can avoid producing false positive
// results for examples like this:
//
//   int r = f();
//   if (r < 0) return -1;
//   unsigned int x = r;
from FunctionCall call, ReturnStmt ret, DataFlow::Node source, DataFlow::Node sink
where
  ret.getExpr().getValue().toInt() < 0 and
  call.getTarget() = ret.getEnclosingFunction() and
  source.asExpr() = call and
  DataFlow::localFlow(source, sink) and
  sink.asExpr().getFullyConverted().getType().getUnderlyingType().(IntegralType).isUnsigned() and
  lowerBound(sink.asExpr()) < 0
select source, sink
