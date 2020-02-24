import cpp

from Function fun, FunctionAccess access
where
  fun.getName().matches("%ioctl%") and
  access = fun.getAnAccess()
select access, fun
