# Eating error codes in libssh2

Download this [snapshot](https://github.com/github/securitylab/releases/download/libssh2-codeql-database/libssh2_libssh2_C_C++_38bf7ce.zip) for the demo.

This demo shows how to develop, step-by-step, the query from the [blog post](https://blog.semmle.com/libssh2-integer-overflow/) about libssh2 CVE-2019-13115. This query did not find the bug that caused the CVE. It is instead about doing variant analysis on a bug that we noticed on the development branch of libssh2. We sent the query results to the libssh2 development team and they were able to fix all the variants before the next version of libssh2 was released.

The problem is that `_libssh2_get_c_string` returns a negative integer as an error code, but the type of `r_len` is `unsigned int`, so the error code is accidentally ignored.

For a shorter demo, stop at step 02. Steps 03 and 04 make the query more sophisticated by adding local data flow and range analysis.
