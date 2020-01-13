import cpp
import common
import bindings
import field
import callbacks

/**
 * Dataflow library for tracking unretained or retained types in a callback.
 */

/**
 * An assignment to a callback field.
 */
predicate isCallbackFieldSink(DataFlow::Node sink) {
  exists(Field f | sink.asExpr() = generalAssignValue(f))
}

/**
 * Running of a callback.
 */
predicate runCallbackSink(DataFlow::Node sink) {
  exists(RunCallback cb | cb.getCallback() = sink.asExpr())
}

/**
 * An expression that posts a callback to a task runner.
 */
predicate postTaskSink(DataFlow::Node sink) {
  exists(FunctionCall postTask | postTask.getTarget().getName().matches("PostTask%") or
    postTask.getTarget().getName() = "PostDelayedTask" |
    postTask.getAnArgument() = sink.asExpr()
  )
}

/**
 * Callback gets passed inside an interface pointer function. The idea is that such a
 * callback may then be called from the renderer. (A bit like 
 * https://bugs.chromium.org/p/project-zero/issues/detail?id=1755
 * )
 */
predicate interfacePtrCallSink(DataFlow::Node sink) {
  exists(FunctionCall mojom, InterfacePtr iPtr, Function interfaceFunc | 
    overrides*(mojom.getTarget(), interfaceFunc) and
    interfaceFunc.getDeclaringType() = iPtr.getInterfaceType() and
    sink.asExpr() = mojom.getAnArgument()
  ) or
  exists(BindCall bc, InterfacePtr iPtr, Function interfaceFunc | 
    overrides*(bc.getFunction(), interfaceFunc) and interfaceFunc.getDeclaringType() = iPtr.getInterfaceType() and
    sink.asExpr() = bc.getAnArgument() and
    sink.asExpr() != bc.getArgument(0)
  )
}

/**
 * Callback that then binds to another callback as an argument.
 */
predicate callbackArgSink(DataFlow::Node sink) {
  exists(GeneralCallback cb | cb.getACallbackArg() = sink.asExpr())
}

class CallbackConfig extends DataFlow::Configuration {
  CallbackConfig() {
    this = "callbackconfig"
  }
  
  override predicate isSource(DataFlow::Node source) {
    (
      exists(GeneralCallback fc |
        source.asExpr() = fc
      )
      or
      exists(CallbackField f | source.asExpr() = f.getAnAccess())
    ) 
    and
    not source.asExpr().getFile().getBaseName() = "bind.h" and
    not source.asExpr().getFile().getBaseName() = "callback_helpers.h"// and
  }
  
  override predicate isSink(DataFlow::Node sink) {
    ( 
      isCallbackFieldSink(sink)
      or
      runCallbackSink(sink)
      or
      postTaskSink(sink)
      or
      interfacePtrCallSink(sink)
      or
      callbackArgSink(sink)
    ) and     
    (
      //Exclude sinks that are in uninteresting files.
      not sink.asExpr().getFile().getBaseName() = "bind_internal.h" and
      not sink.asExpr().getFile().getBaseName() = "tuple" and
      not sink.asExpr().getFile().getBaseName() = "memory" and
      not sink.asExpr().getFile().getAbsolutePath().matches("%/libc++/%") and
      not sink.asExpr().getFile().getBaseName() = "bind.h" and
      not sink.asExpr().getFile().getBaseName() = "binding_state.h" and
      not sink.asExpr().getFile().getBaseName() = "interface_endpoint_client.h" and
      not sink.asExpr().getFile().getBaseName() = "associated_interface_registry.h" and
      not sink.asExpr().getFile().getBaseName() = "callback_helpers.h" and
      not sink.asExpr().getFile().getBaseName() = "callback_list.h" and
      sink.asExpr().fromSource()
    )
  }
  
  override predicate isAdditionalFlowStep(DataFlow::Node node1, DataFlow::Node node2) {
    callbackStep(node1, node2) or
    collectionsEdge(node1, node2) or
    getEdge(node1, node2) or
    generalAssignEdge(node1, node2) or
    exists(Parameter p | p = node1.asParameter() and
      node2.asExpr() = p.getAnAccess()
    ) or
    copyConstructorEdge(node1, node2)
    or
    pointerTransferEdge(node1, node2)
    or
    adaptCallbackEdge(node1, node2)
    or
    callbackWrapperEdge(node1, node2)
    or
    callbackToBindEdge(node1, node2)
    or
    polymorphicCallEdge(node1, node2)
    or
    passEdge(node1, node2)
    or
    ownedEdge(node1, node2)
    or
    retainedRefEdge(node1, node2)
    or
    unretainedEdge(node1, node2)
    or
    forRangeEdge(node1, node2)
  }
}