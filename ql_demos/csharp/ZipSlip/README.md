# C# Zip Slip demo

## Snapshot

Use [this snapshot](http://downloads.lgtm.com/snapshots/csharp/microsoft/powershell/PowerShell_PowerShell_csharp-srcVersion_450d884668ca477c6581ce597958f021fac30bff-dist_odasa-lgtm-2018-09-11-e5cbe16-linux64.zip)
of PowerShell.

## Introduction

The "Zip Slip" vulnerability was announced on June 5th 2018, by [Snyk](https://snyk.io/research/zip-slip-vulnerability).
You can see on [this page](https://snyk.io/research/zip-slip-vulnerability#dot-net) some sample code that shows the vulnerable code.

Microsoft immediately wanted to search their codebase to see if any of their own code was vulnerable. Within a few days,
they had written a basic query and run it on their own LGTM-e instances that turned up several results across
several codeases. Because Semmle has a close working relationship with Microsoft, we then helped Microsoft to refine
that query further and submit it as a [pull request](https://github.com/Semmle/ql/pull/54) against our open source QL repository.

It was deployed within 2 weeks where it was run over thousands of open source C# projects.

Here are some [sample results](https://lgtm.com/rules/1506511188430/alerts/) for the ZipSlip query. 
One of those projects was Microsoft PowerShell.

As a result of this query, [a senior Microsoft engineer](https://github.com/TravisEz13)
fixed this query in November 2018 in
[this PR](https://lgtm.com/projects/g/PowerShell/PowerShell/rev/b39a41109d86d9ba75f966e2d7b52b81fa629150).

So how did they do it?

## Query 1-4: Exploring sources and sinks

Open the snapshot in QL4E, and show QL as a simple query language for identifying sources and sinks. Look for sinks
(calls to `ExtractToFile`), noting that we actually want to identify the vulnerable argument.

Sources are identified by for example the `FullName` property of `ZipArchiveEntry`. If we omit the 
name of the declaring type, we get too many results, and the full query would use the qualified name of
the property.

Already we've found the vulnerable code.

## Query 5

This query uses local dataflow to find data flow from the source to the sink. We actually need to use taint tracking
due to the use of `Path.Combine`.

## Query 6

This uses a global taint tracking configuration.

# Final query

The [final query](https://lgtm.com/rules/1506511188430/) includes query help, and identifies various other sources and sinks,
but uses the same general structure. It also includes metadata for LGTM.
