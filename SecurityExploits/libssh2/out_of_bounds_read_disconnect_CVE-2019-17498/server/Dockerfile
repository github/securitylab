FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      sudo tmux screen emacs git gdb net-tools psmisc \
      build-essential autoconf g++ netcat

ARG UID=1000

# Create a non-root user account.
RUN adduser hal --disabled-password --uid $UID

# Grant the 'hal' user sudo access. This is not used for the demo,
# but it is often handy for installing extra packages.
RUN adduser hal sudo
RUN echo "hal:x" | chpasswd
COPY home/ /home/hal/
RUN chown -R hal:hal /home/hal

# Switch over to the 'hal' user, since root access is no longer required
USER hal
WORKDIR /home/hal
