import cpp
import callbacks

/**
 * Models various mechanism in Chrome that is used for managing object lifetime.
 */

 /**
  * A map field that has pointer as key and managed pointer of the same type as value. 
  * Usually the value are backing the keys, so it is usually ok.
  */
class ManagedKeyValueField extends MapField {
  ManagedKeyValueField() {
    exists(PointerType key, ManagedPtr value |
      this.getType().(MapType).getComponentTypeAt(0) = key and
      this.getType().(MapType).getComponentTypeAt(1) = value and
      key.stripType() = value.getManagedType()
    )
  }
}

/**
 * A wrapper of `destructorCleanup` for raw pointer fields that are removed when the type is destroyed.
 */
predicate cleanupInDestructor(GeneralPointerField f) {
  exists(Destructor d, Expr fa | destructorCleanup(f, d, fa))
}

/**
 * Whether the destructor of a type will remove the field f when executed.
 */
predicate destructorCleanup(GeneralPointerField f, Destructor d, Expr fa) {
  f.getPointerType() = d.getDeclaringType().getABaseClass*() and
  fa = f.getACleanup() and
  reach*(d, fa.getEnclosingFunction())
}

/**
 * FrameServiceBase is a class that observes the lifetime of RenderFrameHostImpl and
 * so raw pointer of rfh inside it is usually ok.
 */
class FrameServiceBase extends ClassTemplateInstantiation {
  FrameServiceBase() {
    getName().matches("FrameServiceBase<%")
  }
  
  Type getService() {
    result = getTemplateArgument(0)
  }
}

predicate frameServiceBaseProtected(Field f) {
  f.hasName("render_frame_host_") and
  exists(FrameServiceBase fsb | fsb.getService() = f.getDeclaringType())
}

/**
 * An expression that is not inside the constructor of a class. This and
 * the following few predicates are useful to see if a managed pointer reset is
 * inside constructor/destructor etc., which usually makes them ok.
 */
predicate notInsideConstructor(Class c, Expr e) {
  not exists(Constructor f | f = e.getEnclosingFunction() and
    f.getDeclaringType() = c
  )
}

predicate notInsideDestructor(Class c, Expr e) {
  not exists(Destructor f | f = e.getEnclosingFunction() and
    f.getDeclaringType() = c
  )
}
