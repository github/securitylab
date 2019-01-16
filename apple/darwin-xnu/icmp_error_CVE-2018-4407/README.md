## Heap buffer overflow in icmp_error (CVE-2018-4407)

Proof-of-concept exploit for a remotely triggerable heap buffer overflow vulnerability in iOS 11.4.1 and macOS 10.13.6. This exploit can be used to crash any vulnerable iOS or macOS device that is connected to the same network as the attacker's computer. The vulnerability can be triggered without any user interaction on the victim's device. The exploit involves sending a TCP packet with non-zero options in the IP and TCP headers. It is possible that some routers or switches will refuse to deliver such packets, but it has worked for me on all the home and office networks that I have tried it on. However, I have found that it is not usually possible to send the malicious packet across the internet. 

For more information about the vulnerability, see the [blog post on lgtm.com](https://lgtm.com/blog/apple_xnu_icmp_error_CVE-2018-4407).

The buffer overflow is in this code [bsd/netinet/ip_icmp.c:339](https://github.com/apple/darwin-xnu/blob/0a798f6738bc1db01281fc08ae024145e84df927/bsd/netinet/ip_icmp.c#L339):

```c
m_copydata(n, 0, icmplen, (caddr_t)&icp->icmp_ip);
```

The exploit sets `icmplen == 120`, which is far too big for the destination buffer. The buffer is overwritten with garbage, so this causes the kernel to crash.

## Usage

The exploit code is designed to be built and run on Linux. The code uses a raw socket to send the malicious packet, because we need to have complete control over the contents of the IP and TCP headers. On Linux, root privileges are required to open a raw socket. Therefore, `sudo` is required to run the PoC. But this is on the attacker's computer, not the victim's, so it does not mitigate the severity of the vulnerability. The code does not do anything malicious to the Linux machine: the root privileges are only used to open a raw socket.

To build the PoC:

```bash
make
```

This builds two versions of the PoC: `direct_attack` and `crash_all`. The former requires you to supply the IP addresses of the target machines, like this:

```bash
sudo ./direct_attack 192.168.0.8 192.168.0.12
```

The latter does not require you to list the IP addresses:

```bash
sudo ./crash_all
```

Use `crash_all` with care: it will crash any unpatched Apple device that is connected to the same network as you.

Alternatively you can give the `CAP_NET_RAW` capability to the binaries instead of using them under full root privileges:

```bash
sudo setcap cap_net_raw=ep crash_all
sudo setcap cap_net_raw=ep direct_attack
```

Some libs are required to use setcap (libcap-ng-utils, libcap-progs, libcap2)
