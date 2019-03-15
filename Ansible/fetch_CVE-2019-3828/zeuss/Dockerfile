FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      git curl zip unzip psmisc \
      tmux sudo emacs openssh-server net-tools x11-apps \
      build-essential libssl-dev libffi-dev python-dev \
      python-setuptools python-jinja2 python-yaml

# Create a non-root user account to run Ansible.
RUN adduser bofh --disabled-password

# Grant the 'bofh' user sudo access, so that we can start sshd.
RUN adduser bofh sudo
RUN echo "bofh:x" | chpasswd
COPY home/ /home/bofh/
RUN chown -R bofh:bofh /home/bofh

# Switch over to the 'bofh' user, since root access is no longer required
USER bofh
WORKDIR /home/bofh

# Get vulnerable version of Ansible source code.
RUN git clone https://github.com/ansible/ansible.git && \
    cd ansible && \
    git checkout f9f7b29a5a5543e8d1c25e8cc1f2d3040d8536b7
