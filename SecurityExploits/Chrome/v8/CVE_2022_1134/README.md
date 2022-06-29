#Chrome renderer RCE CVE-2022-1134

The write up can be found [here](https://github.blog/2022-06-29-the-chromium-super-inline-cache-type-confusion/). This is a bug in the v8 that I reported in March 2022. This bug allows RCE in the Chrome renderer sandbox by simply visiting a malicious website.

The exploit is tested with the Linux official build of Chrome version `99.0.4844.84` with the following revision (this can be checked from `chrome://version`):

```
Chromium	99.0.4844.84 (Official Build) (64-bit) 
Revision	81a11fc2ee8a41e17451f29195387f276d3bb379-refs/branch-heads/4844_74@{#6}
```

For reference, the tested binary is compiled with the following flags, following the instructions to compile Chrome [here](https://chromium.googlesource.com/chromium/src/+/main/docs/linux/build_instructions.md):

```
is_debug = false
symbol_level = 2
blink_symbol_level = 2
dcheck_always_on = false
is_official_build = true
chrome_pgo_phase = 0
```

To test, host the file `superic_rce.html` and then open it in Chrome with the `--no-sandbox` flag:

```
./chrome --user-data-dir=/tmp/chromium_data --no-sandbox
```

If successful, it'll pop `xcalc` instantly (on Ubuntu). The exploit should be very reliable and I've not experience any failure with it.
