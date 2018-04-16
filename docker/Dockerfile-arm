FROM kerberos/base:linux-arm

ARG APP_ENV=master
ENV APP_ENV ${APP_ENV}

MAINTAINER "Cédric Verstraeten" <hello@cedric.ws>

#################################
# Surpress Upstart errors/warning

RUN dpkg-divert --local --rename --add /sbin/initctl
RUN ln -sf /bin/true /sbin/initctl

#############################################
# Let the container know that there is no tty

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y libcurl4-openssl-dev

############################
# Clone and build machinery

RUN apt-get install -y pkg-config
RUN export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/
RUN	git clone https://github.com/kerberos-io/machinery /tmp/machinery && \
    cd /tmp/machinery && git checkout ${APP_ENV} && \
    mkdir build && cd build && \
    cmake .. && make && make install

RUN cd /tmp/machinery && \
  make check

RUN rm -rf /tmp/machinery && \
    chown -Rf www-data.www-data /etc/opt/kerberosio && \
    chmod -Rf 777 /etc/opt/kerberosio/config

#############################################
# Make config and capture directories visible

VOLUME ["/etc/opt/kerberosio/config"]
VOLUME ["/etc/opt/kerberosio/capture"]
VOLUME ["/etc/opt/kerberosio/logs"]

##############
# Expose Ports

EXPOSE 8889

#############################
# Supervisor Config and start

ADD ./supervisord.conf /etc/supervisord.conf
ADD ./run.sh /run.sh
RUN chmod 755 /run.sh

CMD ["/bin/bash", "/run.sh"]
