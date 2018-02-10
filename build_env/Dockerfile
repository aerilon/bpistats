FROM ubuntu:17.10

RUN export DEBIAN_FRONTEND=noninteractive && \
	apt-get update && \
	apt-get -y install \
		bzip2 \
		cmake \
		git \
		gcc \
		g++ \
		libssl-dev \
		tar \
		wget \
		zlib1g-dev

RUN cd /root && \
	wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.bz2 && \
	tar xjf boost_1_66_0.tar.bz2 && \
	cd boost_1_66_0 && \
	./bootstrap.sh && \
	./b2 --prefix=$HOME/opt/boost -j 8 install

