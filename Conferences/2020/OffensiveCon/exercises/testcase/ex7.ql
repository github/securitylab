import cpp

class TargetFunction extends Function {
  TargetFunction() { this.getName() = "amqp_pool_alloc_bytes" }
}

from TargetFunction fun
select fun.getACallToThisFunction()
