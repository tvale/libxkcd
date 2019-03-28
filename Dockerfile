FROM ubuntu:16.04
RUN apt-get update
RUN apt-get install --yes build-essential
RUN apt-get install --yes cmake
RUN apt-get install --yes libcurl4-openssl-dev
RUN apt-get install --yes libxml2-dev
RUN apt-get install --yes ninja-build
RUN apt-get install --yes python3-pip
RUN apt-get install --yes pkg-config
RUN pip3 install meson
RUN pip3 install gcovr
WORKDIR /libxkcd
COPY . .
RUN meson build && cd build && ninja test
