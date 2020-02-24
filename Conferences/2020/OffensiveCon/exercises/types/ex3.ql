import cpp

from FunctionCall call, Expr child, Expr sized
where
  call.getTarget().getName() = "kmalloc" and
  child = call.getAnArgument().getAChild*() and
  child instanceof SizeofOperator and
  sized = child.getChild(0) and
  sized.getUnderlyingType() instanceof PointerType
select call, sized, sized.getUnderlyingType()
