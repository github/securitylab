---
next: docs/hello-world.md
title: Introduction
layout: docs.liquid
---

# Introduction

If you've ever thought, "wouldn't it be cool if GitHub could…"; imma stop you right there. Most features can actually be added via [GitHub Apps](https://docs.github.com/apps/), which extend GitHub and can be installed directly on organizations and user accounts and granted access to specific repositories. They come with granular permissions and built-in webhooks. Apps are first class actors within GitHub.

**Probot is a framework for building [GitHub Apps](http://docs.github.com/apps) in [Node.js](https://nodejs.org/)**. It aims to eliminate all the drudgery–like receiving and validating webhooks, and doing authentication handstands–so you can focus on the features you want to build.

Probot apps are easy to write, deploy, and share. Many of the most popular Probot apps are hosted, so there's nothing for you to deploy and manage. Here are just a few examples of things that have been built with Probot:

{% include "docs/examples" %}

Check out the [featured apps](https://probot.github.io/apps/) or [browse more examples on GitHub](https://github.com/search?q=topic%3Aprobot-app&type=Repositories)

Ready to get started?

# Probot

[![npm version](https://img.shields.io/npm/v/probot.svg)](https://www.npmjs.com/package/probot) [![](https://img.shields.io/twitter/follow/ProbotTheRobot.svg?style=social&logo=twitter&label=Follow)](https://twitter.com/ProbotTheRobot)

> 🤖 Um framework para criar aplicativos do GitHub para automatizar e melhorar seu fluxo de trabalho

Se você já pensou, "não seria legal se o GitHub pudesse..."; Eu vou parar você aí mesmo. A maioria dos recursos pode realmente ser adicionada via [GitHub Apps](https://docs.github.com/apps/), que estende o GitHub e pode ser instalado diretamente em organizações e contas de usuários e com acesso a repositórios específicos. Eles vêm com permissões granulares e webhooks integrados. Os aplicativos são atores de primeira classe no GitHub.

## Como funciona

**Probot é um framework para construir [GitHub Apps](https://docs.github.com/apps) em [Node.js](https://nodejs.org/)**, escrito em [TypeScript](https://www.typescriptlang.org/). O GitHub Apps pode ouvir eventos de webhook enviados por um repositório ou organização. O Probot usa seu emissor de evento interno para executar ações com base nesses eventos. Um aplicativo Probot simples pode ter esta aparência:

```js
module.exports = (app) => {
  app.on("issues.opened", async (context) => {
    const issueComment = context.issue({
      body: "Obrigado por abrir esta issue!",
    });
    return context.octokit.issues.createComment(issueComment);
  });
};
```

## Criando um app Probot

Se você acessou este repositório GitHub e está procurando começar a construir seu próprio aplicativo Probot, não precisa procurar mais do que [probot.github.io](https://probot.github.io/docs/)! O site Probot contém nossa extensa documentação inicial e o guiará pelo processo de configuração.

Este repositório hospeda o código do pacote npm Probot, que é o que todos os Apps Probot executam. A maioria das pessoas que vem neste repositório provavelmente estão querendo começar [construindo seu próprio aplicativo](https://probot.github.io/docs/).

## new robot automatic

Probot é construído por pessoas como você! A maioria das coisas interessantes são construídas com o Probot, então considere começar [escrevendo um novo aplicativo](https://probot.github.io/docs/) ou melhorando [um dos existentes](https://github.com/search?q=topic%3Aprobot-app&type=Repositories).

Se você estiver interessado em contribuir com o Probot, confira nossa [doc de contribuição](CONTRIBUTING.md) para começar.

Quer conversar com usuários Probot e colaboradores? [Junte-se a nós no Slack](https://probot-slackin.herokuapp.com/)!

## Ideias

Tem uma ideia para um novo app GitHub legal (feito com o Probot)? Isso é ótimo! Se você quer feedback, ajuda, ou apenas para compartilhá-lo com o mundo, você pode fazer isso [criando uma issue no repositório `probot/ideas`](https://github.com/probot/ideas/issues/new)!


*This vulnerability report template is offered to you by the GitHub Security Lab. Use it as an inspiration for your own reports. Reporting a vulnerability using this template does not imply that this report has been acknowledged by the GitHub Security Lab. Remove this first section and any mention of the GitHub Security Lab when you use this template.* 

# Vulnerability Report

I identified potential security vulnerabilities in [product].

I am committed to working with you to help resolve these issues. In this report you will find everything you need to effectively coordinate a resolution of these issues.

If at any point you have concerns or questions about this process, please do not hesitate to reach out to me at [email].

If you are _NOT_ the correct point of contact for this report, please let me know!

## Summary

*Short summary of the problem. Make the impact and severity as clear as possible. For example: An unsafe deserialization vulnerability allows any unauthenticated user to execute arbitrary code on the server.*

## Product

[product]

## Tested Version

[version]

## Details

*Give all details on the vulnerability. Pointing to the incriminated source code is very helpful for the maintainer.*

## PoC

*Complete instructions, including specific configuration details, to reproduce the vulnerability*

## Impact

[impact]

## Remediation

*Propose a remediation suggestion if you have one. Make it clear that this is just a suggestion, as the maintainer might have a better idea to fix the issue.*

## GitHub Security Advisories

If possible, please could you create a private [GitHub Security Advisory](https://help.github.com/en/github/managing-security-vulnerabilities/creating-a-security-advisory) for these findings? This allows you to invite me to collaborate and further discuss these findings in private before they are [published](https://help.github.com/en/github/managing-security-vulnerabilities/publishing-a-security-advisory). I will be happy to collaborate with you, and review your fix to make sure that all corner cases are covered. 
When you use a GitHub Security Advisory, you can request a CVE identification number from GitHub. GitHub usually reviews the request within 72 hours, and the CVE details will be published after you make your security advisory public. Publishing a GitHub Security Advisory and a CVE will help notify the downstream consumers of your project, so they can update to the fixed version.

## Credit

*List all researchers who contributed to this disclosure.*
*If you found the vulnerability with a specific tool, you can also credit this tool.*

## Contact

[contact]

## Disclosure Policy

*Describe or link to your disclosure policy. It's important to have a disclosure policy where the public disclosure deadline, and the potential exceptions to it, are clear. You are free to use the [GitHub Security Lab disclosure policy](https://securitylab.github.com/advisories/#policy), which is copied below for your convenience, if it resonates with you.*

The *your_team_name_here* research team is dedicated to working closely with the open source community and with projects that are affected by a vulnerability, in order to protect users and ensure a coordinated disclosure. When we identify a vulnerability in a project, we will report it by contacting the publicly-listed security contact for the project if one exists; otherwise we will attempt to contact the project maintainers directly.

If the project team responds and agrees the issue poses a security risk, we will work with the project security team or maintainers to communicate the vulnerability in detail, and agree on the process for public disclosure. Responsibility for developing and releasing a patch lies firmly with the project team, though we aim to facilitate this by providing detailed information about the vulnerability.

Our disclosure deadline for publicly disclosing a vulnerability is: 90 days after the first report to the project team.

We **appreciate the hard work** maintainers put into fixing vulnerabilities and understand that sometimes more time is required to properly address an issue. We want project maintainers to succeed and because of that we are always open to discuss our disclosure policy to fit your specific requirements, when warranted.
