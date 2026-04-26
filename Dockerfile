FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    bash \
    build-essential \
    ca-certificates \
    check \
    gcc \
    git \
    make \
    pkg-config \
    python3 \
    python3-pip \
    python3-venv \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

# Runtime defaults for this project layout.
ENV TREASURE_RUNNER_ASSETS=/workspace/assets
ENV LD_LIBRARY_PATH=/workspace/dist
ENV PYTHONPATH=/workspace/python

CMD ["bash"]
