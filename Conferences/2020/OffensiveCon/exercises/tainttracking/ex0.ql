import cpp
import semmle.code.cpp.dataflow.TaintTracking

from MacroInvocation macro, Expr e1, Expr e2
where
  macro.getMacroName() = "_IOC_SIZE" and
  e1 = macro.getExpr() and
  TaintTracking::localExprTaint(e1, e2)
select e1, e2
