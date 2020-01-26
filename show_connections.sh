#!/bin/bash
for PORT in $(eval echo {$2..$3}); do
  	timeout 0.2 bash -c "</dev/tcp/$1/$PORT &>/dev/null" &&  echo -e "port $PORT is open\n"
done
echo -e "\n"