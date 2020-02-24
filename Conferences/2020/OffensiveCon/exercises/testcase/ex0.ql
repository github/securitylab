import cpp

from FunctionCall call
where call.getTarget().getName() = "recv"
select call
