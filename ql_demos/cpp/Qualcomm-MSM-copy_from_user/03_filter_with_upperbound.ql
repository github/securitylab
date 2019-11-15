/**
 * @name Filter with upper bound, also for arrays
 * @description This query excludes results that are safe because the upper
 *              bound of the size argument is less than or equal to the size of
 *              the destination variable or array.
 */

import cpp
import semmle.code.cpp.rangeanalysis.SimpleRangeAnalysis

// It turns out that the filter in the previous query does
// not work for array types, so let's add a second filter which
// excludes examples like this:
//
// ```
// struct MyStruct s[2];
// copy_from_user(s, usrptr, sizeof(s));
// ```
from FunctionCall call, Expr destArg, Expr sizeArg
where
  call.getTarget().getName() = "copy_from_user" and
  destArg = call.getArgument(0) and
  sizeArg = call.getArgument(2) and
  not destArg.getType().(PointerType).getBaseType().getSize() >= upperBound(sizeArg) and
  not destArg.getType().(ArrayType).getSize() >= upperBound(sizeArg)
select call, destArg.getType(), lowerBound(sizeArg), upperBound(sizeArg),
  call.getFile().getRelativePath()
