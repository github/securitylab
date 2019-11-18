import csharp

from Property p
where
  p.hasName("FullName") and
  p.getDeclaringType().hasName("ZipArchiveEntry")
select p.getAnAccess()
