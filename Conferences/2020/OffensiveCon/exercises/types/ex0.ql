import cpp

from FunctionCall call
where call.getAnArgument() instanceof SizeofOperator
select call
