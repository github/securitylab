import cpp

from FunctionAccess access
where access.getTarget().getName() = "amqp_tcp_socket_recv"
select access
