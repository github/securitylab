import cpp
import semmle.code.cpp.dataflow.TaintTracking

class NetworkBytes extends FieldAccess {
  NetworkBytes() {
    this.getQualifier().getType().getName() = "amqp_bytes_t" and
    this.getTarget().getName() = "bytes"
  }
}

class TargetFunction extends Function {
  TargetFunction() { this.getName() = "amqp_pool_alloc_bytes" }
}

class Config extends TaintTracking::Configuration {
  Config() { this = "rabbitmq-c" }

  override predicate isSource(DataFlow::Node source) { source.asExpr() instanceof NetworkBytes }

  override predicate isSink(DataFlow::Node sink) {
    exists(TargetFunction function, FunctionCall call |
      call = function.getACallToThisFunction() and
      call.getAnArgument() = sink.asExpr()
    )
  }
}

from Config config, DataFlow::Node source, DataFlow::Node sink
where config.hasFlow(source, sink)
select source, sink
