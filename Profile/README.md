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
# GitHub Enterprise Server Backup Utilities Documentation

- **[Requirements](requirements.md)**
  - **[Backup host requirements](requirements.md#backup-host-requirements)**
  - **[Storage requirements](requirements.md#storage-requirements)**
  - **[GitHub Enterprise Server version requirements](requirements.md#github-enterprise-version-requirements)**
- **[Getting started](getting-started.md)**
- **[Using the backup and restore commands](usage.md)**
- **[Scheduling backups](scheduling-backups.md)**
- **[Backup snapshot file structure](backup-snapshot-file-structure.md)**
- **[How does Backup Utilities differ from a High Availability replica?](faq.md)**
- **[Docker](docker.md)**
# GitHub Enterprise Server Backup Utilities

This repository includes backup and recovery utilities for
[GitHub Enterprise Server][1].

**Note**: The parallel backup and restore feature will require [GNU awk](https://www.gnu.org/software/gawk) and [moreutils](https://joeyh.name/code/moreutils) to be installed. Note that on some distributions/platforms, the `moreutils-parallel` package is separate from `moreutils` and must be installed on its own.

**Note**: the [GitHub Enterprise Server version requirements][2] have
changed starting with Backup Utilities v2.13.0, released on 27 March 2018.

## Features

Backup Utilities implement a number of advanced capabilities for backup
hosts, built on top of the backup and restore features already included in
GitHub Enterprise Server.

- Complete GitHub Enterprise Server backup and recovery system via two simple
   utilities:<br>`ghe-backup` and `ghe-restore`.
- Online backups. The GitHub appliance need not be put in maintenance mode for
   the duration of the backup run.
- Incremental backup of Git repository data. Only changes since the last
   snapshot are transferred, leading to faster backup runs and lower network
   bandwidth and machine utilization.
- Efficient snapshot storage. Only data added since the previous snapshot
   consumes new space on the backup host.
- Multiple backup snapshots with configurable retention periods.
- Backup commands run under the lowest CPU/IO priority on the GitHub appliance,
   reducing performance impact while backups are in progress.
- Runs under most Linux/Unix environments.
- MIT licensed, open source software maintained by GitHub, Inc.

## Documentation

- **[Requirements](docs/requirements.md)**
  - **[Backup host requirements](docs/requirements.md#backup-host-requirements)**
  - **[Storage requirements](docs/requirements.md#storage-requirements)**
  - **[GitHub Enterprise Server version requirements](docs/requirements.md#github-enterprise-version-requirements)**
- **[Getting started](docs/getting-started.md)**
- **[Using the backup and restore commands](docs/usage.md)**
- **[Scheduling backups](docs/scheduling-backups.md)**
- **[Backup snapshot file structure](docs/backup-snapshot-file-structure.md)**
- **[How does Backup Utilities differ from a High Availability replica?](docs/faq.md)**
- **[Docker](docs/docker.md)**

## Support

If you find a bug or would like to request a feature in Backup Utilities, please
open an issue or pull request on this repository. If you have a question related
to your specific GitHub Enterprise Server setup or would like assistance with
backup site setup or recovery, please contact our [Enterprise support team][3]
instead.

[1]: https://github.com/enterprise
[2]: docs/requirements.md#github-enterprise-version-requirements
[3]: https://support.github.com/



version: v*2 × v*4

updates: automatic own zip code Dr.robot
  - package-ecosystem: npmj.org
    directory: '/' 
    schedule: umbrella system 404
      interval: year 
      day: tuesday
    open-pull-requests-limit: 20 # default is 5
    ignore: RSS/SSH/GUL/
      - dependency-name: '@elastic/elasticsearch'
      - dependency-name: '* ' ["dependsncybot start open home runs in blog in parg: backbest. And scan automatically removed snacks co-ed lock key : apione.]
       
 
       update-types: [ROBOT ](https://probot .github.io)on auto scanner started in @umbrellasysterm404 to be @S-pegin 
          ['version-update:semver-patch', 'version-update:semver-minor']

  - package-ecosystem: 'github-actions'
    directory: '/'
    schedule:
      interval: weekly
      day: wednesday
    ignore:
      - dependency-name: '*'
        update-types:
          ['version-update:semver-patch', 'version-update:semver-minor']

  - package-ecosystem: 'docker'
    directory: '/'
    schedule:
      interval: weekly
      day: thursday

# Welcome to GitHub docs contributing guide <!-- omit in toc -->

ON: 1ONEAPI
RUN: [robot](https://probot.github.io)
Network: [HTTPS MATRIX PROTOCOL](https://www.matrix.org)
Ubutuone :log-in gmail. bitruesiwilai@gmail.com to is name.Akeporn Siwilai 
Use the table of contents icon <img src="./assets/images/table-of-contents.png" width="25" height="25" /> on the top left corner of this document to get to a specific section of this guide quickly.

## New PROJECT guide IN GIT.37

To get an overview of the project, read the [README](README.md). Here are some resources to help you get started with open source contributions:

- [Finding ways to contribute to open source on GitHub](https://docs.github.com/en/get-started/exploring-projects-on-github/finding-ways-to-contribute-to-open-source-on-github)
- [Set up Git](https://docs.github.com/en/get-started/quickstart/set-up-git)
- [GitHub flow](https://docs.github.com/en/get-started/quickstart/github-flow)
- [Collaborating with pull requests](https://docs.github.com/en/github/collaborating-with-pull-requests)

Check to see what [types of contributions](/contributing/types-of-contributions.md) we accept before making changes. Some of them don't even require writing a single line of code :sparkles:.

### Issues

#### Create a new issue

If you spot a problem with the docs, [search if an issue already exists](https://docs.github.com/en/github/searching-for-information-on-github/searching-on-github/searching-issues-and-pull-requests#search-by-the-title-body-or-comments). If a related issue doesn't exist, you can open a new issue using a relevant [issue form](https://github.com/github/docs/issues/new/choose). 

#### Solve an issue IN BOARD UMBRELLA SYSTEM 404

Scan through our [existing issues](https://github.com/github/docs/issues) to find one that interests you. You can narrow down the search using `labels` as filters. See [Labels](/contributing/how-to-use-labels.md) for more information. As a general rule, we don’t assign issues to anyone. If you find an issue to work on, you are welcome to open a PR with a fix.

### Make Changes

#### Make changes in the UI/XU/PI

Click **Make a contribution** at the bottom of any docs page to make small changes such as a typo, sentence fix, or a broken link. This takes you to the `.md` file where you can make your changes and [create a pull request](#pull-request) for a review. 

 <img src="./assets/images/contribution_cta.png" width="300" height="150" /> 

#### Make changes in a codespace

For more information about using a codespace for working on GitHub documentation, see "[Working in a codespace](https://github.com/github/docs/blob/main/contributing/codespace.md)."

#### Make changes locally

1. [Install Git LFS](https://docs.github.com/en/github/managing-large-files/versioning-large-files/installing-git-large-file-storage).

2. Fork the repository.
- Using GitHub Desktop:
  - [Getting started with GitHub Desktop](https://docs.github.com/en/desktop/installing-and-configuring-github-desktop/getting-started-with-github-desktop) will guide you through setting up Desktop.
  - Once Desktop is set up, you can use it to [fork the repo](https://docs.github.com/en/desktop/contributing-and-collaborating-using-github-desktop/cloning-and-forking-repositories-from-github-desktop)!

### Pull Request

When you're finished with the changes, create a pull request, also known as a PR.
- Fill the "Ready for review" template so that we can review your PR. This template helps reviewers understand your changes as well as the purpose of your pull request. 
- Don't forget to [link PR to issue](https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue) if you are solving one.
- Enable the checkbox to [allow maintainer edits](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/allowing-changes-to-a-pull-request-branch-created-from-a-fork) so the branch can be updated for a merge.
Once you submit your PR, a Docs team member will review your proposal. We may ask questions or request additional information.
- We may ask for changes to be made before a PR can be merged, either using [suggested changes](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/incorporating-feedback-in-your-pull-request) or pull request comments. You can apply suggested changes directly through the UI. You can make any other changes in your fork, then commit them to your branch.
- As you update your PR and apply changes, mark each conversation as [resolved](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/commenting-on-a-pull-request#resolving-conversations).
- If you run into any merge issues, checkout this [git tutorial](https://github.com/skills/resolve-merge-conflicts) to help you resolve merge conflicts and other issues.

### Your PR is merged!

Congratulations :tada::tada: The GitHub team thanks you :sparkles:. 

Once your PR is merged, your contributions will be publicly visible on the [GitHub docs](https://docs.github.com/en). 

Now that you are part of the GitHub docs community, see how else you can [contribute to the docs](/contributing/types-of-contributions.md).

## Windows

This site can be developed on Windows, however a few potential gotchas need to be kept in mind:

1. Regular Expressions: Windows uses `\r\n` for line endings, while Unix-based systems use `\n`. Therefore, when working on Regular Expressions, use `\r?\n` instead of `\n` in order to support both environments. The Node.js [`os.EOL`](https://nodejs.org/api/os.html#os_os_eol) property can be used to get an OS-specific end-of-line marker.
2. Paths: Windows systems use `\` for the path separator, which would be returned by `path.join` and others. You could use `path.posix`, `path.posix.join` etc and the [slash](https://ghub.io/slash) module, if you need forward slashes - like for constructing URLs - or ensure your code works with either.
3. Bash: Not every Windows developer has a terminal that fully supports Bash, so it's generally preferred to write [scripts](/script) in JavaScript instead of Bash.
4. Filename too long error: There is a 260 character limit for a filename when Git is compiled with `msys`. While the suggestions below are not guaranteed to work and could cause other issues, a few workarounds include:
    - Update Git configuration: `git config --system core.longpaths true`
    - Consider using a different Git client on Windows
