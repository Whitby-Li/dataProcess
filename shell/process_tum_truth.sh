#!/bin/bash
# shellcheck disable=SC2164
cd /datasets/data_process

dataset_folder="/datasets/tum"

find "$dataset_folder" -maxdepth 1 -mindepth 1 -type d -not -name "calib" -print0 | while IFS= read -r -d '' folder; do
    echo "$folder"
    ./bin/process_euroc_truth "$folder"/mocap0/data.csv "$folder"/ground_truth.txt
done