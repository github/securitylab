FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y curl

# Create user account for the attacker.
RUN adduser attacker --disabled-password

# Switch over to the 'attacker' user, since root access is no longer required
USER attacker
WORKDIR /home/attacker
