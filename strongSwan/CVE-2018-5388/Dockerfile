FROM ubuntu:artful

RUN apt-get update && \
    apt-get install -y \
      openjdk-8-jdk git-core gnupg flex bison gperf build-essential \
      zip curl zlib1g-dev gcc-multilib g++-multilib libc6-dev-i386 \
      lib32ncurses5-dev x11proto-core-dev libx11-dev lib32z-dev ccache \
      libgl1-mesa-dev libxml2-utils xsltproc unzip python gdb python3 \
      tmux screen pkg-config libtool automake sudo libgmp-dev iptables \
      xl2tpd module-init-tools supervisor emacs gettext libcap-dev

# Create a vpn group.
RUN groupadd vpn
RUN useradd -g vpn vpn

WORKDIR /opt/work
RUN git clone git://git.strongswan.org/strongswan.git
RUN cd strongswan && git checkout 5.6.2 && ./autogen.sh && \
    ./configure --with-capabilities=libcap --with-user=vpn --with-group=vpn && \
    make && make install

# Create an 'attacker' user. This user will be a member of the vpn
# group, but does not get superuser privileges.
RUN adduser attacker
RUN adduser attacker vpn

# Switch to the attacker user and create the exploit code.
USER attacker
WORKDIR /home/attacker/

# Get a copy of the strongswan codebase for the "attacker" user. This
# is just a lazy way to write the code for the exploit. The only thing
# that we will use from this copy of the code is the "stroke" utility.
# We will modify the code slightly and use stroke to send a malicious
# message to the charon daemon.
RUN git clone git://git.strongswan.org/strongswan.git
COPY stroke_patch.txt /home/attacker/stroke_patch.txt
RUN cd strongswan && git checkout 5.6.2 && \
    git apply ../stroke_patch.txt && \
    ./autogen.sh && ./configure && make

# Switch back to the root user so that we can start ipsec when we start
# the container.
USER root
