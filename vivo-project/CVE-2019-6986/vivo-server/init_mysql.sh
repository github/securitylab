#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root"
   exit 1
fi

# Workaround for this issue: https://serverfault.com/questions/870568
chown -R mysql:mysql /var/lib/mysql

service mysql start

Commands=$(cat <<EOF
CREATE DATABASE vitrodb CHARACTER SET utf8;
CREATE USER 'vitrodbUsername'@'localhost' IDENTIFIED BY 'vitrodbPassword';
GRANT ALL PRIVILEGES ON vitrodb.* TO 'vitrodbUsername'@'localhost';
EOF
)

echo "$Commands" | mysql -p
