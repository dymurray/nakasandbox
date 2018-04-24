FROM centos

WORKDIR /home/nakamoto

RUN yum update  -y \
    && yum install -y epel-release
RUN yum -y install cryptopp-devel cryptopp keyutils-libs git which

RUN git clone https://github.com/boostorg/boost --recursive 
WORKDIR /home/nakamoto/boost

RUN yum -y install gcc gcc-c++ boost boost-devel

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

ADD gcc-6.2.0.tar.gz /home/nakamoto
WORKDIR /home/nakamoto/build
RUN ../gcc-6.2.0/configure
RUN yum -y install file java-devel zip
RUN make
RUN make install
#RUN ./configure --with-system-zlib --disable-multilib --enable-languages=c,c++
#RUN yum -y install zlib-devel
#RUN make -j 8
#RUN make install
#WORKDIR /home/nakamoto/testapp

COPY include/ /home/nakamoto/include
COPY lib/ /home/nakamoto/lib
COPY . /home/nakamoto/naka/

