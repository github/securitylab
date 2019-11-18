import csharp
import semmle.code.csharp.dataflow.TaintTracking

from DataFlow::Node source, DataFlow::Node sink, MethodCall c, Property p
where
  c.getTarget().hasName("ExtractToFile") and
  p.hasName("FullName") and
  p.getDeclaringType().hasName("ZipArchiveEntry") and
  sink.asExpr() = c.getAnArgument() and
  source.asExpr() = p.getAnAccess() and
  TaintTracking::localTaint(source, sink)
select sink, "ZipSlip from $@.", source, source.toString()
