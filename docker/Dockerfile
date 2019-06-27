FROM ubuntu:18.04

RUN apt-get update && apt-get install -y \
	openssh-client \
	git \
	g++ \
	device-tree-compiler \
	make \
	wget \
	texinfo \
	bison \
	flex \
	cmake \
	python3 \
	python3-pip

RUN pip3 install cffi

RUN apt-get clean
