FROM phusion/baseimage:latest

MAINTAINER "Cédric Verstraeten" <hello@cedric.ws>

#################################
# Surpress Upstart errors/warning

RUN dpkg-divert --local --rename --add /sbin/initctl
RUN ln -sf /bin/true /sbin/initctl

#############################################
# Let the container know that there is no tty

ENV DEBIAN_FRONTEND noninteractive

#########################################
# Update base image
# Add sources for latest nginx and cmake
# Install software requirements

RUN sudo sed -i -e 's/us.archive.ubuntu.com/archive.ubuntu.com/g' /etc/apt/sources.list
RUN apt-get update && \
apt-get install -y software-properties-common && \
nginx=stable && \
apt-get update && \
apt-get upgrade -y
RUN apt-get -y install git supervisor curl subversion libcurl4-gnutls-dev cmake dh-autoreconf libav-tools autotools-dev autoconf automake gcc build-essential libtool make nasm zlib1g-dev tar libx264.

#################################
# Kerberos.io version

ENV CHECKOUT master

############################
# Clone and build machinery

RUN git clone https://github.com/FFmpeg/FFmpeg && \
	cd FFmpeg && git checkout remotes/origin/release/2.8 && \
	./configure --enable-gpl --enable-libx264 && make && \
    make install && \
    cd .. && rm -rf FFmpeg
    
RUN	git clone https://github.com/kerberos-io/machinery /tmp/machinery && \
    cd /tmp/machinery && git checkout develop && \
    mkdir build && cd build && \
    cmake .. && make && make check && make install && \
    rm -rf /tmp/machinery && \
    chown -Rf www-data.www-data /etc/opt/kerberosio && \
    chmod -Rf 777 /etc/opt/kerberosio/config

#############################################
# Make config and capture directories visible

VOLUME ["/etc/opt/kerberosio/config"]
VOLUME ["/etc/opt/kerberosio/capture"]

##############
# Expose Ports

EXPOSE 8888

#############################
# Supervisor Config and start

ADD ./supervisord.conf /etc/supervisord.conf
ADD ./run.sh /run.sh
RUN chmod 755 /run.sh

CMD ["/bin/bash", "/run.sh"]
