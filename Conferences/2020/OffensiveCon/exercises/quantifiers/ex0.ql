import cpp

class UnusedFunction extends Function {
  UnusedFunction() {
    this.hasDefinition() and
    not exists(FunctionCall call | call.getTarget() = this) and
    not exists(FunctionAccess access | access.getTarget() = this)
  }
}

from UnusedFunction unused
select unused
