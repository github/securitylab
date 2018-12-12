# Command injection in CImg

This is a proof of concept for a command injection vulnerability in the [CImg](http://cimg.eu/) library. The vulnerability was found by [Cristian-Alexandru Staicu](https://www.linkedin.com/in/crstaicu/), during his internship at Semmle in 2018. We reported the vulnerability to David Tschumperle, maintainer of CImg, on Jul 27, 2018. The vulnerability was [fixed](https://github.com/dtschump/CImg/commit/5ce7a426b77f814973e56182a0e76a2b04904146) in version 2.3.4.

The problem is that the `load_network` function does not do any sanitization on the url string. Internally, `load_network` calls `system`, which means that a specially crafted url can trigger code execution. Since CImg is a library, the severity of the issue depends greatly on how it is used. If anyone has written an application that calls `load_network` directly with a string that came from something like a HTTP request, then it would be a remote code execution vulnerability.

To run the PoC, first build and run the docker image:

```bash
docker build . -t cimg
docker run -i -t cimg
```

The Dockerfile clones the [CImg](https://framagit.org/dtschump/CImg.git) git repository and checks out the vulnerable version.

Now, inside docker, compile and run the PoC as follows:

```bash
g++ -I./CImg poc.c -o poc
./poc
```

Notice that the file `~/CImg-RCE` has now been created.
