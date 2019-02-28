/**
 * @name Filter with upper bound and kzalloc
 * @description This query excludes results that are safe because the upper
 *              bound of the size argument is less than or equal to the size of
 *              the destination variable or array. It also excludes results
 *              that are safe because the right amount of memory was allocated
 *              with kzalloc.
 */

import cpp
import semmle.code.cpp.rangeanalysis.SimpleRangeAnalysis
import semmle.code.cpp.valuenumbering.GlobalValueNumbering
import semmle.code.cpp.dataflow.DataFlow

// We wrote this predicate in the previous query.
predicate safe_malloc(FunctionCall allocCall, FunctionCall copy_from_user) {
  exists(DataFlow::Node source, DataFlow::Node sink |
    allocCall.getTarget().getName() = "kzalloc" and
    copy_from_user.getTarget().getName() = "copy_from_user" and
    source.asExpr() = allocCall and
    sink.asExpr() = copy_from_user.getArgument(0) and
    DataFlow::localFlow(source, sink) and
    globalValueNumber(allocCall.getArgument(0)) = globalValueNumber(copy_from_user.getArgument(2))
  )
}

// Add a filter to remove results that match the `safe_malloc` pattern.
from FunctionCall call, Expr destArg, Expr sizeArg
where
  call.getTarget().getName() = "copy_from_user" and
  destArg = call.getArgument(0) and
  sizeArg = call.getArgument(2) and
  not destArg.getType().(PointerType).getBaseType().getSize() >= upperBound(sizeArg) and
  not destArg.getType().(ArrayType).getSize() >= upperBound(sizeArg) and
  not safe_malloc(_, call)
select call, destArg.getType(), lowerBound(sizeArg), upperBound(sizeArg),
  call.getFile().getRelativePath()
