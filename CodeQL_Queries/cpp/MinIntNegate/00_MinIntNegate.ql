/**
 * @name 00_MinIntNegate
 * @description Negating MIN_INT is an integer overflow
 * @kind problem
 * @id cpp/min-int-negate
 * @problem.severity warning
 */

import cpp
import semmle.code.cpp.controlflow.Guards

// Find this pattern:
//
// ```
// if (x < 0) {
//   x = -x;
// }
// ```
//
// If the value of `x` is `0x80000000` then this will not make the value of `x` positive.
from GuardCondition guard, BasicBlock block, UnaryMinusExpr unaryMinus, Variable v, Expr use
where
  guard.(LTExpr).getLeftOperand() = v.getAnAccess() and
  guard.(LTExpr).getRightOperand().getValue().toInt() = 0 and
  guard.controls(block, true) and
  block.contains(unaryMinus) and
  unaryMinus.getOperand() = v.getAnAccess()
select unaryMinus, "If the value of $@ is MinInt then this assignment will not make it positive", v,
  v.getName()
