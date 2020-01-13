import cpp
import common
import semmle.code.cpp.dataflow.DataFlow

//-------- Dataflow edges ------------

/**
 * Edges that goes from some simple standard containers into their elements.
 */
predicate pairEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | 
    fc.getTarget().getName().matches("tuple%") or
    fc.getTarget().getName().matches("pair<%") or
    fc.getTarget().getName().matches("value_type%")
    |
    node1.asExpr() = fc.getAnArgument() and node2.asExpr() = fc
  )
}

/** Goes from an element into a collection via an overloaded index operator.*/
predicate indexSetEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().hasName("operator[]") |
    node2.asExpr() = getQualifier(fc) and node1.asExpr() = fc
  )
}

/** Goes from a collection to its element via an overloaded index operator. */
predicate indexGetEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget().hasName("operator[]") |
    node1.asExpr() = getQualifier(fc) and node2.asExpr() = fc
  )
}

/**
 * Edge that goes from an a method that adds to a collection into a collection.
 */
predicate addToCollectionEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget() instanceof AddToCollection and
    getQualifier(fc) = node2.asExpr() and node1.asExpr() = fc
  )
}

/**
 * Edge that goes from a collection to a the result of a method that takes elements from it.
 */
predicate takeFromCollectionEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc | fc.getTarget() instanceof TakeFromCollection and
    getQualifier(fc) = node1.asExpr() and node2.asExpr() = fc
  )
}

/** 
 * To go from an iterator to iterator->second.
 */
predicate mapIteratorEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FieldAccess fa | fa.getTarget().hasName("second") and
    node1.asExpr() = getQualifier(fa) and
    node2.asExpr() = fa
  )
}

/**
 * Combine edge that goes from a collection to an element.
 */
predicate collectionsGetEdge(DataFlow::Node node1, DataFlow::Node node2) {
  indexGetEdge(node1, node2) or
  takeFromCollectionEdge(node1, node2) or
  mapIteratorEdge(node1, node2) or
  pairEdge(node1, node2)
}

/**
 * Combine edge that goes from an element into a collection.
 */
predicate collectionsSetEdge(DataFlow::Node node1, DataFlow::Node node2) {
  indexSetEdge(node1, node2) or
  addToCollectionEdge(node1, node2) or
  pairEdge(node1, node2)
}

/**
 * Combine edge that goes either from element to collection or vice versa.
 */
predicate collectionsEdge(DataFlow::Node node1, DataFlow::Node node2) {
  indexGetEdge(node1, node2) or
  indexSetEdge(node1, node2) or
  addToCollectionEdge(node1, node2) or
  takeFromCollectionEdge(node1, node2) or
  mapIteratorEdge(node1, node2) or
  pairEdge(node1, node2)
}

/**
 * Various methods that adds to a collection.
 */
class AddToCollection extends Function {
  AddToCollection() {
    hasName("push_back") or
    hasName("insert") or
    hasName("emplace_back") or
    hasName("emplace") or
    hasName("push") or
    hasName("push_front")
  }
}

/**
 * Various methods that takes from a collection.
 */
class TakeFromCollection extends Function {
  TakeFromCollection() {
    hasName("find") or
    getName().matches("pop_%") or
    hasName("pop") or
    hasName("find_if") or
    hasName("front")
  }
}

/**
 * Loose heuristics for collection types that are like vector, list etc.
 */
class ListType extends Class {
  ListType() {
    exists(Function f, Type t |
      f.hasName("push_back") or f.hasName("front") or f.hasName("push") or f.hasName("pop")
      | t = f.getDeclaringType() and
        this = t.stripType().(ClassTemplateInstantiation)
    )
  }
  
  /**
   * Gets the type of the component in the container.
   */
  Type getComponentType() {
    result = getComponentType*(this.getTemplateArgument(0)) and
    not collectionTemplateType(result)
  }
}

/**
 * General type that represents a set.
 */
class SetType extends Class {
  SetType() {
    this.getName().matches("set<%") or
    this.getName().matches("flat_set<%")
  }
  
