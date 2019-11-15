FROM ubuntu:artful

RUN apt-get update && \
    apt-get install -y \
      sudo tmux screen emacs git gdb net-tools \
      python python3 build-essential gcc \
      cmake bison flex libprotobuf-c-dev libreadline-dev libsqlite3-dev \
      libssl-dev libunwind-dev libz1 libz-dev make gawk protobuf-c-compiler \
      uuid-dev liblz4-tool liblz4-dev libprotobuf-c1 libsqlite3-0 \
      libuuid1 libz1 tzdata ncurses-dev tcl bc dh-autoreconf pkg-config \
      libgnutls28-dev libcurl4-gnutls-dev python-docutils libgcrypt20-dev \
      iproute2 nmap gnutls-bin

RUN mkdir /opt/work
COPY build-all.sh /opt/work/
COPY benevolent/ /opt/work/benevolent/
COPY malicious/ /opt/work/malicious/
WORKDIR /opt/work

RUN git clone https://github.com/rsyslog/libee.git
RUN git clone https://github.com/rsyslog/libestr.git
RUN git clone https://github.com/rsyslog/libfastjson.git
RUN git clone https://github.com/rsyslog/liblogging.git
RUN git clone https://github.com/rsyslog/librelp.git
RUN git clone https://github.com/rsyslog/rsyslog.git

# Checkout versions with the bug
WORKDIR /opt/work/libee
RUN git checkout 1569d91bf33101f012cfc5b25beea68f2c6e25f2
WORKDIR /opt/work/libestr
RUN git checkout 75ea6e3b5a2187dbe48e7f5cec82311ca3a09c22
WORKDIR /opt/work/libfastjson
RUN git checkout v0.99.8
WORKDIR /opt/work/liblogging
RUN git checkout 5602f9dacbfc8e1912aa25f9e27be6aac13ac4ce
WORKDIR /opt/work/librelp
RUN git checkout v1.2.14
WORKDIR /opt/work/rsyslog
RUN git checkout v8.33.1

WORKDIR /opt/work
RUN ./build-all.sh

# switch over to the 'semmle_build' user, since root access is no longer required
RUN addgroup semmle_build --gid 1001
RUN adduser semmle_build --disabled-password --uid 1001 --gid 1001
RUN adduser semmle_build sudo
RUN echo "semmle_build:x" | chpasswd
RUN chown -R semmle_build:semmle_build /opt/work

USER semmle_build
ENV HOME /opt/work
WORKDIR /opt/work/benevolent/certs
RUN pwd 1>&2
RUN ls -al 1>&2
RUN ./create-certs.sh
WORKDIR /opt/work/malicious/kevcertz
RUN pwd
RUN ./create-certz.sh
WORKDIR /opt/work/
