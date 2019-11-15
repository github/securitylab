FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      ansible git curl zip unzip psmisc \
      tmux sudo emacs openssh-server net-tools \
      gcc

# Create a user account named "bofh" (which will have sudo privileges).
RUN adduser bofh --disabled-password

# Grant the 'bofh' user sudo access (only so that we can start sshd).
RUN adduser bofh sudo
RUN echo "bofh:x" | chpasswd
COPY home/ /home/bofh/
RUN chown -R bofh:bofh /home/bofh

# Switch over to the 'bofh' user, since root access is no longer required
USER bofh
WORKDIR /home/bofh
