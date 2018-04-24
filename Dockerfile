FROM centos/devtoolset-7-toolchain-centos7
USER 0
WORKDIR /home/nakamoto

RUN yum update  -y \
    && yum install -y epel-release
RUN yum -y install cryptopp-devel cryptopp keyutils-libs git which

RUN git clone https://github.com/boostorg/boost --recursive 
WORKDIR /home/nakamoto/boost

RUN yum -y install boost boost-devel

RUN ./bootstrap.sh --prefix=/opt/boost

RUN yum -y install python python-devel

RUN ./b2 install --prefix=/opt/boost --with=all

COPY include/  /usr/include/
COPY lib/ /usr/lib/

WORKDIR /home/nakamoto
COPY examples/c++/testapp/ /home/nakamoto/testapp
#WORKDIR /home/nakamoto/testapp
RUN yum -y install make
RUN yum -y install gmp-devel mpfr-devel libmpc-devel glibc-devel.i686 libgcc.i686 gcc-c++
WORKDIR /home/nakamoto/testapp
