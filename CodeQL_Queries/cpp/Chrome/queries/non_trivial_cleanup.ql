/**
 * @name non trivial cleanup
 * @description raw pointer fields that does has some clean up, but not trivial (i.e. executes in destructor) or managed objects that backs them.
 * @kind problem
 * @problem.severity warning
 */

import cpp
import common
import pointers.raw_ptr
import pointers.managed_ptr
import object_lifetime.lifetime_management
import object_lifetime.obj_lifetime

from GeneralPointerField f
where 
//The type of the pointer field contains a managed field that is the declaring type of the pointer field, e.g.
//``` class A {
//      F* f;
//    }
//    class F {
//      unique_ptr<A> a;
//    }
//```
//    often, `F` is the owner of `A`.
not exists(GeneralManagedField mf | f.getPointerType() = mf.getDeclaringType().getABaseClass*()
  and mf.getManagedType() = f.getDeclaringType() and
  mf.isOwner()
) 
and
//Cases like:
//```
// class A {
//   F* f;
// }
// class B {
//   std::unique_ptr<A> a;
//   std::unique_ptr<F> f; 
// }
//```
// Class B should be managing both `A` and `F`, this can have interesting consequences if `a` gets reset and `f` doesn't.
// Leave it out for now.
//
not exists(GeneralManagedField ptr_mf, GeneralManagedField mf | f.getDeclaringType() = mf.getManagedType() and
  f.getPointerType() = ptr_mf.getManagedType() and
  mf.getDeclaringType() = ptr_mf.getDeclaringType()
)
and
//Restrict to files of interests.
(f.getFile().getAbsolutePath().matches("%/browser/%") or f.getFile().getAbsolutePath().matches("%/components/%")) and
//exclude protobuf
not f.getFile().getBaseName().matches("%.pb.%") and
//raw pointer has a clean up
exists(Expr p | p = f.getACleanup()) and
//but clean up is not in destructor, so may be skipped
not cleanupInDestructor(f) and
//exclude field that is probably safe (See `ManagedKeyValueField`)
not f instanceof ManagedKeyValueField
and
//FrameServiceBase and raw pointer is `render_frame_host_`, most likely ok as FrameServiceBase observes lifetime of rfh.
not exists(FrameServiceBase fsb | 
  (fsb.getService() = f.getDeclaringType() or fsb = f.getDeclaringType())and
  f.getName() = "render_frame_host_"
) and
//Unlikely to be able to delete.
not f.getPointerType() instanceof OwnedByBrowserMainLoop and
//Unlikely to be able to delete.
not f.getPointerType() instanceof Singleton and
//Generated
not f.getFile().getAbsolutePath().matches("%/out/%") and
//BrowserContext only destroyed during shutdown
not f.getPointerType().hasName("BrowserContext") and
not f.getPointerType() instanceof OwnedByBrowserContext

select f, f.getDeclaringType()
