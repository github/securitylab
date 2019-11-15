import cpp
import semmle.code.cpp.valuenumbering.GlobalValueNumbering
import semmle.code.cpp.controlflow.Guards
import Options

// Find expressions of the form `base->colormap[i][j]` where
// `j` is checked against `base->cmap_length`

/**
 * Gets an expression of the form `base->fieldName`, where `base`
 * is of type `_bmp_source_struct`.
 */
Expr bmpSourceStructField(GVN base, string fieldName) {
  exists (FieldAccess fa |
    fa.getTarget().getName() = fieldName and
    fa.getTarget().getDeclaringType().hasName("_bmp_source_struct") and
    base = globalValueNumber(fa.getQualifier()) and
    globalValueNumber(result) = globalValueNumber(fa)
  )
}

from ArrayExpr outer, ArrayExpr inner, GVN base, GVN index
where inner = outer.getArrayBase() and
      inner.getArrayBase() = bmpSourceStructField(base, "colormap") and
      outer.getArrayOffset() = index.getAnExpr() and
      exists (GuardCondition gc, Expr bound |
        bound = bmpSourceStructField(base, "cmap_length") and
        gc.ensuresLt(index.getAnExpr(), bound, 0, inner.getBasicBlock(), true)
      )
select outer, "Guarded indexing into colormap."
