import cpp

from Function fun
where fun.getName().matches("%ioctl%") and fun.hasDefinition()
select fun
