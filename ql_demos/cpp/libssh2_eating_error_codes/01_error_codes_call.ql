/**
 * @name 01_error_codes_call
 */

import cpp

// Extend the previous query to also find calls to functions that sometimes
// return a negative integer constant.
from Function f, FunctionCall call, ReturnStmt ret
where
  ret.getExpr().getValue().toInt() < 0 and
  ret.getEnclosingFunction() = f and
  call.getTarget() = f
select ret, call
