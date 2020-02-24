import cpp

class UnusedVariable extends LocalVariable {
  UnusedVariable() { not exists(VariableAccess access | access.getTarget() = this) }
}

from UnusedVariable unused
select unused
