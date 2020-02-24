import cpp

from FunctionCall call, Expr size
where
  call.getTarget().getName() = "kmalloc" and
  size = call.getArgument(0) and
  size instanceof MulExpr
select call, size
