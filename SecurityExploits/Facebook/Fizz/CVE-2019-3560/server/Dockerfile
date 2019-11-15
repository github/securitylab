FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      sudo tmux screen emacs git gdb net-tools g++ cmake \
      libboost-all-dev libevent-dev libdouble-conversion-dev \
      libgoogle-glog-dev libgflags-dev libiberty-dev liblz4-dev \
      liblzma-dev libsnappy-dev make zlib1g-dev binutils-dev \
      libjemalloc-dev libssl-dev pkg-config libsodium-dev

ARG UID=1000

# Create a non-root user account to run Fizz.
RUN adduser victim --disabled-password --uid $UID

# Grant the 'victim' user sudo access. This is not used for the demo,
# but it is often handy for installing extra packages.
RUN adduser victim sudo
RUN echo "victim:x" | chpasswd
COPY home/ /home/victim/
RUN chown -R victim:victim /home/victim

# Switch over to the 'victim' user, since root access is no longer required
USER victim
WORKDIR /home/victim

# Clone and build Folly, which Fizz depends on.
RUN git clone https://github.com/facebook/folly && \
    cd folly && \
    git checkout df5a0575d95f3c2cc9200b15e40db4af82e1f2eb && \
    mkdir build_ && cd build_ && \
    cmake .. && \
    make -j $(nproc)

# Install Folly.
USER root
RUN cd /home/victim/folly/build_ && make install
USER victim

# Clone and build Fizz.
RUN git clone https://github.com/facebookincubator/fizz && \
    cd fizz && \
    git checkout eaa81af854bef509c3c1d7c83df0cd0b084a0fef && \
    mkdir build_ && cd build_ && \
    cmake ../fizz && \
    make -j $(nproc)

# Install Fizz.
USER root
RUN cd /home/victim/fizz/build_ && make install
USER victim
