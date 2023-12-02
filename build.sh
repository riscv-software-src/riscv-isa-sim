#!/bin/bash



function build_spike() {
  if [ ! -d build ]; then
    mkdir -p build
    cd build
    ../configure --prefix=$RISCV --with-boost=no --with-boost-asio=no --with-boost-regex=no
    cd ..
  fi

  cd build
  make -j32
}

build_spike | tee build.log
