/**
 * @name 02_MinIntNegate
 * @description Negating MIN_INT is an integer overflow
 * @kind problem
 * @id cpp/min-int-negate
 * @problem.severity warning
 */

import cpp
import semmle.code.cpp.controlflow.Guards
import semmle.code.cpp.valuenumbering.GlobalValueNumbering

// The previous query, 01_MinIntNegate, eliminated a bad result
// from 00_MinIntNegate, but it also lost a good result.
// The missing result is test.cpp, line 14:
//
//   if (s->myfield < 0) {
//     s->myfield = -s->myfield;
//   }
//
// The problem is that `s->myfield` is not a `LocalScopeVariable`.
// The solution is to use the GlobalValueNumbering library, which
// is a more general way to find expressions that compute the same
// value.
from GuardCondition guard, BasicBlock block, UnaryMinusExpr unaryMinus, Expr use1, Expr use2
where
  guard.(LTExpr).getLeftOperand() = use1 and
  guard.(LTExpr).getRightOperand().getValue().toInt() = 0 and
  guard.controls(block, true) and
  block.contains(unaryMinus) and
  unaryMinus.getOperand() = use2 and
  globalValueNumber(use1) = globalValueNumber(use2)
select unaryMinus, "If the value of $@ is MinInt then this assignment will not make it positive",
  use2, use2.toString()
