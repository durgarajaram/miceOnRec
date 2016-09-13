#!/bin/bash

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.


max_mem_usage=16000

dt=$(date '+%d/%m/%Y %H:%M:%S');
echo "$dt"

echo
echo
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "++  This script starts the MICE Online reconstruction as a background process. ++"
echo "++  It also checks periodically if the Online reconstruction is still running. ++"
echo "++                    Started:" $dt "                            ++"
echo "++                    PID:" $$ "                                              ++"
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo
echo

# Check if the MAUS environment is set.
if [[ -z "$MAUS_ROOT_DIR" ]]; then

  source maus_env.sh

fi

# Check if the Monitoring environment is set.
if [ [!DATE_HOSTNAME] ]; then

  source mon_env.sh

fi

# Check if another instance of this script is running.
pid_this=`pgrep -x start_onrec.sh`
# echo $pid_this  `echo $pid_this | wc -l`

if [ `echo $pid_this | wc -w` -gt  1 ]; then
    old=`pgrep -o start_onrec.sh`
    echo "Another instance of this script is running! (PID: "$old")"
    echo "Going to kill it."
    kill -9 $old
fi

# Check if the OnRec is running.
pid_onrec=`pgrep -x onrecomt`

# echo $pid_onrec  `echo $pid_onrec | wc -l`
if [ `echo $pid_onrec | wc -w` -gt  1 ]; then

  echo "*** ERROR: More than one Online-Reconstruction processes (onrecomt) are running on this computer! ***"
  echo "*** Kill the useless processes and start this script again. ***"
  exit

fi

if [ $pid_onrec ]; then

  echo "The Online reconstruction (onrecomt) is running. PID:" $pid_onrec

else

  nohup bin/onrecomt &>onrec.log &
  echo "Starting the Online reconstruction (onrecomt). PID:" $!

fi

while [ true ]; do

  pid=`pgrep -x onrecomt`
  if [ `echo $pid | wc -w` -eq 0 ]; then

    dt=$(date '+%d/%m/%Y %H:%M:%S');
    echo
    echo "*** The Online reconstruction process (onrecomt) is dead! ***"
    echo "***  " $dt "  ***"
    echo "Going to restart it."
    echo

    nohup bin/onrecomt &>>onrec.log &
    pid=`pgrep -x onrecomt`

  fi

#   mem=`ps -C onrecomt -o %mem | tail -n 1`
  mem=`pmap -x $pid | awk '/total/ { print $4 / 1024 }'`

  mem_int=${mem%.*}

  if [ $mem_int -gt $max_mem_usage ]; then

    dt=$(date '+%d/%m/%Y %H:%M:%S');
    echo "*** ERROR: Memory consumption exceeds the limit! ("$mem" MB) ***"
    echo "***  " $dt "  ***"
    echo "Going to restart the Online reconstruction (onrecomt)."

    touch build/stop
    sleep 5s
    rm build/stop

  fi

  # echo $mem " - " $mem_int

  sleep 30s
done


