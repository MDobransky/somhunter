#!/bin/bash

docker build -t somhunter:base -f base .
docker run -ti --rm -p8080:8080 -v ~/somhunter:/host somhunter:base
