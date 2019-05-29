import cpp

/** Matches `var < var + ???`. */
predicate overflowCheck(LocalScopeVariable var, AddExpr add, RelationalOperation compare) {
  compare.getAnOperand() = var.getAnAccess() and
  compare.getAnOperand() = add and
  add.getAnOperand() = var.getAnAccess()
}

from LocalScopeVariable var, AddExpr add
where overflowCheck(var, add, _)
  and var.getType().getSize() < 4
select add, "Overflow check on variable of type " + var.getUnderlyingType()
