/**
 * @name Filter with upper bound
 * @description This query excludes results that are safe because the upper
 *              bound of the size argument is less than or equal to the size of
 *              the destination variable.
 */

import cpp
import semmle.code.cpp.rangeanalysis.SimpleRangeAnalysis

// Let's exclude filter out results that look like this:
//
// ```
// struct MyStruct s;
// copy_from_user(&s, usrptr, sizeof(s));
// ```
from FunctionCall call, Expr destArg, Expr sizeArg
where
  call.getTarget().getName() = "copy_from_user" and
  destArg = call.getArgument(0) and
  sizeArg = call.getArgument(2) and
  not destArg.getType().(PointerType).getBaseType().getSize() >= upperBound(sizeArg)
select call, destArg.getType(), lowerBound(sizeArg), upperBound(sizeArg),
  call.getFile().getRelativePath()
