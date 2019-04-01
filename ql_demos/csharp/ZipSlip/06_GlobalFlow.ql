import csharp
import semmle.code.csharp.dataflow.TaintTracking

class ZipSlipTaintTracking extends TaintTracking::Configuration {
  ZipSlipTaintTracking() { this = "Zip Slip taint tracking" }

  override predicate isSource(DataFlow::Node node) {
    exists(Property p |
      p.hasName("FullName") and
      p.getDeclaringType().hasName("ZipArchiveEntry") and
      node.asExpr() = p.getAnAccess()
    )
  }

  override predicate isSink(DataFlow::Node node) {
    exists(MethodCall call | call.getTarget().hasName("ExtractToFile") |
      node.asExpr() = call.getAnArgument()
    )
  }
}

from ZipSlipTaintTracking config, DataFlow::Node source, DataFlow::Node sink
where config.hasFlow(source, sink)
select sink, "Zip Slip vulnerability from $@.", source, source.toString()
