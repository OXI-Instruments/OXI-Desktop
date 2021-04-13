#!/bin/bash

if [[ $# < 2  || ! -d ${2} ]]; then
  echo "Usage: ${0} <ouput_file.icns> <input_directory.iconset>"
  exit 255
fi

iconutil --convert icns --output ${1} ${2}
