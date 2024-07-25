#!/usr/bin/env python
import sys
import re

# get the log from ecspy
# parse the gpio control register values

args = sys.argv
ecspy_log = args[1]
lines = open(ecspy_log).readlines()
gpio_data = {}

# returns a list of positions of bits set to 1 in the integer
def int_to_bits(n):
    return [i for i in range(n.bit_length()) if n & (1 << i)]

# Regular expression to parse each line
gpr_line_re = re.compile(r"([A-M]\d): data (\d) mirror (\d) pot (\d) control ([0-9A-Fa-f]{2})")
gcr_line_re = re.compile(r"GCR(\d): 0x([0-9A-Fa-f]{2})")
gpr_grouped = {}
gcr_grouped = {}
for line in lines:
    match_gpr = gpr_line_re.match(line)
    match_gcr = gcr_line_re.match(line)
    if match_gpr:
        gpr = {}
        gpr['pin'] = match_gpr.group(1)
        gpr['data'] = int(match_gpr.group(2))
        gpr['mirror'] = int(match_gpr.group(3))
        gpr['pot'] = int(match_gpr.group(4))
        gpr['control'] = int(match_gpr.group(5), 16)

        port = gpr['pin'][0]
        port_number = int(gpr['pin'][1])

        if port not in gpr_grouped:
            gpr_grouped[port] = {}
        gpr_grouped[port][port_number] = gpr
    if match_gcr:
        gcr = {}
        gcr['number'] = int(match_gcr.group(1))
        gcr['value'] = int(match_gcr.group(2), 16)
        if gcr['number'] not in gcr_grouped:
            gcr_grouped[gcr['number']] = gcr

print("GPR:")
for group in gpr_grouped:
    print(f"\n{group}: {gpr_grouped[group]}")
print("\n\nGCR:")
for group in gcr_grouped:
    print(f"\n{group}: {gcr_grouped[group]}")

