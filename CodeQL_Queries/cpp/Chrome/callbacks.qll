import cpp
import semmle.code.cpp.dataflow.TaintTracking
import callback_tracking
import bindings
import collections
import pointers.managed_ptr
import pointers.raw_ptr
import common

//-------- dataflow edges

predicate callbackStep(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc instanceof BindCall |
    exists(FunctionAccess fa, Function f, int i, int j | 
      fa.getTarget() = f and fa = fc.getArgument(0).getAChild*() and
      (
        if (f.isStatic() or not exists(f.getDeclaringType()))then
          j = i + 1
        else
          j = i + 2
      )
      and
      node1.asExpr() = fc.getArgument(j) and
      node2.asParameter() = f.getParameter(i)
    )
    or
    exists(LambdaExpression lambda, int i, int j |
      j = i + 1 and
      lambda = fc.getArgument(0) and
      node1.asExpr() = fc.getArgument(j) and
      node2.asParameter() = lambda.getLambdaFunction().getParameter(i)
    )
  )
}

predicate unretainedEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().getName() = "Unretained" or
    fc.getTarget().getName() = "UnretainedWrapper" |
    fc.getAnArgument() = node1.asExpr() and
    fc = node2.asExpr()
  )
}

predicate retainedRefEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().getName() = "RetainedRef" or
    fc.getTarget().getName() = "RetainedRefWrapper" |
    fc.getAnArgument() = node1.asExpr() and
    fc = node2.asExpr()
  )
}

predicate passEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().getQualifiedName() = "base::Passed" or
    fc.getTarget().getName() = "PassedRefWrapper" |
    fc.getAnArgument() = node1.asExpr() and
    fc = node2.asExpr()
  )  
}

predicate ownedEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().getQualifiedName() = "base::Owned" or
    fc.getTarget().getName() = "OwnedWrapper" |
    fc.getAnArgument() = node1.asExpr() and
    fc = node2.asExpr()
  )  
}

predicate adaptCallbackEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().hasName("AdaptCallbackForRepeating") and
    node1.asExpr() = fc.getAnArgument() and node2.asExpr() = fc
  )
}

predicate callbackWrapperEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().hasName("OnceCallback") or 
    fc.getTarget().hasName("RepeatingCallback") |
    node1.asExpr() = fc.getAnArgument() and
    node2.asExpr() = fc
  ) or
  exists(FunctionCall fc | fc.getTarget().hasName("BarrierClosure") and
    node1.asExpr() = fc.getArgument(1) and
    node2.asExpr() = fc
  )
}

predicate callbackToBindEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall bind | bind.getTarget().getName().matches("Bind%") and
    node2.asExpr() = bind and
    node1.asExpr() = bind.getAnArgument() and
    node1.asExpr().getType().stripType().(Class).getABaseClass*().hasName("CallbackBase")
  )  
}

//-------- dataflow edges end

/**
 * A useful predicate to construct call graphs which takes `Bind` callbacks into account.
 */
predicate reach(Function f, Function g) {
  (
    exists(FunctionCall gc | 
      if g instanceof Destructor then
        g = gc.getTarget()
      else
        overrides*(g, gc.getTarget())
      |
      g = gc.getTarget() and
      gc.getEnclosingFunction() = f
    ) or
    exists(CallbackSinks sink | sink.getEnclosingCallable() = f and
      sink.getCalledFunction() = g and
      (sink instanceof CallbackPostTaskSink or sink instanceof CallbackRunSink)
    )
  )
}

class UnretainedWrapper extends ClassTemplateInstantiation {
  UnretainedWrapper() {
    stripType().getName().matches("UnretainedWrapper<%")
  }
  
  Type getUnretainedType() {
    result = getTemplateArgument(0)
  }
}

class RetainedRefWrapper extends ClassTemplateInstantiation {
  RetainedRefWrapper() {
    stripType().getName().matches("RetainedRefWrapper<%")
  }
  
  Type getRetainedType() {
    result = getTemplateArgument(0)
  }
}

class RunCallback extends FunctionCall {
  Expr callback;
  
  RunCallback() {
    exists(FunctionCall fc | fc.getTarget() instanceof StdMove and
      this.getTarget().getName() = "Run" and
      this.getQualifier() = fc and
      fc.getAnArgument() = callback
    )
  }
  
  Expr getCallback() {result = callback}
}

/**
 * A general callback, either created from `BindOnce`, `BindRepeating` or
 * from a wrapper like `AdaptCallbackForRepeating`.
 */
abstract class GeneralCallback extends FunctionCall {

  /** Gets a retained type in the binding set of a callback.*/
  Type getARetainedType() {
    exists(ClassTemplateInstantiation t | 
      t.getName().matches("RetainedRefWrapper<%") |
      generalStripType(getAnArgument().getType()) = t and
      result = t.getTemplateArgument(0)
    ) or
    exists(ManagedPtr t | getAnArgument().getType().stripType() = t and
      result = t.getManagedType()
    ) or
    exists(PointerType t, Expr arg | arg = getAnArgument() and
      t = arg.getType() and
      t.stripType().(Class).getABaseClass*().getName().matches("RefCounted%") and
      result = t.stripType()
    )
  }

