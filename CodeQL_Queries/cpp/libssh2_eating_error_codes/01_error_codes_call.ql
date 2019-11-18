/**
 * @name 01_error_codes_call
 */

import cpp

// Extend the previous query to also find calls to functions that sometimes
// return a negative integer constant.
from FunctionCall call, ReturnStmt ret
where
  ret.getExpr().getValue().toInt() < 0 and
  call.getTarget() = ret.getEnclosingFunction()
select ret, call
