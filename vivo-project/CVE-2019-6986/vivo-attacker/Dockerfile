FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y curl tmux emacs net-tools ssh sudo

# Create user account for the attacker.
RUN adduser attacker --disabled-password

# Copy the exploit PoC into the attacker's home directory.
COPY post.sh /home/attacker/post.sh
RUN chown attacker:attacker /home/attacker/post.sh

# Switch over to the 'attacker' user, since root access is no longer required
USER attacker
WORKDIR /home/attacker
