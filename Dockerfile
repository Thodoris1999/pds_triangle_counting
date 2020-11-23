# This file builds the Docker image for the CSAL Unified Development Enviroment

# Use Official Ubuntu base image
FROM ubuntu:18.04

# Set some variables
ENV USER thodoris
ENV HOME /home/$USER
ENV DIR parallel_ws

# Avoid stall during installation of tzdata
ENV TZ=Europe/Athens
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install basic dependencies
RUN apt-get update && apt-get -y install \
    locales \
    sudo \
    build-essential \
    openssh-server \
    && rm -rf /var/lib/apt/lists/*

# Install libraries usef
RUN apt-get update && apt-get install -y \
     gcc \
#    nlohmann-json-dev \
#    libshp-dev \
#    liblas-dev \   #need to build from source to support LASZip
#    liblas-c-dev \
#    libpugixml-dev \
#    libproj-dev \
#    libtriangle-dev \
#    libnetcdf-c++4-dev \
    clang-format \
    clang-tidy \
    ca-certificates \
    automake\
    colordiff\
    openmpi-bin\ 
    libopenmpi-dev \
    && rm -rf /var/lib/apt/lists/*

# Install git with lfs support and other packages needed
RUN apt-get update && apt-get install -y \
    git \
    git-lfs \
    dos2unix \
    vim \
    rsync \
    wget \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Get and extract OpenCilk
RUN wget https://github.com/OpenCilk/opencilk-project/releases/download/opencilk%2Fbeta3/OpenCilk-9.0.1-Linux.tar.gz && tar xvzf OpenCilk-9.0.1-Linux.tar.gz 
RUN mv OpenCilk-9.0.1-Linux/ /usr/local/ && chmod og+xr /usr/local/OpenCilk-9.0.1-Linux/ -R

# Create shared volume
VOLUME $HOME/$DIR
WORKDIR $HOME/$DIR

# Generate welcome message printed at login
COPY Welcome $HOME/.welcome
COPY src $HOME/$DIR
COPY Makefile $HOME/$DIR
RUN echo "cat $HOME/.welcome" >> $HOME/.bashrc

# Start bash login shell
ENTRYPOINT ["/bin/bash", "-l", "-c"]
CMD ["/bin/bash", "-i"]
