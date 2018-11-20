FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y curl tmux emacs net-tools gcc ssh build-essential

# Create user account for the attacker.
RUN adduser attacker --disabled-password

# Copy the exploit PoC into the attacker's home directory.
COPY src /home/attacker/src
RUN chown -R attacker:attacker /home/attacker/src

# Switch over to the 'attacker' user, since root access is no longer required
USER attacker
WORKDIR /home/attacker
RUN cd src && make

# Create an ssh key for the attacker.
RUN ssh-keygen -t ed25519 -f ~/.ssh/id_ed25519 -q -P ""
