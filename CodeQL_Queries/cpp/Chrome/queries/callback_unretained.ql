/**
 * @name callback_unretained
 * @description For each callback, get the types that are unretained in the callback.
 * @kind problem
 * @problem.severity warning
 */

import cpp
import callbacks
import object_lifetime.obj_lifetime

//Can change to different types of CallbackSinks for more specific investigation
from CallbackSinks sink, Type unretainedType
where not exists(FunctionCall fc | fc.getTarget().hasName("set_connection_error_handler") and
    fc.getAnArgument() = sink.asExpr()
  )
and not sink.asExpr().getFile().getBaseName().matches("%test-utils%") and
unretainedType = sink.getAnUnretainedType() and
not unretainedType instanceof Singleton and
not unretainedType instanceof OwnedByBrowserMainLoop

select sink, unretainedType
