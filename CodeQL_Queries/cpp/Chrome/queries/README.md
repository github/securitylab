# Example queries

The queries in this directory are more of an example of how to use the CodeQL libraries in the repository, rather than concrete queries used for finding bugs. Many of these require tweaking filters to restrict the results to some subset of the Chromium codebase.

Here is an overview of what they do:

## Callback tracking

These queries use the `callbacks.qll` library and are useful in checking raw pointers that are stored in a particular callback. The library `callbacks.qll` divides callbacks into the following types: `CallbackFieldSink`, `CallbackPostTaskSink`, `CallbackRunSink`, `CallbackInterfacePtrSink` that allows further specification of callback types. In general, it is recommended to run this query without restriction first, and use the result as a lookup table during manual inspection, instead of running the query every time with different restrictions to the sink, as the former will be more efficient.

### `callback_unretained.ql`

Used to identify the pointer types that are stored inside a callback. This is a very useful query for investigation as callbacks often contain other callbacks and it can take some time to figure out what is stored in them.

### `callback_unretained_field.ql`

Like `callback_unretained.ql`, but specifically looks for callback fields and outputs the specific field associated with the callback. This is often useful as callback fields can be hard to track manually.

## Pointer cleanup

These queries are used for looking up the cleanup pattern of raw pointer fields. This is generally achieved by matching types of the field with cleanup operations performed on fields of the same type, rather than tracking the precise movement of the field itself. This is a compromise that has to be made in order to reduce the complexity of the queries and make them feasible, both in terms of the manual effort involved in writing the libraries and the running time of the queries. In practice, this compromise works out mostly ok.

### `no_cleanup.ql`

A query that is used to identify raw pointer fields that does not get set to nullptr, or gets erased from containers (clean up), while removing some cases that are less likely to be dangerous.

### `non_trivial_cleanup.ql`

This is similar to no_cleanup.ql, but instead looks for raw pointers that do get cleaned up, but the clean up is not called (transitively) from the destructor of the pointer type. The idea is that there may be ways to destroy the pointer without removing the field (as the destructor does not do the clean up).

