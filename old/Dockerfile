# Fill files in algorun_info folder and put your source code in src folder
# Don't change the following three lines
FROM algorun/algorun

RUN apt-get update && \
apt-get install -y wget build-essential cmake && \
apt-get clean && \
rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#RUN wget http://www.cmake.org/files/v2.8/cmake-2.8.3.tar.gz && \
#tar -xf cmake-2.8.3.tar.gz
#RUN cd cmake-2.8.3 && \
#./bootstrap && \
#make -C /cmake-2.8.3/ && \
#make -C /cmake-2.8.3/ install

RUN apt-get update && apt-get install -y graphviz

ADD ./algorun_info /home/algorithm/web/algorun_info/
ADD ./src /home/algorithm/src/
ADD CMakeLists.txt /home/algorithm/

# Install any algorithm dependencies here
RUN cmake /home/algorithm/CMakeLists.txt && \
make -C /home/algorithm/ clean && \
make -C /home/algorithm/ install

RUN apt-get update && \
apt-get install -y python

# [Optional] Sign your image
MAINTAINER Abdelrahman Hosny <abdelrahman.hosny@hotmail.com>