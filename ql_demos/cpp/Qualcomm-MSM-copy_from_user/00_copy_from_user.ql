/**
 * @name Calls to copy_from_user
 * @description Find all calls to copy from user.
 * @problem.severity warning
 */

import cpp
import semmle.code.cpp.rangeanalysis.SimpleRangeAnalysis

// This first query is essentially equivalent to `grep -r copy_from_user`.
// It has almost 1300 results.
from FunctionCall call
where call.getTarget().getName() = "copy_from_user"
select call
