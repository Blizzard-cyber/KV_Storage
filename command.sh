#!/bin/bash

# 可执行文件的路径
EXECUTABLE="./main"

# 随机生成一个不定长的字符串
generate_random_string() {
    local length=$((RANDOM % 20 + 1)) # 随机生成字符串长度，范围为1到20
    local str=""
    for ((i = 0; i < length; i++)); do
        str+=$(printf "\\$(printf '%03o' $((RANDOM % 26 + 97)))") # 生成随机小写字母
    done
    echo "$str"
}

# 随机生成用户输入的命令
generate_random_command() {
    local commands=("PUT" "GET" "DEL")
    local command=${commands[$RANDOM % ${#commands[@]}]}
    local key=$((RANDOM % 100)) # 随机生成一个0到99999之间的键
    local value=$(generate_random_string) # 生成不定长的字符串

    case $command in
        "PUT")
            echo "$command $key $value"
            ;;
        "GET" | "DEL")
            echo "$command $key"
            ;;
    esac
}

# 将用户输入传递给可执行文件
while true; do
    INPUT=$(generate_random_command)
    echo "$INPUT"
    sleep 0.1 # 模拟用户输入的间隔时间
done | $EXECUTABLE