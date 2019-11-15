FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y git gcc build-essential curl

# Create user account for the attacker.
RUN adduser semmle --disabled-password

# Copy the exploit PoC into the user's home directory.
COPY poc.c /home/semmle/poc.c
RUN chown -R semmle:semmle /home/semmle/

# Switch over to the 'semmle' user, since root access is no longer required
USER semmle
WORKDIR /home/semmle
RUN git clone https://framagit.org/dtschump/CImg.git
RUN cd CImg && git checkout 5bb8a03d7fed06275ddb53a56c567fb6f61aa4a4
