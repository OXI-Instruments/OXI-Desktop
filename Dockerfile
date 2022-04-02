FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone && \
    apt-get update -y && \
    apt-get install \
        git \
        cmake \
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
        libxkbcommon-dev -y && \
    apt-get clean && rm -rf /var/lib/apt/lists/

RUN pip3 install -U pip && \
    pip3 install aqtinstall

ARG QT=6.2.4
ARG QT_MODULES=
ARG QT_HOST=linux
ARG QT_TARGET=desktop
ARG QT_ARCH=
RUN aqt install-qt --outputdir /opt/qt ${QT_HOST} ${QT_TARGET} ${QT} ${QT_ARCH} -m ${QT_MODULES}

ENV PATH /opt/qt/${QT}/gcc_64/bin:$PATH
ENV QT_PLUGIN_PATH /opt/qt/${QT}/gcc_64/plugins/
ENV QML_IMPORT_PATH /opt/qt/${QT}/gcc_64/qml/
ENV QML2_IMPORT_PATH /opt/qt/${QT}/gcc_64/qml/
