#!/usr/bin/env python
import sys
import re

# get the log from ecspy
# parse the gpio control register values

args = sys.argv
ecspy_log = args[1]
lines = open(ecspy_log).readlines()
gpio_data = {}

# Regular expression to parse each line
line_re = re.compile(r"([A-M]\d): data (\d) mirror (\d) pot (\d) control ([0-9A-Fa-f]{2})")

gpr_grouped = {}
for line in lines:
    match = line_re.match(line)
    if match:
        gpr = {}
        gpr['pin'] = match.group(1)
        gpr['data'] = int(match.group(2))
        gpr['mirror'] = int(match.group(3))
        gpr['pot'] = int(match.group(4))
        gpr['control'] = int(match.group(5), 16)

        port = gpr['pin'][0]
        port_number = int(gpr['pin'][1])

        if port not in gpr_grouped:
            gpr_grouped[port] = {}
        gpr_grouped[port][port_number] = gpr


for group in gpr_grouped:
    print(gpr_grouped[group])
    for reg in group:
        print(reg)


