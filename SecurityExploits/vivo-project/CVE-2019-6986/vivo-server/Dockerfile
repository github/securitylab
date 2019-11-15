FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      maven git curl wget zip unzip mysql-server \
      tmux sudo emacs maven openssh-server net-tools x11-apps \
      default-jdk openjdk-11-dbg

# Workaround for https://serverfault.com/questions/870568
# VOLUME /var/lib/mysql

ARG UID=1000

# Create a non-root user account to run Struts.
RUN adduser vivo --disabled-password --uid $UID

# Grant the 'vivo' user sudo access, so that we can start sshd.
RUN adduser vivo sudo
RUN echo "vivo:x" | chpasswd

# Get Tomcat.
RUN cd /tmp && \
    wget http://www-eu.apache.org/dist/tomcat/tomcat-9/v9.0.14/bin/apache-tomcat-9.0.14.tar.gz && \
    tar xf apache-tomcat-9.0.14.tar.gz && \
    mv apache-tomcat-9.0.14 /usr/local/tomcat && \
    rm apache-tomcat-9.0.14.tar.gz

COPY init_mysql.sh /home/vivo/init_mysql.sh
RUN chown vivo:vivo /home/vivo/init_mysql.sh

RUN mkdir -p /usr/local/vivo/home
RUN chown -R vivo:vivo /usr/local/tomcat
RUN chown -R vivo:vivo /usr/local/vivo

# Switch over to the 'vivo' user, since root access is no longer required
USER vivo
WORKDIR /home/vivo

# Create an ssh authorized keys file. Systems administrators would add their
# public key to this file so that they can login remotely with ssh.
RUN mkdir -m 700 ~/.ssh && touch ~/.ssh/authorized_keys && chmod 600 ~/.ssh/authorized_keys

# Get Vivo source code.
RUN git clone https://github.com/vivo-project/Vitro.git Vitro
RUN cd Vitro && git checkout 6e717446b4a1b3da0fcf0130f3d0cfd1ce8b75ed
RUN git clone https://github.com/vivo-project/VIVO.git VIVO
RUN cd VIVO && git checkout 3da53e27fe1020ffd3157d288f6fe39ec15f87b2

# Build Vivo.
RUN cd VIVO && mvn install -s installer/example-settings.xml

RUN cd /usr/local/vivo/home/config && \
    cp example.runtime.properties runtime.properties && \
    cp example.applicationSetup.n3 applicationSetup.n3
