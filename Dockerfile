# This is a comment
FROM ubuntu:14.04
MAINTAINER Gaetan Collaud <gaetancollaud@gmail.com>
RUN apt-get update
RUN apt-get -y install \
	cmake \
	libqt4-core \
	libqt4-dev \
	libtinyxml-dev \
	libopencv-dev \
	liblog4cxx10-dev \
	libboost-all-dev \
	cmake-curses-gui \
	qtcreator \
	python-dev \
	cxxtest \
	libcurl4-openssl-dev \
	python-opencv \
	python-sklearn \
	python-skimage \
	libav-tools  \
	python-pymongo \
	python-gridfs \
	git


WORKDIR /home/markus
ADD . /home/markus/

RUN cmake .
RUN make clean
RUN make markus
RUN make update_modules_list
RUN make

VOLUME /home/markus/config

RUN ln /dev/null /dev/raw1394
