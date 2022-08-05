FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && \
    apt-get update -y && \
    apt-get install -y \
        git \
        cmake \
        curl \
        file \
        pkg-config \
        python3 \
        python3-pip \
        build-essential \
        libdbus-1-3 \
        libpulse-mainloop-glib0 \
        libjack-dev \
        libasound2-dev \
        libgl1-mesa-dev \
        libfreetype6-dev \
        libfontconfig1 \
        libxkbcommon-dev \
        ninja-build \
        patchelf \
        qtbase5-dev \
        qtbase5-dev-tools \
        qtcreator && \
    apt-get clean && rm -rf /var/lib/apt/lists/
