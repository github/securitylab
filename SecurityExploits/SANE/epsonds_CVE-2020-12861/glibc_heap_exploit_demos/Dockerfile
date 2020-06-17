FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      build-essential gdb \
      tmux sudo emacs

ARG UID=1000

# Create a non-root user account.
RUN adduser demo --disabled-password --uid $UID

# Grant the 'demo' user sudo access. This is not used for the demo,
# but it is often handy for installing extra packages.
RUN adduser demo sudo
RUN echo "demo:x" | chpasswd
COPY home/ /home/demo/
RUN chown -R demo:demo /home/demo

# Switch over to the 'demo' user, since root access is no longer required
USER demo
WORKDIR /home/demo
