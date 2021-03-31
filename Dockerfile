FROM ubuntu:focal
ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt update && apt install -y --no-install-recommends gcc g++ make cmake \
    libxcb-shape0-dev libxcb-xfixes0-dev libx11-xcb-dev \
    libglib2.0-dev \
    qt5-default qttools5-dev libqt5xdg-dev \
    libkf5windowsystem-dev libqt5svg5-dev \
    libqt5x11extras5-dev
WORKDIR /src
RUN ls /usr/include/qt5xdg
COPY ./ ./
RUN mkdir build && cd build && cmake -DSG_DBUS_NOTIFY=ON ../ && make VERBOSE=1 && make install
RUN strip --strip-unneeded /usr/local/bin/screengrab
ENTRYPOINT /usr/local/bin/screengrab
