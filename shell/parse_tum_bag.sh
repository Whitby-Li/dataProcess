#!/bin/bash

# shellcheck disable=SC2164
cd /datasets/data_process

data_directory="/datasets/tum"

for file in "$data_directory"/*/*.bag; do
  ./bin/parse_tum_bag $file
done