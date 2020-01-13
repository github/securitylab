import cpp
import common

/**
 * General library code to deal with raw pointer types.
 */

/**
 * General type that contains raw pointers, e.g. raw pointer fields,
 * pointers in collections etc.
 */
abstract class GeneralPointerType extends Type {
  /**
   * Gets the underlying raw pointer type.
   */
  abstract Type getPointerType();
}

/**
 * General field that are `GeneralPointerType`.
 */
abstract class GeneralPointerField extends Field {
  
  /**
   * Gets the underlying raw pointer type
   */
  abstract Type getPointerType();
  
  /**
   * Gets a clean up that will reset the raw pointer.
   */
  abstract Expr getACleanup();
}

/**
 * A normal raw pointer field.
 */
class PointerField extends GeneralPointerField {
  PointerField() {
    getType().getUnspecifiedType() instanceof PointerType and
    //Needs to exclude some types that are less interesting (mostly generated code)
    not getDeclaringType().getName().matches("ArrayDataViewImpl%") and
    not getDeclaringType().getName().matches("AllocatedData") and
    not getDeclaringType().getName().matches("AlignedUnion") and
    not getDeclaringType().getName().matches("ArrayViewBase%") and
    not getDeclaringType().getName().matches("AssociatedInterface%") and
    not hasName("receiver_") and
    (
      //Only interested in certain area of the code. Can tweak
      getFile().getAbsolutePath().matches("%/browser/%") or
      getFile().getAbsolutePath().matches("%/media/%") or
      getFile().getAbsolutePath().matches("%/components/%")
    )
  }

  override Type getPointerType() {result = this.getType().stripType()}

  override Expr getACleanup() {
    exists(GeneralAssignment assign | assign.getLValue() = this.getAnAccess() and
      assign.getRValue() instanceof Zero and
      result = assign.getLValue()
    )
  }
}
/** 
 * A field that stores raw pointers in map.
 */
class PointerMapField extends MapField, GeneralPointerField {
  PointerMapField() {
    getType().(MapType).getComponentType() instanceof PointerType
  }
  
  override Type getPointerType() {
    exists(Type t |
      t = getType().(MapType).getComponentType() and
      t instanceof PointerType and
      result = t.stripType()
    )
  }
  
  override Expr getACleanup() {
    exists(FunctionCall fc | fc.getTarget().hasName("erase") or
      fc.getTarget().hasName("clear") or fc.getTarget().hasName("remove") |
      getQualifier(fc) = this.getAnAccess() and
      result = fc
    )
  }
  
}

/**
 * A field that stores raw pointers in list/vector.
 */
class PointerListField extends ListField, GeneralPointerField {
  PointerListField() {
    getType().(MapType).getComponentType() instanceof PointerType
  }
  
  override Type getPointerType() {
    exists(Type t |
      t = getType().(ListType).getComponentType() and
      t instanceof PointerType and
      result = t.stripType()
    )
  }

  override Expr getACleanup() {
    exists(FunctionCall fc | fc.getTarget().hasName("erase") or
      fc.getTarget().hasName("clear") or fc.getTarget().hasName("remove") |
      getQualifier(fc) = this.getAnAccess() and
      result = fc
    )
  }
  
}
