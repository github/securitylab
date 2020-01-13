import cpp
import collections
import pointers.managed_ptr
import semmle.code.cpp.dataflow.DataFlow

/** 
 * Common utilities that is adapted to the specific coding pattern in chrome.
 */


//--------- dataflow edges
 
/**
 * A dataflow edge that allows propagation through operator= as well as normal assignment.
 * See `GeneralAssignment` for details.
 */
predicate generalAssignEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(GeneralAssignment expr | node1.asExpr() = expr.getRValue() and
    node2.asExpr() = expr.getLValue()
  )
}

/**
 * A dataflow edge that allows propagation into loop variable of the form
 * ```
 * for (int x : xs) {
 *   ...
 * }
 * ```
 * i.e. goes from `xs` to `x`.
 */
predicate forRangeEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(RangeBasedForStmt stmt | node1.asExpr() = stmt.getRange() and
    node2.asExpr() = stmt.getRangeVariable().getAnAccess()
  )
}

/**
 * Allows flows of the form:
 * ```
 * *x = a; //<-- a tainted
 * b = x; //<-- b now tainted.
 * ```
 */
predicate useUsePairEdge(DataFlow::Node node1, DataFlow::Node node2) {
  useUsePair(_, node1.asExpr(), node2.asExpr())
}

predicate copyConstructorEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall cp | cp.getTarget() instanceof CopyConstructor and
    cp.getArgument(0) = node1.asExpr() and
    cp = node2.asExpr()
  )
}

predicate newEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(NewExpr expr | expr = node2.asExpr() and
    node1.asExpr() = expr.getInitializer()
  )
}

/**
 * Allows dataflow to go through polymorphic function call. Exclude some functions that
 * are frequently overloaded to cut down on noise, probably needs tweaking.
 */
predicate polymorphicCallEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc, MemberFunction g, int i | fc.getArgument(i) = node1.asExpr() and
    overrides*(g, fc.getTarget()) and node2.asParameter() = g.getParameter(i)|
    not fc.getTarget() instanceof AddToCollection and
    not fc.getTarget() instanceof TakeFromCollection and
    not fc.getTarget() instanceof PointerTransferFunction and
    not g.getDeclaringType() instanceof ManagedPtr and
    not count(g) = 1
  )
}

/**
 * Edge that takes into account of general constructions like make_unique, 
 */
predicate constructorEdge(DataFlow::Node node1, DataFlow::Node node2) {
  exists(FunctionCall fc, Constructor cstor, Class c, Parameter p, int idx | p.getAnAccess() = node2.asExpr() and
    constructionCall(c, fc) and p = cstor.getParameter(idx) and
    cstor.getDeclaringType() = c and
    cstor.getNumberOfParameters() = fc.getNumberOfArguments() and
    node1.asExpr() = fc.getArgument(idx)
  )
}

//---------- End dataflow edges
 
/**
 * This should be used instead of the `.getQualifier` method in `FunctionCall` and `FieldAccess` because 
 * the operator -> is often overriden in Chrome. This general method skip the overriden operator -> to find the
 * true qualifier in those circumstances.
 */
Expr getQualifier(Expr fc) {
  exists(Expr qualifier | qualifier = fc.(FunctionCall).getQualifier() or qualifier = fc.(FieldAccess).getQualifier() |
    if qualifier.(FunctionCall).getTarget().hasName("operator->") then
      result = getQualifier(qualifier)
    else
      result = qualifier
  )
}

/**
 * Again, this should be used instead of `Assignment` because the operator= is often overloaded. This general
 * expression takes operator= into account.
 */
class GeneralAssignment extends Expr {
  GeneralAssignment() {
    this instanceof Assignment or
    this.(FunctionCall).getTarget().hasName("operator=")
  }
  
  Expr getLValue() {
    result = this.(Assignment).getLValue() or
    result = this.(FunctionCall).getQualifier()
  }
  
  Expr getRValue() {
    result = this.(Assignment).getRValue() or
    result = this.(FunctionCall).getArgument(0)
  }
}

/**
 * General construction calls that takes `make_unique` and `MakeRefCounted` into account.
 */
predicate constructionCall(Class c, FunctionCall fc) {
  (fc instanceof ConstructorCall and fc.getTarget().getDeclaringType() = c) or
  (
   (fc.getTarget().getName().matches("make_unique%") or fc.getTarget().getName().matches("MakeRefCounted%"))
   and
   fc.getTemplateArgument(0) = c
  )
}

/**
 * General construction call that takes `make_unique` and `MakeRefCounted` into account, but also
 * constructor of base class from derived classes.
 */
predicate polyConstructionCall(Class c, FunctionCall fc) {
  (fc instanceof ConstructorCall and fc.getTarget().getDeclaringType().getABaseClass*() = c and
   not fc instanceof ConstructorBaseInit and not fc.isCompilerGenerated()) or
  (
   (fc.getTarget().getName().matches("make_unique%") or fc.getTarget().getName().matches("MakeRefCounted%"))
   and
   fc.getTemplateArgument(0).(Class).getABaseClass*() = c
  )
}


/**
 * Convenient function to extend `stripType` to include managed pointers also.
 */
Type generalStripType(Type t) {
  exists(Type st | st = t.stripType() |
    if st instanceof ManagedPtr then
      result = st.(ManagedPtr).getManagedType().stripType()
    else
      result = st
  )
}

/** For Optional, UnretainedWrapper, RetainedWrapper to get the underlying type*/
//TODO: Decide whether to include StructPtr or not
Type unwrapType(ClassTemplateInstantiation t) {
  if 
  (
    t.getName().matches("Optional<%") or
    t.getName().matches("UnretainedWrapper<%") or
    t.getName().matches("RetainedRefWrapper<%")
  ) then
    result = t.getTemplateArgument(0)
  else
    result = t
}