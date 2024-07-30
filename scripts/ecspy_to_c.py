#!/usr/bin/env python
import sys
import re


gpcr_control_pin_mode_to_c_define = {
    "00": "GPIO_ALT1",
    "01": "GPIO_OUT",
    "10": "GPIO_IN",
}


# parses the log file given as a list of lines
# returns two dicts
# - GCR register values: {GCR_number(int):
#       value(list of '1' positions in binary form)
#   }
# - GP registers: {letter(str): {
#       number(int): {
#           data(int),
#           mirror(int),
#           pot(int),
#           control: (list of '1' positions in binary form
#       }
#   }
def parse_ecspy_log(lines: list[str]) -> tuple[dict, dict]:
    gpr_line_re = re.compile(r''
        '(?P<letter>[A-M])'
        '(?P<number>\d): '
        '(data (?P<data>\d) )'
        '(mirror (?P<mirror>\d) )'
        '(pot (?P<pot>\d) )?'
        '(control (?P<control>[0-9A-Fa-f]{2}))'
    )
    gcr_line_re = re.compile(r''
        'GCR(?P<number>\d?): '
        '0x(?P<value>[0-9A-Fa-f]{2})'
    )
    gpr_grouped = {}
    gcr_grouped = {}
    for line in lines:
        match_gpr = gpr_line_re.match(line)
        match_gcr = gcr_line_re.match(line)
        if match_gpr:
            # print(match_gpr.groups())
            gpr = {}
            gpr['letter'] = match_gpr.group('letter')
            gpr['number'] = int(match_gpr.group('number'))
            gpr['data'] = int(match_gpr.group('data'))
            gpr['mirror'] = int(match_gpr.group('mirror'))
            gpr['pot'] = int(match_gpr.group('pot')) \
                if match_gpr.group('pot') is not None \
                else None
            gpr['control'] = int_to_bit_positions(int(match_gpr.group('control'), 16))

            if gpr['letter'] not in gpr_grouped:
                gpr_grouped[gpr['letter']] = {}
            gpr_grouped[gpr['letter']][gpr['number']] = gpr

        if match_gcr:
            gcr = {}
            gcr['number'] = match_gcr.group('number')
            gcr['value'] = int_to_bit_positions(int(match_gcr.group('value'), 16))
            if gcr['number'] not in gcr_grouped:
                gcr_grouped[gcr['number']] = gcr
    return gpr_grouped, gcr_grouped

def bit_positions_to_hex(positions: list[int]) -> str:
    return hex(sum([1 << i for i in positions]))

# returns a string defining the GPCR register value
# using #defines for the bit positions
def gpcr_control_bit_positions_to_c(positions: list[int]) -> str:
    macros = []
    pimmode = ("1" if 7 in positions else "0") + ("1" if 6 in positions else "0")
    macros.append(gpcr_control_pin_mode_to_c_define[pimmode])
    if 2 in positions:
        macros.append("GPIO_UP")
    if 1 in positions:
        macros.append("GPIO_DOWN")
    [print(f"WARNING: undefined GPCR control bit position: {pos} in {bit_positions_to_hex(positions)}") \
        for pos in positions if pos not in [1, 2, 6, 7]]
    output = " | ".join(macros)
    return output

# returns a list of positions of bits set to 1 in the integer
# e.g. 5(0b101) -> [0, 2]
def int_to_bit_positions(n: int) -> list[int]:
    return [i for i in range(n.bit_length()) if n & (1 << i)]

# converts a list of bit positions to a string defining setting the bits
# using BIT() macro
# e.g. [0, 2] -> "BIT(2) | BIT(0)"
def bit_positions_to_macros(positions: list[int]) -> str:
    positions.sort(reverse=True)
    macros = [f"BIT({pos})" for pos in positions]
    macros_joined = " | ".join(macros)
    return macros_joined

def gpcr_is_output(gpdr: dict) -> bool:
    return 6 in gpdr['control'] and 7 not in gpdr['control']

# converts a dictionary of gpdr letter group
# data to a list of bit positions
# where the data is set to 1
# e.g.  {
#           0: {'data': 1, (...)}
#           1: {'data': 0, (...)}
#           2: {'data': 1, (...)}
#       } -> [0,2]
def gpdr_letter_group_data_to_bit_positions(gpdrs: dict) -> list[int]:
    bit_positions = []
    for number in gpdrs:
        if gpdrs[number]['data'] == 1 and gpcr_is_output(gpdrs[number]):
            bit_positions.append(number)
    return bit_positions

# converts a dictionary of gcr values to a string
# defining the gcr values in c using BIT() macro
def gcr_grouped_to_c(gcr_grouped: dict) -> str:
    converted_output = "\n\n//GCR\n\n"
    for number in gcr_grouped:
        gcr = gcr_grouped[number]
        if gcr['value']:
            macros = bit_positions_to_macros(gcr['value'])
            converted_output += f"GCR{gcr['number']} = {macros};\n"
        else:
            converted_output += f"GCR{gcr['number']} = 0;\n"
    return converted_output

# converts a dictionary of gpr values to a string
# defining the gpr values in c using BIT() macro
# and GPCR values using GPIO defines
def gpr_grouped_to_c(gpr_grouped: dict) -> str:
    converted_output = "\n\n//GPDR\n\n"
    for letter in gpr_grouped:
        data_value = ""
        bit_positions = gpdr_letter_group_data_to_bit_positions(gpr_grouped[letter])
        data_value = bit_positions_to_macros(bit_positions)
        if data_value == "":
            data_value = "0"
        converted_output += f"GPDR{letter} = {data_value};\n"
    converted_output += "\n\n//GPCR\n\n"
    for letter in gpr_grouped:
        for number in gpr_grouped[letter]:
            gpr = gpr_grouped[letter][number]
            macros = gpcr_control_bit_positions_to_c(gpr['control'])
            if macros != "":
                converted_output += f"GPCR{letter}{number} = {macros};\n"
            else:
                print(f"WARNING: GPR{letter}{number}: {gpr} undefined control value: {gpr['control']}")
                exit(1)
    return converted_output

args = sys.argv

def usage():
    print(f"Usage: {args[0]} <ecspy_log_file>")
    exit(1)

if __name__ == "__main__":
    if len(args) != 2:
        usage()
    ecspy_log = args[1]

    try:
        lines = open(ecspy_log).readlines()
    except FileNotFoundError:
        print(f"File {ecspy_log} not found")
        exit(1)

    gpr_grouped, gcr_grouped = parse_ecspy_log(lines)

    # print("GPR:")
    # for group in gpr_grouped:
    #     print(f"\n{group}: {gpr_grouped[group]}")
    # print("\n\nGCR:")
    # for group in gcr_grouped:
    #    print(f"\n{group}: {gcr_grouped[group]}")

    output = gcr_grouped_to_c(gcr_grouped)
    output += gpr_grouped_to_c(gpr_grouped)

    print(output)
