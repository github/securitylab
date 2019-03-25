import csharp

from Property p
where p.hasName("FullName")
select p.getAnAccess()
