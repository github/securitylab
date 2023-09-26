## V8 type confusion CVE-2023-3420

The analysis of this bug can be found [here](https://github.blog/2023-09-26-getting-rce-in-chrome-with-incorrect-side-effect-in-the-jit-compiler). 

The exploit here is tested on `v8` version 11.4.183.19, which is the version shipped with Chrome 114.0.5735.106, the one before the bug is fixed, on Ubuntu 22.04. I have not tested it on Chrome itself.

To test, check out `v8` at version 11.4.183.19 and compile with the default settings using `tools/dev/gm.py x64.release`. Then open the file `poc.js` with `d8`:

```
./d8 poc.js
```

On Ubuntu 22.04, it should call `execve("/bin/sh")` to spawn a new process:

```
./d8 exploit.js
If succeeded, it should pop a shell and give the following output:
func address: 19ba61
jit code address: c56cd640 55ef
$ 
```
 It should succeed often, but can fail due to the randomness involved in the layout of dictionary objects. A failure does not result a crash and can be detected in the script. In case of running on Chrome, when a failure is detected, the page can simply be reloaded to retry the exploit.

In case of failure, it should print out the following:

```
func address: 7ff80000
jit code address: 9999999a 40019999
exploit failed, please retry
```

Due to the Maglev compiler being shipped with version 114 of Chrome, the exploit may need slight modifications to make sure that the optimized functions are compiled with TurboFan instead of Maglev, otherwise the bug may not trigger.

Shell code may need changing on other platforms.


