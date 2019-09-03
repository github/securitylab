/**
 * @name 01_error_codes_call
 */

import cpp

// Extend the previous query to also find calls to functions that sometimes
// return a negative integer constant.
from Function f, FunctionCall call, ReturnStmt ret
where
  call.getTarget() = f and
  ret.getEnclosingFunction() = f and
  ret.getExpr().getValue().toInt() < 0
select ret, call
