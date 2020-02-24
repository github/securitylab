import cpp

from FunctionCall call, Expr size
where
  call.getTarget().getName() = "kmalloc" and
  size = call.getArgument(0)
select call, size, size.getValue().toInt()
