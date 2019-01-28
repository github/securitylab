FROM ubuntu:bionic

RUN echo "Etc/UTC" > /etc/timezone
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y git python-pip postgresql redis-server gunicorn sudo

ARG UID=1000

# Create a non-root user account to run Refinery.
RUN adduser victim --disabled-password --uid $UID

# Grant the 'victim' user sudo access, so that we can start postgresql.
RUN adduser victim sudo
RUN echo "victim:x" | chpasswd

# Switch over to the 'victim' user, since root access is no longer required
USER victim
WORKDIR /home/victim

# Some of the Python module names have changed since the Refinery code
# was written, so we have to apply a simple patch.
COPY diff.txt /home/victim/diff.txt

# Get Refinery source code and check out the vulnerable version.
RUN git clone https://github.com/daeilkim/refinery && cd refinery && \
    git checkout 0d5de8fc3d680a2c79bd0e9384b506229787c74f && \
    git apply /home/victim/diff.txt

RUN pip install flask flask_login flask_sqlalchemy flask_wtf celery joblib psycopg2 redis scipy
