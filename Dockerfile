FROM centos/devtoolset-7-toolchain-centos7
USER 0
WORKDIR /home/nakamoto
ENV USER_NAME=nakasendo \
    USER_UID=1001 \
    BASE_DIR=/home/nakasendo
ENV HOME=${BASE_DIR}

RUN yum update  -y \
    && yum install -y epel-release cryptopp-devel cryptopp keyutils-libs git which boost boost-devel python python-devel make gmp-devel mpfr-devel libmpc-devel glibc-devel.i686 libcc.i686 gcc-c++

RUN git clone https://github.com/boostorg/boost --recursive 
WORKDIR /home/nakamoto/boost

RUN ./bootstrap.sh --prefix=/opt/boost

RUN ./b2 install --prefix=/opt/boost --with=all

COPY include/  /usr/include/
COPY lib/ /usr/lib/
RUN yum -y install cryptopp cryptopp-devel

#RUN mkdir -p ${BASE_DIR} \
#             && useradd -u 1002 -r -g 0 -M -d ${BASE_DIR} -b ${BASE_DIR} -s /sbin/nologin -c "nakasendo user" ${USER_NAME}
WORKDIR /home/nakasendo
COPY examples /home/nakasendo
USER 1000
