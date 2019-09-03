/**
 * @name 00_error_codes
 */

import cpp

// Look for return statements that return a negative integer constant.
// For example:
//
//   return -1;
//
// The negative return value might be an error code.
from ReturnStmt ret
where ret.getExpr().getValue().toInt() < 0
select ret
