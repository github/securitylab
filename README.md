# GitHub Security Lab

This is the main git repository of [GitHub Security Lab](https://securitylab.github.com/).
We use it for these main purposes:

* We share with our community some best practices about security research and vulnerability disclosures in our [docs](/docs)
* We use [issues on this repo](https://github.com/github/securitylab/issues?q=is%3Aissue+is%3Aopen+label%3A%22All+For+One%22) to track CodeQL [bounty requests](https://securitylab.github.com/bounties).
* We use it for publishing some of our proof-of-concept exploits (after the vulnerability has been fixed). These PoCs can be found in the [SecurityExploits](SecurityExploits) sub-directory.
* Examples of CodeQL queries, which can be found in the [CodeQL_Queries](CodeQL_Queries) sub-directory.

## CodeQL Resources

**This section is yours!** Do you want to share a cool CodeQL query with the community? Or an awesome tutorial or video, or some helpful tooling? Your contributions are welcome. Please open a pull request. See [Contributing](#Contributing) below.

### Official resources

* [CodeQL documentation](https://codeql.github.com/docs/)
* [CodeQL GitHub repo](https://github.com/github/codeql)

### Example queries

* Java
  * [Apache Struts CVE-2018-11776](CodeQL_Queries/java/Apache_Struts_CVE-2018-11776)
  * [Insecure JMS deserialization in Spring applications](https://github.com/silentsignal/jms-codeql/)
* C/C++
  * [Apple XNU icmp_error CVE-2018-4407](CodeQL_Queries/cpp/XNU_icmp_error_CVE-2018-4407)
  * [Facebook Fizz integer overflow vulnerability (CVE-2019-3560)](CodeQL_Queries/cpp/Facebook_Fizz_CVE-2019-3560)
  * [Eating error codes in libssh2](CodeQL_Queries/cpp/libssh2_eating_error_codes)
  * [Itergator](https://github.com/trailofbits/itergator) - Library and queries for iterator invalidation ([blog post](https://blog.trailofbits.com/2020/10/09/detecting-iterator-invalidation-with-codeql/))
* Javascript
  * [Etherpad CVE-2018-6835](CodeQL_Queries/javascript/Etherpad_CVE-2018-6835)
* C#
  * [C# Zip Slip demo](CodeQL_Queries/csharp/ZipSlip)
* GitHub Actions:
  * [pull_request_target with explicit pull request checkout](https://github.com/github/codeql/blob/main/javascript/ql/src/experimental/Security/CWE-094/UntrustedCheckout.ql)
  * [Command injection from user-controlled Actions context](https://github.com/github/codeql/blob/main/javascript/ql/src/experimental/Security/CWE-094/ExpressionInjection.ql)

### Articles

* [Practical Introduction to CodeQL](https://jorgectf.gitlab.io/blog/post/practical-codeql-introduction/)

### Videos

* Conference talks/workshops:
  * [Finding security vulnerabilities in JavaScript with CodeQL - GitHub Satellite 2020](https://www.youtube.com/watch?v=pYzfGaLTqC0)
  * [Finding security vulnerabilities in Java with CodeQL - GitHub Satellite 2020](https://www.youtube.com/watch?v=nvCd0Ee4FgE)
  * [CodeQL as an auditing oracle - POC 2020](https://www.youtube.com/watch?v=XmAEgl8bVhg)
  * [mbuf-oflow: Finding Vulnerabilities In iOS/MacOS Networking Code](https://www.youtube.com/watch?v=0EHP2gzwVAY)
* CodeQL demos from the Semmle days (short Youtube videos):
  * [PII data leaks: Identifying personal information in logs with CodeQL](https://www.youtube.com/watch?v=hHaOxbyqy44)
  * [Vulnerability Hunting: Quest for an Exploit using QL](https://www.youtube.com/watch?v=irrYp3wdtsw)
  * [Finding Insecure Deserialization in Java](https://www.youtube.com/watch?v=XsUcSd75K00)
  * [Finding integer overflows in Libssh2](https://www.youtube.com/watch?v=czXicfULOfk)

### Tools

* Editor plugins
  * [Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=GitHub.vscode-codeql) (Official)
  * [Neovim](https://github.com/pwntester/codeql.nvim)
  * [Emacs](https://github.com/anticomputer/emacs-codeql)
* Code generation
  * [cqlgen](https://github.com/gagliardetto/cqlgen) — A codeql generation library written in Go.
  * [codemill](https://github.com/gagliardetto/codemill) — A codeql model generator for Go with a web UI. 

## Disclaimer

The recommendations from the GitHub Security Lab are provided graciously and it's ultimately the responsibility of the recipients to apply them or not. This concerns recommendations given through our written or audio content, our conferences, our answers in our community spaces, or our informal office hours.

## Contributing

We welcome contributions to the [CodeQL_Queries](CodeQL_Queries) sub-directory and to the [CodeQL Resources](#codeql-resources) section of this README.

If you have written a cool CodeQL query that you would like to share with the community, then please open a pull request to add it to the [CodeQL_Queries](CodeQL_Queries) sub-directory. Put your query in its own new sub-directory. For example: `CodeQL_Queries/cpp/mynewsubdir/mycoolquery.ql`. Of course, if you think your query might be eligible for a [bounty](https://securitylab.github.com/bounties), then you should open a pull request to the [codeql](https://github.com/github/codeql) repo instead, as we do not offer bounties for queries submitted to this repo. The queries in this repo are usually highly specialized queries that only make sense for a specific codebase, such as queries that specifically target [Chrome](CodeQL_Queries/cpp/Chrome) or [Apache Struts](CodeQL_Queries/java/Apache_Struts_CVE-2018-11776), or utility queries that help you explore your code without necessarily finding a vulnerability. Such queries are inappropriate for the [codeql](https://github.com/github/codeql) repo, which is for general purpose queries only.

If you would like to add a link to the [CodeQL Resources](#codeql-resources) section of this README, to share a nice video or an awesome tool, then just add another bullet point in the appropriate section.
* Each bullet point should consist of a hyperlinked title and a short description. The short description is optional if the title is already self-explanatory.
* Please add new bullet points at the bottom of the list. In the future, we may choose some other ordering such as alphabetical but for now it is just a sequential list.

Please see [CONTRIBUTING.md](CONTRIBUTING.md), [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md), and [LICENSE.md](LICENSE.md) for further information on our contributing guidelines and license.
