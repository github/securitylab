import cpp

from Struct struct, Field recv
where
  struct.getName() = "amqp_socket_class_t" and
  recv = struct.getAField() and
  recv.getName() = "recv"
select struct, recv.getAnAccess()
