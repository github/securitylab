import csharp

from MethodCall c
where c.getTarget().hasName("ExtractToFile")
select c.getArgument(1)
