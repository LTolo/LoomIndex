FROM ubuntu:24.04

# Prevent interactive prompts during apt-get install
ENV DEBIAN_FRONTEND=noninteractive

# Update and install build tools (C++20 capable)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    make \
    pkg-config \
    libcurl4-openssl-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code into the image
COPY . /app

# Grant execute rights for the startup script
RUN chmod +x run_project.sh

# Execute script as entrypoint
ENTRYPOINT ["./run_project.sh"]
