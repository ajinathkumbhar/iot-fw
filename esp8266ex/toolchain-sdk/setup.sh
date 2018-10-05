
logtag=$(basename $0)
# toolchain 
toolchain_dl_path="https://dl.espressif.com/dl/xtensa-lx106-elf-linux64-1.22.0-88-gde0bdc1-4.8.5.tar.gz"
toolchain_dl_file=$(basename $toolchain_dl_path)
esp_rtos_sdk_dl_path="https://github.com/espressif/ESP8266_RTOS_SDK.git"
esp_rtos_sdk_tag="v3.0-rc1"

# path 
root_dir=$(pwd)


function Slogi() {
    echo "# I $logtag : $1"
}

function Sloge() {
    echo "# E $logtag : $1"
}

function check_error_and_exit() {
    if [ $1 -ne 0 ]; then
        echo "# $1"
        return 1
    fi
    return 0
}


function setup_tool_chain() {
    Slogi " "
    Slogi "toolchain setup start, wait..."
    Slogi " "
    wget $toolchain_dl_path
    ret=$?
    if [ $ret -ne 0 ]; then
        Sloge "Failed to download $toolchain_dl_path"
        return 1
    fi

    tar -xvf $toolchain_dl_file
    ret=$?
    if [ $ret -ne 0 ]; then
        Sloge "Failed to exract $toolchain_dl_file"
        return 1
    fi
    rm -rf $toolchain_dl_file
    Slogi "setup_tool_chain done..."
    Slogi " "
    return 0
}


function setup_esp_rtos_sdk() {
    Slogi " "
    Slogi "ESP8266_RTOS_SDK setup start, wait..."
    Slogi " "
    
    git clone $esp_rtos_sdk_dl_path
    ret=$?
    if [ $ret -ne 0 ]; then
        Sloge  "Failed to git clone ESP8266_RTOS_SDK"
        return 1
    fi

    cd ESP8266_RTOS_SDK
    git checkout tags/$esp_rtos_sdk_tag -b $esp_rtos_sdk_tag
    ret=$?
    if [ $ret -ne 0 ]; then
        Sloge  "Failed to checkout to tag $esp_rtos_sdk_tag"
        return 1
    fi

    cd $root_dir
    Slogi " "
    Slogi "setup_esp_rtos_sdk done..."
    Slogi " "
    return 0
}

function setup_tools() {
    cd $root_dir 
    cd tools
    make
    ret=$?
    if [ $ret -ne 0 ]; then
        Sloge "Failed to compile tools"
    fi
}

#-----------------------
# main start here
#-----------------------
setup_tool_chain
check_error_and_exit $? "Failed to setup_tool_chain"

setup_esp_rtos_sdk 
check_error_and_exit $? "Failed to setup_esp_rtos_sdk"

setup_tools 

Slogi " ---------------------"
Slogi " "
Slogi " Build hello world "
Slogi " "
Slogi " $ source build/envsetup.sh"
Slogi " $ cd ESP8266_RTOS_SDK/examples/get-started/project_template"
Slogi " $ make menuconfig"
Slogi " $ make "
Slogi " "
Slogi " connect ESP8266 board and flash"
Slogi " $ make flash"
Slogi " "
Slogi " The ESP8266 board has default baud rate 74880"
Slogi " So, you need to set baud rate on host machine to get "
Slogi " serial logs from esp board" 
Slogi " Open gtkterm in one terminal"
Slogi " "
Slogi " $ sudo gtkterm"
Slogi " "
Slogi " select /dev/ttyUSB0 and baudrate 115200"
Slogi ""
Slogi " Open another terminal and change baudrate with folloing command"
Slogi " "
Slogi " $ setup-custom-baudrate /dev/ttyUSB0 74880"
Slogi " "
Slogi " -------- Enjoy --------"



