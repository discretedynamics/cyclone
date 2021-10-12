# Fill files in algorun_info folder and put your source code in src folder
# Don't change the following three lines
FROM algorun/algorun:20.04

RUN apt-get update && \
DEBIAN_FRONTEND="noninteractive" TZ="America/New_York" apt-get install -y tzdata && \
apt-get install -y wget build-essential cmake && \
apt-get clean && \
rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN apt-get update && apt-get install -y graphviz git

ADD ./algorun_info /home/algorithm/web/algorun_info/
ADD ./src /home/algorithm/src/
COPY ./CMakeLists.txt /home/algorithm/

# Install any algorithm dependencies here
RUN mkdir /home/algorithm/build && \
cd /home/algorithm/build && \
cmake -DCMAKE_BUILD_TYPE=Release ../ && \
make && \
mv /home/algorithm/build/simFDS /usr/bin/ && \
mv /home/algorithm/src/runner.sh /usr/bin/

#RUN apt-get update && \
#apt-get install -y python

# ACK: maintainer is deprecated, see https://docs.docker.com/engine/reference/builder/
# [Optional] Sign your image
MAINTAINER Abdelrahman Hosny <abdelrahman.hosny@hotmail.com>
