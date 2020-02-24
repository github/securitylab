import cpp
import semmle.code.cpp.dataflow.TaintTracking

class NetworkBytes extends FieldAccess {
  NetworkBytes() {
    this.getQualifier().getType().getName() = "amqp_bytes_t" and
    this.getTarget().getName() = "bytes"
  }
}

from NetworkBytes bytes
select bytes
