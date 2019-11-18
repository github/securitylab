/**
 * @name 01_MinIntNegate
 * @description Negating MIN_INT is an integer overflow
 * @kind problem
 * @id cpp/min-int-negate
 * @problem.severity warning
 */

import cpp
import semmle.code.cpp.controlflow.Guards

// The previous query had an incorrect result at test.cpp, line 20:
//
//   if (s->myfield < 0) {
//     s->myfield = -t->myfield;
//   }
//
// The problem is that the query used `Variable`, which includes fields.
// So here we restrict the query to use `LocalScopeVariable` instead.
from
  GuardCondition guard, BasicBlock block, UnaryMinusExpr unaryMinus, LocalScopeVariable v, Expr use
where
  guard.(LTExpr).getLeftOperand() = v.getAnAccess() and
  guard.(LTExpr).getRightOperand().getValue().toInt() = 0 and
  guard.controls(block, true) and
  block.contains(unaryMinus) and
  unaryMinus.getOperand() = v.getAnAccess()
select unaryMinus, "If the value of $@ is MinInt then this assignment will not make it positive", v,
  v.getName()
