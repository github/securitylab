/**
 * @name 02_eating_error_codes
 */

import cpp

from Function f, FunctionCall call, ReturnStmt ret
where call.getTarget() = f
and ret.getEnclosingFunction() = f
and ret.getExpr().getValue().toInt() < 0
and call.getFullyConverted().getType().getUnderlyingType().(IntegralType).isUnsigned()
select call, ret