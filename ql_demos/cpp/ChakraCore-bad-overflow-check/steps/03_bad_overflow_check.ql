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
  and not add.getConversion+().getType().getSize() < 4
select add, "Bad overflow check on variable of type " + var.getUnderlyingType()
