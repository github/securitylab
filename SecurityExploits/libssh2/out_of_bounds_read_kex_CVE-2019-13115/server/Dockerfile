FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      sudo tmux screen emacs git gdb net-tools psmisc \
      build-essential autoconf g++ \
      libssl-dev zlib1g-dev

ARG UID=1000

# Create a non-root user account to build openssh-server.
RUN adduser hal --disabled-password --uid $UID

# Grant the 'hal' user sudo access. This is not used for the demo,
# but it is often handy for installing extra packages.
RUN adduser hal sudo
RUN echo "hal:x" | chpasswd
COPY home/ /home/hal/
RUN chown -R hal:hal /home/hal

# Create the sshd user
RUN mkdir /var/empty && \
    chown root:sys /var/empty && \
    chmod 755 /var/empty && \
    groupadd sshd && \
    useradd -g sshd -c 'sshd privsep' -d /var/empty -s /bin/false sshd

# Switch over to the 'hal' user, since root access is no longer required
USER hal
WORKDIR /home/hal

# Clone openssh, insert some malicious code, and built it.
RUN git clone https://github.com/openssh/openssh-portable.git && \
    cd openssh-portable && \
    git checkout 21da87f439b48a85b951ef1518fe85ac0273e719 && \
    git apply /home/hal/diff.txt && \
    autoreconf && \
    ./configure && \
    make

# Install openssh.
USER root
RUN cd /home/hal/openssh-portable && make install
USER hal
