FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    g++ \
    clang \
    libclang-rt-21-dev \
    cmake \
    make \
    ninja-build \
    flex \
    bison \
    gcc-riscv64-linux-gnu \
    g++-riscv64-linux-gnu \
    qemu-system-riscv \
    qemu-user \
    qemu-user-binfmt \
    opensbi \
    u-boot-qemu \
    qemu-efi-riscv64 \
    gdb-multiarch \
    lld \
    llvm \
    git \
    valgrind \
    python3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["/bin/bash"]
