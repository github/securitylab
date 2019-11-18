/**
 * @name 07_MinIntNegate
 * @description Negating MIN_INT is an integer overflow
 * @kind problem
 * @id cpp/min-int-negate
 * @problem.severity warning
 */

import cpp
import semmle.code.cpp.controlflow.Guards
import semmle.code.cpp.valuenumbering.GlobalValueNumbering
import semmle.code.cpp.dataflow.DataFlow

// The guards library automatically handles negations like this for us:
//
// ```
// if (!(x < 0)) {
//   // Do nothing.
// } else {
//   x = -x;
// }
// ```
//
// But it does not handle cases where dataflow is involved. For example:
//
// ```
// bool b = !(0 <= x);
// if (b) {
//   x = -x;
// }
// ```
//
// It's easy to fix by adding one more recursive clause to `lessThanWithNegate`.

/**
 * Holds if `cond` is a comparison of the form `lhs < rhs`.
 * `isStrict` is true for < and >, and false for <= and >=.
 */
predicate lessThan(Expr cond, Expr lhs, Expr rhs, boolean isStrict) {
  cond.(LTExpr).getLeftOperand() = lhs and
  cond.(LTExpr).getRightOperand() = rhs and
  isStrict = true
  or
  cond.(GTExpr).getLeftOperand() = rhs and
  cond.(GTExpr).getRightOperand() = lhs and
  isStrict = true
  or
  cond.(LEExpr).getLeftOperand() = lhs and
  cond.(LEExpr).getRightOperand() = rhs and
  isStrict = false
  or
  cond.(GEExpr).getLeftOperand() = rhs and
  cond.(GEExpr).getRightOperand() = lhs and
  isStrict = false
}

/**
 * Holds if `cond` is a comparison of the form `lhs < rhs`.
 * `isStrict` is true for < and >, and false for <= and >=.
 * `branch` is true if the comparison is true and false if it is not.
 */
predicate lessThanWithNegate(Expr cond, Expr lhs, Expr rhs, boolean isStrict, boolean branch) {
  branch = true and lessThan(cond, lhs, rhs, isStrict)
  or
  // (x < y) == !(y <= x)
  lessThanWithNegate(cond, rhs, lhs, isStrict.booleanNot(), branch.booleanNot())
  or
  // (!(x < y) == branch)  ==  ((x < y) == !branch)
  lessThanWithNegate(cond.(NotExpr).getOperand(), lhs, rhs, isStrict, branch.booleanNot())
  or
  // bool b = x < 0;
  // if (b) { ... }
  exists(Expr prev |
    DataFlow::localExprFlow(prev, cond) and
    lessThanWithNegate(prev, lhs, rhs, branch, isStrict)
  )
}

from
  GuardCondition guard, BasicBlock block, UnaryMinusExpr unaryMinus, Expr use1, Expr use2,
  Expr zero, boolean branch
where
  lessThanWithNegate(guard, use1, zero, _, branch) and
  zero.getValue().toInt() = 0 and
  guard.controls(block, branch) and
  block.contains(unaryMinus) and
  unaryMinus.getOperand() = use2 and
  globalValueNumber(use1) = globalValueNumber(use2)
select unaryMinus, "If the value of $@ is MinInt then this assignment will not make it positive",
  use2, use2.toString()
