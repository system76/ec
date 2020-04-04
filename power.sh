#!/usr/bin/env bash

set -e

./ectool.sh info

header=1
if [ -e power.csv ]
then
    header=0
fi

while true
do
    if [ "${header}" == "1" ]
    then
        F="Time    "
        F="${F}\tBAT V"
        F="${F}\tBAT A"
        F="${F}\tCPU W"
        F="${F}\tCPU C"
        F="${F}\tFAN %"
    else
        F="$(date "+%T")"

        uV="$(cat /sys/class/power_supply/BAT0/voltage_now)"
        V="$(echo "${uV}/1000000" | bc -lq)"
        F="${F}\t$(printf "%.2f" "${V}")"

        uA="$(cat /sys/class/power_supply/BAT0/current_now)"
        A="$(echo "${uA}/1000000" | bc -lq)"
        F="${F}\t$(printf "%.2f" "${A}")"

        last_E="$(cat /sys/class/powercap/intel-rapl\:0/energy_uj)"
        sleep 1
        next_E="$(cat /sys/class/powercap/intel-rapl\:0/energy_uj)"
        W="$(echo "(${next_E} - ${last_E})/1000000" | bc -lq)"
        F="${F}\t$(printf "%.1f" "${W}")"

        T="$(cat /sys/devices/platform/coretemp.0/hwmon/hwmon*/temp1_input)"
        C="$(echo "${T}/1000" | bc -lq)"
        F="${F}\t$(printf "%.1f" "${C}")"

        D="$(sudo tool/target/release/system76_ectool fan 0)"
        P="$(echo "(${D} * 100)/255" | bc -lq)"
        F="${F}\t$(printf "%.0f" "${P}")"
    fi

    for file in /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq
    do
        if [ "${header}" == "1" ]
        then
            id="$(basename "$(dirname "$(dirname "${file}")")")"
            F="${F}\t${id}"
        else
            KHz="$(cat "${file}")"
            MHz="$(echo "${KHz}/1000" | bc -lq)"
            F="${F}\t$(printf "%.0f" "${MHz}")"
        fi
    done

    echo -e "${F}" | tee -a power.csv

    header=0
done
