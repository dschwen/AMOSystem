# syntax=docker/dockerfile:1
# Versioned OSXCross 14.5-r0 toolchain, pinned by multi-platform manifest.
FROM --platform=$BUILDPLATFORM ghcr.io/crazy-max/osxcross:14.5-r0-ubuntu@sha256:d9f2e0694b9420a0bd3e89272c1266ac7cc3b3d287c43297d6f049f2d30de3d8 AS osxcross
FROM ubuntu:24.04@sha256:fe81560990fae86dc4743202b2cb4d3df98d0bcf949173cecef15ce1127ef852
RUN apt-get update && apt-get install -y --no-install-recommends \
    clang lld llvm libc6-dev make python3 git file ca-certificates xz-utils \
    libxml2 libssl3 libtinfo6 libuuid1 zlib1g && rm -rf /var/lib/apt/lists/*
COPY --from=osxcross /osxcross /osxcross
ENV PATH="/osxcross/bin:$PATH" LD_LIBRARY_PATH="/osxcross/lib"
WORKDIR /src
