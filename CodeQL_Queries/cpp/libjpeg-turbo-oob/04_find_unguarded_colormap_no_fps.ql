/**
 * @problem.severity error
 * @kind problem
 * @id libjpeg/unguarded-colormap-index
 */

import cpp
import semmle.code.cpp.dataflow.DataFlow
import semmle.code.cpp.valuenumbering.GlobalValueNumbering
import semmle.code.cpp.controlflow.Guards
import Options

// Find expressions of the form `base->colormap[i][j]` where
// `j` is checked against `base->cmap_length`

/** Recognize `error_exit` as a non-returning function. */
class JpegTurboCustomOptions extends CustomOptions {
  override predicate exprExits(Expr e) {
    super.exprExits(e) or
    exists (ExprCall c | e = c |
      c.getExpr().(PointerDereferenceExpr).getOperand().(FieldAccess).getTarget().hasName("error_exit")
    )
  }
}

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

/**
 * Holds if `assgn` allocates a new colour map of the given `size` and
 * stores it into `base->colormap`.
 */
predicate colormapInit(AssignExpr assgn, Expr base, Expr size) {
  exists (FieldAccess cmap, ExprCall alloc, PointerDereferenceExpr pd |
    assgn.getLValue() = cmap and
    base = cmap.getQualifier() and
    cmap.getTarget().hasQualifiedName("_bmp_source_struct::colormap") and
    assgn.getRValue() = alloc and
    pd = alloc.getExpr() and
    pd.getOperand().(FieldAccess).getTarget().getName() = "alloc_sarray" and
    size = alloc.getArgument(2)
  )
}

/**
 * A flow configuration for tracking structs that contain colour maps.
 */
class ColorMapTracker extends DataFlow::Configuration {
  ColorMapTracker() { this = "ColorMapTracker" }

  predicate isSource(AssignExpr assgn, DataFlow::Node nd) {
    exists (Expr s | colormapInit(assgn, s, _) | useUsePair(_, s, nd.asExpr()))
  }

  override predicate isSource(DataFlow::Node nd) {
    isSource(_, nd)
  }

  override predicate isSink(DataFlow::Node nd) {
    any()
  }
}

/**
 * A flow configuration for tracking expressions that denote the size of
 * colour maps.
 */
class ColorMapSizeTracker extends DataFlow::Configuration {
  ColorMapSizeTracker() { this = "ColorMapSizeTracker" }

  predicate isSource(AssignExpr assgn, DataFlow::Node nd) {
    exists (Expr s | colormapInit(assgn, _, s) | useUsePair(_, s, nd.asExpr()))
  }

  override predicate isSource(DataFlow::Node nd) {
    isSource(_, nd)
  }

  override predicate isSink(DataFlow::Node nd) {
    any()
  }
}

/**
 * Holds if `ae` indexes into the colour map allocated at `origin`.
 */
predicate isColorMapIndex(ArrayExpr ae, AssignExpr origin) {
  exists (ColorMapTracker cfg, FieldAccess fa, DataFlow::Node source |
    fa = ae.getArrayBase().(ArrayExpr).getArrayBase() and
    fa.getTarget().getName() = "colormap" and
    cfg.isSource(origin, source) and
    cfg.hasFlow(source, DataFlow::exprNode(fa.getQualifier()))
  )
}

/**
 * Holds if `sz` refers to the size of the colour map allocated at `origin`.
 */
predicate isColorMapSize(Expr sz, AssignExpr origin) {
  exists (ColorMapSizeTracker cfg, DataFlow::Node source |
    cfg.isSource(origin, source) and
    cfg.hasFlow(source, DataFlow::exprNode(sz))
  )
}

from ArrayExpr outer, ArrayExpr inner, GVN base, GVN index
where inner = outer.getArrayBase() and
      inner.getArrayBase() = bmpSourceStructField(base, "colormap") and
      outer.getArrayOffset() = index.getAnExpr() and
      not exists (GuardCondition gc, Expr bound |
        bound = bmpSourceStructField(base, "cmap_length")
        or
        exists (AssignExpr origin |
          isColorMapIndex(outer, origin) and
          isColorMapSize(bound, origin)
        )
        |
        gc.ensuresLt(index.getAnExpr(), bound, 0, inner.getBasicBlock(), true)
      )
select outer, "Unguarded indexing into colormap."
