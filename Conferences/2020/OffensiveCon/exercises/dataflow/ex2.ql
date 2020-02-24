import cpp
import semmle.code.cpp.dataflow.DataFlow

class KMalloc extends Function {
  KMalloc() {
    getName() = "kmalloc" or
    getName() = "acpi_os_allocate_zeroed" or
    getName() = "kzalloc" or
    getName() = "kcalloc" or
    getName() = "kmalloc_array" or
    getName() = "acpi_os_allocate" or
    getName() = "mempool_kmalloc" or
    getName() = "alloc_resource" or
    getName() = "bitmap_alloc" or
    getName() = "sg_kmalloc" or
    getName() = "pcpu_mem_zalloc" or
    getName() = "bitmap_zalloc"
  }
}

from KMalloc fun, FunctionCall source
where
  source = fun.getACallToThisFunction() and
  not exists(IfStmt sink |
    DataFlow::localExprFlow(source, sink.getControllingExpr().getAChild*())
  )
select source
