/**
 * @name kzalloc only
 * @description If the copy_from_user is preceded by a kzalloc of the correct
 *              size, then it is safe. To demonstrate, find only those results.
 */

import cpp
import semmle.code.cpp.valuenumbering.GlobalValueNumbering
import semmle.code.cpp.dataflow.DataFlow

// Let's see if we can detect this pattern:
//
// ```
// buf = kzalloc(size, GFP_KERNEL);
// ...
// copy_from_user(buf, usrptr, size);
// ```
//
// In the next query, we'll use `safe_malloc` to filter those
// calls out, because they are safe.
predicate safe_malloc(FunctionCall allocCall, FunctionCall copy_from_user) {
  exists(DataFlow::Node source, DataFlow::Node sink |
    allocCall.getTarget().getName() = "kzalloc" and
    copy_from_user.getTarget().getName() = "copy_from_user" and
    source.asExpr() = allocCall and
    sink.asExpr() = copy_from_user.getArgument(0) and
    DataFlow::localFlow(source, sink) and
    globalValueNumber(allocCall.getArgument(0)) = globalValueNumber(copy_from_user.getArgument(2))
  )
}

from FunctionCall allocCall, FunctionCall copy_from_user
where safe_malloc(allocCall, copy_from_user)
select allocCall, copy_from_user
