## Overview
This vulnerability is caused by unsafe use of the reflection mechanisms

An attacker may be able to create unexpected control flow paths through the application, potentially bypassing security checks.
Exploitation of this weakness can result in a limited form of code injection.

This vulnerability happens when a programmer refactor code using reflection

## Recommendation

Use reflection only on user trusted input.
```java
// BAD: class_name is user controlled input
String class_name = System.getenv("CLASS_NAME");
Class class = Class.forName(class_name);

// GOOD: use string constant
String class_name = "awt";
Class class = Class.forName(class_name);
```

## References

- OWASP: [Unsafe reflection](https://owasp.org/www-community/vulnerabilities/Unsafe_use_of_Reflection)

