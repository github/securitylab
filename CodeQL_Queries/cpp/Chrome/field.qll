import cpp
import common
import collections
import pointers.raw_ptr

/**
 * An expression that assigns values to a general field. Can be an assignment,
 * or an index expression that modifies a collection etc.
 */
Expr generalAssignValue(Field f) {
  result = f.getAnAssignedValue() or
  //normal assignment
  exists(GeneralAssignment expr | expr.getLValue() = f.getAnAccess() and
    expr.getRValue() = result
  )
  or
  //Adding to a collection field
  exists(FunctionCall fc | fc.getTarget() instanceof AddToCollection and result = fc.getAnArgument() and
    getQualifier(fc) = f.getAnAccess()
  )
  or
  //setting managed pointers.
  exists(FunctionCall fc | fc.getTarget() instanceof ManagedPtrSetFunction and
    getQualifier(fc) = f.getAnAccess() and
    result = fc.getAnArgument()
  )
  or
  //index operator to assign values to a collection field.
  exists(FunctionCall indexer, GeneralAssignment expr | expr.getLValue() = indexer and
    indexer.getTarget().hasName("operator[]") and
    expr.getRValue() = result and
    getQualifier(indexer) = f.getAnAccess()
  )
}
