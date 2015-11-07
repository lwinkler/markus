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
	libjsoncpp-dev \
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

WORKDIR /home
RUN git clone https://github.com/gaetancollaud/markus.git
WORKDIR /home/markus
RUN cmake .
RUN make clean
RUN make markus


#RUN apt-get install -y x11vnc xvfb
#RUN mkdir ~/.vnc
#RUN x11vnc -storepasswd 1234 ~/.vnc/passwd

#run bash -c 'echo "cd /home/markus && make update_modules_list" > ~/.bash_markus && chmod +x ~/.bash_markus'
#run bash -c 'echo "~/.bash_markus" >> ~/.bashrc'
#RUN x11vnc -forever -usepw -create &

RUN make update_modules_list 
RUN make
