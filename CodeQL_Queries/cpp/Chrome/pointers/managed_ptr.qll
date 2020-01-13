import cpp
import semmle.code.cpp.dataflow.DataFlow
import collections

/**
 * General library code to deal with managed pointer types.
 */

//------------ dataflow edges

/**
 * `x -> x.get();`
 * `x` managed pointer
 */
predicate getEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget() instanceof PointerGet and
    node1.asExpr() = fc.getQualifier() and node2.asExpr() = fc
  )
}

/**
 * `x -> std::move(x);`
 * `x` managed pointer.
 */
predicate stdMoveEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget() instanceof StdMove and
    fc.getAnArgument() = node1.asExpr() and
    fc = node2.asExpr()
  )
}

/**
 * `x -> x.release
 */
predicate stdReleaseEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget() instanceof StdRelease and
    fc.getQualifier() = node1.asExpr() and
    fc = node2.asExpr()
  )
}

/** 
 * `x -> std::forward(x);`
 */
predicate stdForwardEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget() instanceof StdForward and
    fc.getAnArgument() = node1.asExpr() and
    fc = node2.asExpr()
  )
}

predicate wrapUniqueEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().getName().matches("WrapUnique%") and
    fc = node2.asExpr() and fc.getArgument(0) = node1.asExpr()
  )
}

predicate makeUniqueEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().getName().matches("make_unique%") and
    fc = node2.asExpr() and fc.getArgument(0) = node1.asExpr()
  )
}

predicate makeRefEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().getName().matches("MakeRefCounted%") and
    fc = node2.asExpr() and fc.getArgument(0) = node1.asExpr()
  )
}

predicate wrapRefCountedEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().getName() = "WrapRefCounted" and
    fc = node2.asExpr() and fc.getArgument(0) = node1.asExpr()
  )
}

predicate pointerWrapperEdge(DataFlow::Node node1, DataFlow::Node node2) {
  makeRefEdge(node1, node2) or
  makeUniqueEdge(node1, node2) or
  wrapUniqueEdge(node1, node2) or
  wrapRefCountedEdge(node1, node2)
}

predicate pointerTransferEdge(DataFlow::Node node1, DataFlow::Node node2) {
  stdMoveEdge(node1, node2) or
  stdForwardEdge(node1, node2) or
  stdReleaseEdge(node1, node2)
}

//-------------dataflow ends

/** 
 * General managed pointer type. Only includes `scoped_refptr` and `unique_ptr`
 * at the moment. (`WeakPtr` not too interesting for memory corruption problem)
 * Includes normal type or container types.
 */
abstract class GeneralManagedType extends Class {
  /**
   * Gets the underlying managed type.
   */
  abstract Type getManagedType();
  
  /**
   * Holds if the type is owned completely, i.e. `unique_ptr`.
   */
  abstract predicate isOwner();
}

/**
 * General managed pointer field. Excludes `WeakPtr` as it is less interesting.
 * Includes normal field or collection field.
 */
abstract class GeneralManagedField extends Field {
  
  /**
   * Gets the underlying type that is being managed.
   */
  abstract Type getManagedType();
  
  /**
   * Gets an expression that resets the pointer, which can free the underlying object.
   */
  abstract Expr getAManagedReset();
  
  /**
   * Holds if the field is owned completely, i.e. `unique_ptr`.
   */
  abstract predicate isOwner();
}

/** Function that transfers ownership of a managed pointer.*/
abstract class PointerTransferFunction extends Function {
}

class StdMove extends PointerTransferFunction {
  StdMove() {
    this.getQualifiedName() = "std::__Cr::move"
  }
}

class StdForward extends PointerTransferFunction {
  StdForward() {
    this.getQualifiedName() = "std::__Cr::forward"
  }
}

class StdRelease extends PointerTransferFunction {
  StdRelease() {
    this.getName() = "release" and
    this.getDeclaringType() instanceof ManagedPtr
  }
}

/** The `get` function from a managed pointer that returns the underlying raw pointer.*/
class PointerGet extends Function {
  PointerGet() {
    this.hasName("get") and
    this.getDeclaringType().stripType().getName().matches("%_ptr%")
  }
}

/** Normal managed pointers.*/
class ManagedPtr extends GeneralManagedType {
  ManagedPtr() {
    this.getName().matches("unique_ptr%") or
    this.getName().matches("scoped_refptr%")
  }
  
  override Type getManagedType() {result = this.getTemplateArgument(0)}
  
  override predicate isOwner() {getName().matches("unique_ptr%")}
}

/** Normal managed pointer fields.*/
class ManagedPtrField extends GeneralManagedField {
  ManagedPtrField() {
    this.getType().stripType() instanceof ManagedPtr
  }
  
  override Type getManagedType() {result = getType().stripType().(ManagedPtr).getManagedType()}
  
  override Expr getAManagedReset() { 
    result.(FunctionCall).getTarget() instanceof ManagedPtrSetFunction and
    getQualifier(result) = this.getAnAccess()
  }
  
  override predicate isOwner() {
    this.getType().stripType().(ManagedPtr).isOwner()
  }
}

/** A function that resets a managed pointer.*/
class ManagedPtrSetFunction extends Function {
  ManagedPtrSetFunction() {
    hasName("operator=") or
    hasName("reset")
  }
}

/**
 * A managed pointer that is inside a collection.
 */
abstract class ManagedCollectionType extends GeneralManagedType {
  ManagedCollectionType() {
    this.stripType().getName().matches("%unique_ptr%") or
    this.stripType().getName().matches("%scoped_refptr%")
  }
  
  override predicate isOwner() {
    this.stripType().getName().matches("%unique_ptr%")
  }
}

/**
 * A managed pointer inside a map.
 */
class ManagedMapType extends ManagedCollectionType, MapType {
  
  ManagedMapType() {
    getComponentType() instanceof ManagedPtr    
  }
  
  override Type getManagedType() {
    exists(Type t |
      t = getComponentType() and
      result =  t.(ManagedPtr).getManagedType()
    )
  }
}

/** A managed pointer inside a list type. */
class ManagedListType extends ManagedCollectionType, ListType {
  
  ManagedListType() {
    getComponentType() instanceof ManagedPtr
  }
  
  override Type getManagedType() {
    exists(Type t |
      t = getComponentType() and
      result =  t.(ManagedPtr).getManagedType()
    )
  }
}

/**
 * A field that stores managed pointers in a map.
 */
class ManagedMapField extends MapField, GeneralManagedField {
  ManagedMapField() {
    // binding set is not likely to have problem.
    not (getName() = "bindings_" and getDeclaringType().getName().matches("BindingSet%")) and
    getType() instanceof ManagedMapType
  }
  
  override Type getManagedType() {
    result = getType().(ManagedMapType).getManagedType()
  }
  
  override Expr getAManagedReset() {result = getAReset()}
  
  override predicate isOwner() {getType().(ManagedMapType).isOwner()}
}

/**
 * A field that stores managed pointers in a list/vector type.
 */
class ManagedListField extends ListField, GeneralManagedField {
  ManagedListField() {
    // binding set is not likely to have problem.
    not (getName() = "bindings_" and getDeclaringType().getName().matches("BindingSet%")) and
    getType() instanceof ManagedListType
  }
  
  override Type getManagedType() {
    result = getType().(ManagedListType).getManagedType()
  }

  override Expr getAManagedReset() {result = getAReset()}
  
  override predicate isOwner() {getType().(ManagedListType).isOwner()}
}