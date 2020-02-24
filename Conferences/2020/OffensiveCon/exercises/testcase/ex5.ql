import cpp

from FunctionCall call
where call.getTarget().getName() = "amqp_socket_recv"
select call