  Type getComponentType() {
    result = getComponentType*(this.getTemplateArgument(0)) and
    not collectionTemplateType(result)
  }
}

/**
 * Type that represents a tuple.
 */
class TupleType extends ClassTemplateInstantiation {
  TupleType() {
    getName().matches("pair%") or
    getName().matches("tuple%")
  }
}

class ValueType extends ClassTemplateInstantiation {
  ValueType() {
    this.getName().matches("__value_type%")
  }
}

class HashValueType extends ClassTemplateInstantiation {
  HashValueType() {
    this.getName().matches("__has_value_type%")
  }
}


/** 
 * General method to get the component type of different containers.
 */
Type getComponentType(Type t) {
  if collectionTemplateType(t) then 
    result = t.(ListType).getComponentType() or
    result = t.(TupleType).getATemplateArgument() or
    result = t.(ValueType).getATemplateArgument() or
    result = t.(HashValueType).getATemplateArgument() or
    result = t.(MapType).getComponentType()
  else
    result = t
}

/** Holds if `t` is a collection type.*/
predicate collectionTemplateType(Type t) {
  t instanceof ListType or
  t instanceof TupleType or
  t instanceof ValueType or
  t instanceof HashValueType or
  t instanceof MapType
}

/**
 * Heuristics for the map type.
 */
class MapType extends Class {
  MapType() {
    (
      exists(Function f, Type t |
        f.hasName("find") or f.hasName("insert")|
        t = f.getDeclaringType() and
        this = t.stripType().(ClassTemplateInstantiation)
      )
    )
    and
    not this.getName().matches("__tuple_leaf%") and
    not this.getName().matches("pair%") and
    not this.getName().matches("union%")
  }
  
  /** Component 0 is the key type, component 1 is the value type.*/
  Type getComponentTypeAt(int i) {
    (
      result = getComponentType*(this.getTemplateArgument(i)) and
      not collectionTemplateType(result)
    )
  }
  
  Type getComponentType() {
    (
      result = getComponentType*(this.getTemplateArgument(0)) and
      not collectionTemplateType(result)
    ) or
    (
      result = getComponentType*(this.getTemplateArgument(1)) and
      not collectionTemplateType(result)
    )
  }
}

/** A general field that is of `Collection` type.*/
abstract class CollectionField extends Field {
  
  /** An expression that resets an element of the collection field. */
  abstract Expr getAReset();
}

class MapField extends CollectionField {
  MapField() {
    getType() instanceof MapType
  }
  
  override Expr getAReset() {
    exists(FunctionCall fc | fc.getTarget().hasName("erase") or
      fc.getTarget().getName().matches("pop%") or fc.getTarget().hasName("clear") or
      fc.getTarget().hasName("insert") |
      fc.getQualifier() = this.getAnAccess() and
      result = fc
    )
    or
    exists(FunctionCall fc | fc.getTarget().hasName("operator[]") and
      fc.getQualifier() = this.getAnAccess() and
      not exists(FunctionCall assign | assign.getTarget().hasName("operator=") and
        fc = assign.getArgument(0)
      ) and
      not exists(AssignExpr assign | assign.getRValue() = fc) and
      result = fc
    )
  }
}

class ListField extends CollectionField {
  ListField() {
    getType() instanceof ListType
  }
  
  override Expr getAReset() {
    exists(FunctionCall fc | fc.getTarget().hasName("erase") or
      fc.getTarget().getName().matches("pop%") or fc.getTarget().hasName("clear") |
      fc.getQualifier() = this.getAnAccess() and
      result = fc
    )
    or
    exists(FunctionCall fc | fc.getTarget().hasName("operator[]") and
      fc.getQualifier() = this.getAnAccess() and
      not exists(FunctionCall assign | assign.getTarget().hasName("operator=") and
        fc = assign.getArgument(0)
      ) and
      not exists(AssignExpr assign | assign.getRValue() = fc) and
      result = fc
    )
  }
}