# Unary minus integer overflow gotcha

This demo is about building a query to find the bug from
[this tweet by Nico Waisman](https://twitter.com/nicowaisman/status/1147178477692608512).

This is the pattern that we're interested in:

```
int32_t Size = user_supplied32();
if(Size < 0) {
   Size = -Size;
}
```

The developer who wrote this probably thinks that `Size` is now a positive number.
But they have forgotten that `MIN_INT` will trigger an integer overflow and remain
negative.
So if the subsequent code relies on `Size` being positive, then something could
go badly wrong.

## Generating a snapshot.

This directory contains a unit test file, [`test.cpp`](test.cpp),
which you can use to create a small database for testing, like this:

```
codeql database create MinIntNegateDB --language=cpp --command="g++ -c test.cpp"
```

This creates a database in a sub-directory named `MinIntNegateDB`.
You can add this database in the
[CodeQL for VSCode extension](https://github.com/github/vscode-codeql)
by clicking the `+` button.
