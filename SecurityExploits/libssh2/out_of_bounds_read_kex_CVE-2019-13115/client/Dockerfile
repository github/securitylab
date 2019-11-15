FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      sudo tmux screen emacs git gdb net-tools psmisc \
      build-essential autoconf automake libtool g++ \
      libssl-dev

ARG UID=1000

# Create a non-root user account to run Libssh2.
RUN adduser victim --disabled-password --uid $UID

# Grant the 'victim' user sudo access. This is not used for the
# demo, but it is often handy for installing extra packages.
RUN adduser victim sudo
RUN echo "victim:x" | chpasswd
COPY home/ /home/victim/
RUN chown -R victim:victim /home/victim

# Switch over to the 'victim' user, since root access is no longer required
USER victim
WORKDIR /home/victim

# Checkout and build libssh2-1.8.2 (commit 02ecf17a6d5f9837699e8fb3aad0c804caa67eeb).
# Note: this PoC also works on commit 38bf7ce9ece3441dcf3a19f0befb5b491ed4adfa,
# which is the commit which contained the bad implementation of _libssh2_check_length.
RUN git clone https://github.com/libssh2/libssh2.git && \
    cd libssh2 && \
    git checkout 02ecf17a6d5f9837699e8fb3aad0c804caa67eeb && \
    ./buildconf && \
    ./configure && \
    make -j4
