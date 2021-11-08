name: All for One, One For All bounty submission
description: Submit a CodeQL query for the All For One, One For All bounty (https://securitylab.github.com/bounties)
title: "[<language>]: <short description>"
labels: [All For One]
body:
  - type: markdown
    attributes:
      value: |
        # Introduction

        Thank you for submitting a query to the GitHub CodeQL project!

        After you submit this issue, the GitHub Security Lab and CodeQL teams will triage the submission and, if it meets the Query Bounty Program requirements, we will grant you a bounty through our HackerOne program.

        Please make sure to carefully read the [bounty program description and conditions](https://securitylab.github.com/bounties/)

        # Questionnaire
  - type: input
    id: pr_url 
    attributes:
      label: Query PR
      description: Link to pull request with your CodeQL query
      placeholder: ex. https://github.com/github/codeql/pull/nnnn
    validations:
      required: true
  - type: dropdown
    id: language
    attributes:
      label: Language
      description: What programming language is your query written for?
      options:
        - Java
        - Javascript
        - GoLang
        - Python
        - C/C++
        - C#
    validations:
      required: true
  - type: input
    id: cwe
    attributes:
      label: CWE
      description: CWE that best fits the vulnerability class modeled with your query
      placeholder: ex. CWE-502: Deserialization of Untrusted Data
    validations:
      required: false
  - type: textarea
    id: cve_ids
    attributes:
      label: CVE(s) ID list
      description: Enter a list of the CVE ID(s) associated with this query, one bullet for each distinct CVE. GitHub will automatically link CVE IDs to the [GitHub Advisory Database](https://github.com/advisories).
      placeholder: |
        ex.
        - CVE-20nn-xxxx
        - CVE-20nn-yyyy
    validations:
      required: true
  - type: textarea
    id: report
    attributes:
      label: Report
      description: Describe the vulnerability. Provide any information you think will help GitHub assess the impact your query has on the open source community.
    validations:
      required: true
  - type: checkboxes
    id: publish
    attributes:
      options:
        - label: Are you planning to discuss this vulnerability submission publicly? (Blog Post, social networks, etc). *We would love to have you spread the word about the good work you are doing*
  - type: textarea
    id: results
    attributes:
      label: Result(s)
      description: Provide at least one useful result found by your query, on some revision of a real project. One bullet for each distinct result.
      placeholder: |
        ex.
        - [project1](url1)
        - [project2](url2)
    validations:
      required: true
