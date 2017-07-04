#!/bin/bash

autoremoval() {
    while true; do
        sleep 60

        if [[ $(df -h | grep /dev/vda1 | head -1 | awk -F' ' '{ print $5/1 }' | tr ['%'] ["0"]) -gt 90 ]];
        then
                echo "Cleaning disk"
                find /etc/opt/kerberosio/capture/ -type f | sort | head -n 100 | xargs rm;
        fi;
    done
}
autoremoval &

/usr/bin/supervisord -n -c /etc/supervisord.conf
