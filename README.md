# Vector SIL Kit Adapters Common repository
This collection of source code represents parts which are used in several Adapters
of [Vector SIL Kit](https://github.com/vectorgrp/sil-kit/). It can be built using
``cmake`` but is intended to be included in a superproject seamlessly.

# How to use
If you are developping this repository without it being included in a superproject
via git submodule, you can still compile it by following the instructions below:

    git submodule update --init --recursive
    mkdir build
    cmake -S. -Bbuild -DSILKIT_PACKAGE_DIR=/path/to/SilKit-x.y.z-$platform/ -D CMAKE_BUILD_TYPE=Release
    cmake --build build --parallel --config Release

This would check if the library does compile in a closed environment, but does not
produce a library for it.

If you want to clone the standalone version of asio manually, do not use the submodule and instead do:

    git clone --branch asio-1-24-0 https://github.com/chriskohlhoff/asio.git third_party/asio