  /**
   * Gets an unretained type in the binding set of a callback.
   */
  Type getAnUnretainedType() {
    exists(ClassTemplateInstantiation t | generalStripType(getAnArgument().getType()) = t and
      t.getName().matches("UnretainedWrapper<%") and
      result = t.getTemplateArgument(0)
    ) or
    exists(PointerType t, Expr arg | arg = getAnArgument() and
      t = arg.getType() and
      not exists(Class c | c.getName().matches("RefCounted%") and c = t.stripType().(Class).getABaseClass*()) and
      result = t.stripType()
    )    
  }
  
  /**
   * Gets an argument of the callback that is another callback. Use for propagating the dataflow when
   * tracking unretained types etc.
   */
  Expr getACallbackArg() {
    result = getAnArgument() and
    generalStripType(result.getType()).(Class).getABaseClass+().hasName("CallbackBase")
  }
}

/**
 * Callbacks that are created using methods like `BindOnce`, `BindRepeating`.
 */
class BindCall extends GeneralCallback {
  BindCall() {
    this.getTarget().getName().matches("Bind%") and
    not this.getTarget().getName() = "Binding" and
    not this.getTarget().getName() = "BindState" and
    not this.getTarget().getName() = "BindImpl"
  }
  
  //TODO: handle case where first argument is a callback
  Function getFunction() {
    exists(FunctionAccess fa | fa = getArgument(0) and
      result = fa.getTarget()
    )
  }
}

/**
 * A callback that is converted from another callback by wrapping it in various functions.
 */
class CallbackWrapper extends GeneralCallback {
  CallbackWrapper() {
    getTarget().hasName("WrapCallbackWithDefaultInvokeIfNotRun") or
    getTarget().hasName("WrapCallbackWithDropHandler") or
    getTarget().hasName("AdaptCallbackForRepeating")
  }
  
  Expr getWrappedCallbackArg() {
    result = getArgument(0)
  }
}

class Callback extends Class {
  Callback() {
    stripType().(Class).getABaseClass+().getName() = "CallbackBase"
  }
}

/**
 * A `Field` that stores callbacks, either normal callback or a collection.
 */
class CallbackField extends Field {
  CallbackField() {
    generalStripType(getType()) instanceof Callback or
    (
      this instanceof GeneralPointerField and
      this.(GeneralPointerField).getPointerType() instanceof Callback
    )
    or
    (
      this instanceof GeneralManagedField and
      this.(GeneralManagedField).getManagedType() instanceof Callback
    )
    or
    (
      this instanceof CollectionField and
      (
        this.getType().(MapType).getComponentType() instanceof Callback or
        this.getType().(ListType).getComponentType() instanceof Callback
      )
    )
  }
}

class CallbackSinks extends DataFlow::Node {
  DataFlow::Node source;
  
  CallbackSinks() {
    exists(CallbackConfig cfg |
      cfg.hasFlow(source, this)
    )
  }
  
  DataFlow::Node getASource() {
    result = source
  }

  /**
   * Gets the actuall function that is called in this callback.
   */
  Function getCalledFunction() {
    exists(GeneralCallback bc | bc = source.asExpr() |
      result = bc.getArgument(0).getAChild*().(FunctionAccess).getTarget() or
      result = bc.getArgument(0).getAChild*().(LambdaExpression).getLambdaFunction() or
      exists(CallbackSinks sink | sink.asExpr() = bc.getArgument(0) and
        result = sink.getCalledFunction()
      )
    ) or
    exists(CallbackField f, CallbackSinks sink | f.getAnAccess() = source.asExpr() and
      sink.asExpr() = generalAssignValue(f) and
      result = sink.getCalledFunction()
    )
  }
  
  Class getARetainedType() {
    exists(GeneralCallback bc | bc = source.asExpr() |
      result = bc.getARetainedType() or
      //propagates through callback args
      exists(CallbackSinks sink | sink.asExpr() = bc.getACallbackArg() and
        result = sink.getARetainedType()
      )
    ) or
    //Propagates through fields
    exists(CallbackField f, CallbackSinks sink | f.getAnAccess() = source.asExpr() and
      sink.asExpr() = generalAssignValue(f) and
      result = sink.getARetainedType()
    )
  }
  
  Class getAnUnretainedType() {
    exists(GeneralCallback bc | bc = source.asExpr() |
      result = bc.getAnUnretainedType() or
      //propagates through callback args
      exists(CallbackSinks sink | sink.asExpr() = bc.getACallbackArg() and
        result = sink.getAnUnretainedType()
      )
    ) or 
    //Propagates through fields
    exists(CallbackField f, CallbackSinks sink | f.getAnAccess() = source.asExpr() and
      sink.asExpr() = generalAssignValue(f) and
      result = sink.getAnUnretainedType()
    )
    
  }
}

class CallbackFieldSink extends CallbackSinks {
  CallbackFieldSink() {
    isCallbackFieldSink(this)
  }
}

class CallbackPostTaskSink extends CallbackSinks {
  CallbackPostTaskSink() {
    postTaskSink(this)
  }
}

class CallbackRunSink extends CallbackSinks {
  CallbackRunSink() {
    runCallbackSink(this)
  }
}

class CallbackInterfacePtrSink extends CallbackSinks {
  CallbackInterfacePtrSink() {
    interfacePtrCallSink(this)
  }
}