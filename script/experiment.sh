#!/bin/bash

#password=ref\!ned
eval $(grep "^username=" script/common.rb)
eval $(grep "^password=" script/common.rb)

for i in 2 1 0;
do
        for j in 2 1 0;
        do
                for k in 1 2 3;
                do
                        while [ ! -z "`sshpass -p $password ssh $username@10.105.3.108 "pgrep broker"`" ]; do
                                sleep 30
                        done;
                        sleep 60
                        if [ -z "`sshpass -p $password ssh $username@10.105.3.108 "pgrep broker"`" ]; then
                                echo ruby script/execute.rb $i $j
                                ruby script/execute.rb $i $j
                        fi
                done;
        done;
done;

