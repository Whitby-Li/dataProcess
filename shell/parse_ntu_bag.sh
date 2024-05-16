#!/bin/bash

# shellcheck disable=SC2164
cd /datasets/data_process

data_directory="/datasets/ntu_viral"

for file in "$data_directory"/*/*.bag; do
  ./bin/parse_ntu_bag $file
done