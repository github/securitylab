import cpp

from Function fun, FunctionCall call
where
  fun.getName().matches("%ioctl%") and
  fun.hasDefinition() and
  call = fun.getACallToThisFunction()
select call.getEnclosingFunction(), call
