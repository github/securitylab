import cpp

// Find expressions of the form `base->colormap[i][j]`

from ArrayExpr outer, ArrayExpr inner
where inner = outer.getArrayBase() and
      inner.getArrayBase().(FieldAccess).getTarget().getName() = "colormap"
select outer, "Indexing into colormap."
