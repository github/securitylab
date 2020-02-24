import cpp

from FunctionCall call
where call.getTarget().getName() = "amqp_tcp_socket_recv"
select call
