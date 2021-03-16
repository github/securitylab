## Chrome Beta Sandbox Escape GHSL-2020-165

The write up can be found [here](https://securitylab.github.com/research/one_day_short_of_a_fullchain_sbx). This is a bug in the beta version of Chrome v86 I reported in September 2020. The GitHub Advisory can be found [here](https://securitylab.github.com/advisories/GHSL-2020-165-chrome) and the Chrome issue Chrome Issue [here](https://bugs.chromium.org/p/chromium/issues/detail?1125614). The bug can be used to escape the Chrome sandbox from a compromised renderer.

The exploit is tested on the 64 bit beta version 86.0.4240.30 of Chrome with the following build config (`args.gn`):

```
target_os = "android"
target_cpu = "arm64"
is_java_debug = false
is_debug = false
symbol_level = 1
blink_symbol_level = 1
```

The exploit is tested on Samsung Galaxy A71 with firmware version A715FXXU3ATJ2, Baseband A715FXXU3ATI5 and Kernel version 4.14.117-19828683. The offsets in the exploit assume this version of the firmware. For other firmware, use the offsets in the corresponding libraries `libhwui.so` and `libc.so` under `system/lib64`. (64 bit) It requires production firmware on the phone and would fail on emulators and phones with development firmware (i.e. OS built from AOSP source) The actual offsets of these libraries also needs to be updated to the ones obtained from the compromised renderer.

It should succeed most of the time and rarely crash. If successful, it'll run the shell command in the `command` variable in the file `payment_request_clip.html`, which would create a file called `pwn` under the directory `/data/data/org.chromium.chrome/`. It can be replaced with other shell commands.

To test, check out version 86.0.4240.30 of Chrome following [these instructions](https://chromium.googlesource.com/chromium/src/+/master/docs/android_build_instructions.md), then apply the file `sbx.patch` to the simulate a compromised renderer. Then build the `chrome_public_apk` target.

Install the resulting apks (under `out/<target>/apks`) on the phone using `adb`, then enable the `MojoJS` feature to simulate a compromised renderer:

1. Enable `Enable command line on non-rooted devices` from `chrome://flags`
2. Create a file in `/data/local/tmp/chrome-command-line` in the phone and then add `chrome --enable-blink-features=MojoJS` to the file
3. Force stop Chrome and restart

Then create a directory to host the `html` files included in this directory, and run `copy_mojo_js_bindings.py` to copy the mojo bindings to the directory and host the files on localhost:

```
python ./copy_mojo_js_bindings.py /path/to/chrome/../out/<target>/gen
python -m SimpleHTTPServer
```

Then open the page `payment_request_clip.html` from Chrome on the device. The easiest way is to use the `chrome://inspect/#devices` tool to set up the proxies etc. and open the url.

If successful, the shell command will run and a file called `pwn` will be created in the directory `/data/data/org.chromium.chrome/` in the phone. If failed, click on the link to reload the page and try again (can't use reload for this one). It shouldn't need too many retries to succeed.
