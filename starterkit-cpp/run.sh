#!/bin/sh

cd build
./main "$@" || echo "run error code: $?"
