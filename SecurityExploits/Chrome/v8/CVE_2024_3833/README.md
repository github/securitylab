## V8 type confusion CVE-2024-3833

The analysis of this bug can be found [here](https://github.blog/2024-06-26-attack-of-the-clones-getting-rce-in-chromes-renderer-with-duplicate-object-properties). 

The exploit here is tested on the official build of Chrome version 123.0.6312.58, on Ubuntu 22.04. The following build config was used to build Chromium:

```
is_debug = false
symbol_level = 1
blink_symbol_level = 1
dcheck_always_on = false
is_official_build = true
chrome_pgo_phase = 0
v8_symbol_level = 1
```

The bug depends on an origin trial and to emulate it locally, the patch `trial-token.patch` should be applied before building Chrome.

If successful, on Ubuntu 22.04, it should call launch `xcalc` when `wasm_poc.html` is opened in Chrome.

Shell code and some addresses may need changing on other platforms.


