import cpp
import common

/**
 * Library for mojo bindings.
 */

class StrongBinding extends ClassTemplateInstantiation {
  StrongBinding() {
    getName().matches("StrongBinding%")
  }
  
  Type getBindingType() {
    result = this.getTemplateArgument(0).stripType()
  }
}

class Binding extends ClassTemplateInstantiation {
  Binding() {
    getName().matches("Binding<%")
  }
  
  Type getBindingType() {
    result = this.getTemplateArgument(0).stripType()
  }
}


class MojoReceiver extends ClassTemplateInstantiation {
  MojoReceiver() {
    getQualifiedName().matches("%mojo::Receiver<%")
  }
  
  Type getBindingType() {
    result = this.getTemplateArgument(0).stripType()
  }
}

class InterfaceBinding extends Class {
  FunctionCall addBinding;
  
  InterfaceBinding() {
    addBinding.getTarget().hasName("AddBinding") and
    this = generalStripType(addBinding.getArgument(0).getAChild*().getType())
  }
  
  FunctionCall getABinding() {
    result = addBinding
  }
}

class InterfacePtr extends ClassTemplateInstantiation {
  InterfacePtr() {
    stripType().getName().matches("InterfacePtr<%") or
    stripType().getName().matches("InterfacePtrInfo<%")
  }
  
  Type getInterfaceType() {
    result = getTemplateArgument(0)
  }
  
  Type getInterfacePtrType() {
    exists(string s | s = getInterfaceType().getName() + "Ptr" and
      result.getName() = s
    )
  }
}

class SetConnectionErrorHandler extends Function {
  SetConnectionErrorHandler() {
    getName() = "set_connection_error_handler" or
    getName() = "set_connection_error_with_reason_handler"
  }
}

/**
 * `StructPtr` usually use for transporting data in IPC.
 */
class StructPtr extends Class {
  StructPtr() {
    getName().matches("StructPtr<%") or
    getName().matches("InlinedStructPtr<%")
  }
  
  Type getStructType() {
    result = getTemplateArgument(0)
  }
}

Type stripStructPtrType(Type c) {
  (
    c.getName().matches("vector<%") and
    result = stripStructPtrType(c.(Class).getTemplateArgument(0))
  ) or
  exists(StructPtr t | t = c.stripType() and
    result = t.getStructType().stripType()
  ) or
  result = c.stripType()
}
