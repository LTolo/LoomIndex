FROM ubuntu:24.04

# Verhindere interaktive Prompts während apt-get install
ENV DEBIAN_FRONTEND=noninteractive

# Update und Installation der Build-Tools (C++20 fähig)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    make \
    pkg-config \
    libcurl4-openssl-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Arbeitsverzeichnis festlegen
WORKDIR /app

# Quellcode ins Image kopieren
COPY . /app

# Execute-Rechte für das Start-Skript vergeben
RUN chmod +x run_project.sh

# Skript als Entrypoint ausführen
ENTRYPOINT ["./run_project.sh"]
