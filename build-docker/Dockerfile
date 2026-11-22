FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    qt6-base-dev \
    libqt6sql6-mysql \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

RUN cmake -G "Ninja" -S /app/Design -B /app/Design/build-docker && \
    cmake --build /app/Design/build-docker

CMD ["/app/Design/build-docker/LibraryManagementApp"]