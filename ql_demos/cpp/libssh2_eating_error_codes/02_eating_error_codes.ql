/**
 * @name 02_eating_error_codes
 */

import cpp

// Look for calls that are cast to unsigned, which means that the error
// code might be accidentally ignored.
from FunctionCall call, ReturnStmt ret
where
  ret.getExpr().getValue().toInt() < 0 and
  call.getTarget() = ret.getEnclosingFunction() and
  call.getFullyConverted().getType().getUnderlyingType().(IntegralType).isUnsigned()
select call, ret
