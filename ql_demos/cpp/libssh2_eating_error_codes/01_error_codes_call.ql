/**
 * @name 01_error_codes_call
 */

import cpp

from Function f, FunctionCall call, ReturnStmt ret
where call.getTarget() = f
and ret.getEnclosingFunction() = f
and ret.getExpr().getValue().toInt() < 0
select call, ret
