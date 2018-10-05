# Env setup 

root_dir=$(pwd)
tool_chain_bin="$root_dir/xtensa-lx106-elf/bin"
tools_bin="$root_dir/tools/bin"
esp8266_rtos_sdk="$root_dir/ESP8266_RTOS_SDK"

export PATH="${PATH}:$tool_chain_bin:$tools_bin"
export IDF_PATH=$esp8266_rtos_sdk
