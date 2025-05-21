#!/usr/bin/env bash

set -euo pipefail
IFS=$'\n\t'

# 检查必要的环境变量
if [[ -z "${PUBLISH_HOSTPORT:-}" ]]; then
	echo "Error: PUBLISH_HOSTPORT environment variable is not set" >&2
	echo "Expected format: host:port" >&2
	exit 1
fi

if [[ -z "${PUBLISH_USERNAME:-}" ]]; then
	echo "Error: PUBLISH_USERNAME environment variable is not set" >&2
	exit 1
fi

if [[ -z "${PUBLISH_PASSWORD:-}" ]]; then
	echo "Error: PUBLISH_PASSWORD environment variable is not set" >&2
	exit 1
fi

# 解析主机和端口
if ! echo "$PUBLISH_HOSTPORT" | grep -q '^[^:]*:[0-9]*$'; then
	echo "Error: Invalid PUBLISH_HOSTPORT format" >&2
	echo "Expected format: host:port" >&2
	echo "Got: $PUBLISH_HOSTPORT" >&2
	exit 1
fi

host=$(echo "$PUBLISH_HOSTPORT" | cut -d ':' -f 1)
port=$(echo "$PUBLISH_HOSTPORT" | cut -d ':' -f 2)

if [[ -z "$host" ]]; then
	echo "Error: Host is empty in PUBLISH_HOSTPORT" >&2
	exit 1
fi

if [[ -z "$port" ]]; then
	echo "Error: Port is empty in PUBLISH_HOSTPORT" >&2
	exit 1
fi

# 设置 SSH 目录
set +e
mkdir -p ~/.ssh
chmod 700 ~/.ssh
touch ~/.ssh/known_hosts
chmod 600 ~/.ssh/known_hosts
set -e

# 添加主机密钥
echo "[$host]:$port ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIDsmJkDd7Rxnuuf4kpbJCDZvkO03lp4lmpzGmFW6LCqG" >> ~/.ssh/known_hosts

# 构建命令
commands="quit"$'\n'
if [[ -z ${PUBLISH_ACCESSCHECK-} ]]; then
	if [[ -z "${PUBLISH_FILENAME:-}" ]]; then
		echo "Error: PUBLISH_FILENAME environment variable is not set" >&2
		exit 1
	fi
	commands="put $PUBLISH_FILENAME"$'\n'"$commands"
	commands="cd ${PUBLISH_DIRECTORY:-.}"$'\n'"$commands"
fi

# 执行 SFTP 命令
echo "Connecting to $host:$port as $PUBLISH_USERNAME..."
SSHPASS="$PUBLISH_PASSWORD" sshpass -e sftp -oPort="$port" -oBatchMode=no -b - "$PUBLISH_USERNAME@$host" <<< "$commands"
