/**
 * @name 00_error_codes
 */

import cpp

from ReturnStmt ret
where ret.getExpr().getValue().toInt() < 0
select ret