FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y \
      openjdk-8-jdk git curl zip unzip \
      tmux sudo emacs maven openssh-server net-tools x11-apps

ARG UID=1000

# Create a non-root user account to run Struts.
RUN adduser victim --disabled-password --uid $UID

# Grant the 'victim' user sudo access, so that we can start sshd.
RUN adduser victim sudo
RUN echo "victim:x" | chpasswd

# Switch over to the 'victim' user, since root access is no longer required
USER victim
WORKDIR /home/victim

# Create an ssh authorized keys file. Systems administrators would add their
# public key to this file so that they can login remotely with ssh.
RUN mkdir -m 700 ~/.ssh && touch ~/.ssh/authorized_keys && chmod 600 ~/.ssh/authorized_keys

# Get Struts source code.
RUN git clone https://github.com/apache/struts.git

# Checkout vulnerable version.
RUN cd struts && git branch struts_2_5_16 STRUTS_2_5_16 && git checkout struts_2_5_16

# Remove namespace from configuration file.
COPY struts-actionchaining.xml /home/victim/struts/apps/showcase/src/main/resources/struts-actionchaining.xml

# Build Struts.
RUN cd struts/apps/showcase && mvn clean package -DskipTests

# Get Tomcat.
RUN curl http://mirror.ox.ac.uk/sites/rsync.apache.org/tomcat/tomcat-9/v9.0.12/bin/apache-tomcat-9.0.12.zip -O
RUN unzip apache-tomcat-9.0.12.zip && rm apache-tomcat-9.0.12.zip

# Deploy the webapp.
RUN cp struts/apps/showcase/target/struts2-showcase.war apache-tomcat-9.0.12/webapps/
RUN chmod 755 apache-tomcat-9.0.12/bin/catalina.sh
