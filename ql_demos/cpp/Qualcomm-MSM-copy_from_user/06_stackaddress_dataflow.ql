/**
 * @name Data flow from stack variable address
 * @description This restricts results to those that are most likely to be
 *              dangerous: copying directly into a stack variable.
 * @kind path-problem
 * @problem.severity warning
 * @id demo/msm/06-stackaddress-dataflow
 */

import cpp
import semmle.code.cpp.rangeanalysis.SimpleRangeAnalysis
import semmle.code.cpp.dataflow.DataFlow
import DataFlow::PathGraph

class Config extends DataFlow::Configuration {
  Config() { this = "copy_from_user" }

  override predicate isSource(DataFlow::Node source) {
    exists(LocalVariable v | source.asExpr().(AddressOfExpr).getOperand() = v.getAnAccess())
  }

  override predicate isSink(DataFlow::Node sink) {
    // This is the logic that was previously in the select clause of the query.
    exists(FunctionCall call, Expr destArg, Expr sizeArg |
      call.getTarget().getName() = "copy_from_user" and
      destArg = sink.asExpr() and
      destArg = call.getArgument(0) and
      sizeArg = call.getArgument(2) and
      not destArg.getType().(PointerType).getBaseType().getSize() >= upperBound(sizeArg) and
      not destArg.getType().(ArrayType).getSize() >= upperBound(sizeArg)
    )
  }
}

// This query looks specifically for cases where the address of a local
// variable is used as the target address of the `copy_from_user`. It also
// uses the DataFlow library, so that you can use the path viewer to see
// where the stack address comes from.
//
// The vulnerabilities are the final two results in `msm_cpp.c`.
from Config cfg, DataFlow::PathNode source, DataFlow::PathNode sink
where cfg.hasFlowPath(source, sink)
select sink, source, sink, "possibly unsafe copy_from_user"
