# Code QL library for Chrome

This repository contains various [CodeQL](https://codeql.com) libraries for [Chromium](https://chromium.googlesource.com/chromium/src.git). In order to use it, follow the instructions to install the [CodeQL plugin in eclipse](https://help.semmle.com/ql-for-eclipse/Content/WebHelp/installation.html) and then import this repository as an eclipse project.

## Building snapshot

As the Chromium source code is huge. In order to use CodeQL with it, you are likely to need to build your own Chromium snapshot and restrict it to a specific target, instead of building and running CodeQL on the whole of Chromium. This can be achieved by a two stage build. For example, if the directory of interest is `content/browser` (e.g. when researching Chromium IPC), then first build the entire Chromium. Next, remove all the build artifacts (*.o, *.so, *.a) in the `src/out/<target>/obj/content/browser/` directory (including subdirectories) and rebuild with CodeQL, following the instructions [here](https://help.semmle.com/codeql/codeql-cli.html). This should result in a usable size snapshot. Note, however, that building with CodeQL CLI is a memory intensive process and even a restricted snapshot would take up a huge amount of RAM (e.g. `content/browser` requires at least 50-60GB of RAM to build) and I'd recommended building it in a VM in the cloud.

## Overview of various libraries

The libraries in this repository are organized as follows:

### `commons.qll`:

Mostly contain general enchancements to the standard QL library and some Chromium specific, but still general material. For example, because the operators `->` and `=` are often overloaded in Chrome, this somehow upsets the usual `getQualifier` and `Assignment` in QL, so two general methods, `getQualifier` and `GeneralAssignment` are implemented to take these into account. Other useful predicates are `constructionCall` and `polyConstructionCall`, which identify all the constructor calls, as well as constructions via `make_unique` and `MakeRefCounted`. `make_unique` works fine in code search, but not `MakeRefCounted`. Neither will work on vanilla QL, where you just end inside the standard library.

### `collections.qll`:

Generally deals with `std::map`, `std::vector` etc. I use some heuristics there because there are just too many different types of containers and it is likely to miss some if I add them manually. The library provides methods that get the component types of a container and also function calls that set/reset components in a container. A set/reset method call (e.g. push_back/erase/clear etc.) are useful as they are needed to identify when raw pointers might get removed and when managed pointers may get reset (which will cause the underlying object to be deleted and may cause a UaF somewhere else). Many bugs are actually related to pointers/managed pointers stored in containers.

### `callbacks.qll`:

The is a very useful library. It provides utilities to track the pointer types that are stored inside a callback, both retained and unretained. It uses dataflow in `callback_tracking.qll` to do this and can track through callback fields and multiple callbacks, e.g.

```c
A* a;
...
assignCallback(base::BindOnce(&foo, Unretained(a)));


void assignCallback(Callback callback) {
  callback_ = std::move(callback);
}

...
void bar() {
  x = base::BindOnce(&foo2, std::move(callback_));  //<-- x unretains type A via the assignCallback call.
}
```

I normally use this as a look up during investigation to see whether a particular callback is dangerous or not.

### `callback_tracking.qll`

Only used by `callbacks.qll` to track the types stored in callbacks.

### `bindings.qll`

Used for modelling the Mojo interface.

## `pointers`: 

QL libraries for managed and raw pointers. 

## `object_lifetime`:

### `lifetime_management.qll`:

QL library that models various clean up logic in Chrome. I haven't got round to implementing too much code there yet.

`obj_lifetime.qll`:

Mostly contains classes that are long living, e.g. BrowserContext, Singleton and the objects that they manage. Mostly used to exclude raw pointer results as these are not likely to be destroyed.
