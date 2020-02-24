import cpp

from FunctionCall call, Expr child
where
  call.getTarget().getName() = "kmalloc" and
  child = call.getAnArgument().getAChild*() and
  child instanceof SizeofOperator
select call, child
