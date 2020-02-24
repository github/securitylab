import cpp

from VariableAccess access
where access.getTarget().getName() = "current_task"
select access.getEnclosingFunction()
