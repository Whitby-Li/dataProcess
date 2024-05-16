#!/bin/bash

# shellcheck disable=SC2164
cd /datasets/data_process

data_directory="/datasets/euroc"

for file in "$data_directory"/*/*.bag; do
  ./bin/parse_euroc_bag $file
done