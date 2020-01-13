/**
 * @name callback_unretained_field
 * @description For each callback field, get the types that are unretained in the callback. (specialized version of callback_unretained)
 * @kind problem
 * @problem.severity warning
 */

import cpp
import callbacks
import object_lifetime.obj_lifetime

from CallbackFieldSink sink, Field f, Type unretainedType
//Heuristics from observation
where not exists(FunctionCall fc | fc.getTarget().hasName("set_connection_error_handler") and
    fc.getAnArgument() = sink.asExpr()
  )
and not sink.asExpr().getFile().getBaseName().matches("%test-utils%")
and sink.asExpr() = generalAssignValue(f) and
unretainedType = sink.getAnUnretainedType() and
not unretainedType instanceof Singleton and
not unretainedType instanceof OwnedByBrowserMainLoop
select f, sink, unretainedType
