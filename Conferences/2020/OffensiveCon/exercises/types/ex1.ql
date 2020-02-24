import cpp

from FunctionCall call
where call.getAnArgument().getAChild*() instanceof SizeofOperator
select call
