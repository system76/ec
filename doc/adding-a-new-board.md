# Adding a new board

## Charger parameters

- `CHARGER_CHARGE_CURRENT`: Currently the same for all boards (1536).
- `CHARGER_CHARGE_VOLTAGE`: On the battery, look for 充电限制电压 (charge limit
  voltage). Convert this from volts to millivolts.
- `CHARGER_INPUT_CURRENT`: On the charger, look for DC output. Convert the
  current from amps to milliamps.

#### Example

The gaze15 battery has

```
充电限制电压: 16.8Vdc
```

and its charger has

```
DC OUTPUT (输出/輸出): 19.5V⎓9.23A 180W
```

This gives

```
CFLAGS+=\
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=16800 \
	-DCHARGER_INPUT_CURRENT=9230
```

## GPIOs

Use ecsim on the proprietary firmware to generate the configuration. Modify
`src/main.rs` if the EC needs to be changed from 5570 to 8587. Refer to
`gpio.sh`, which can automate the process.

Use the EC page in the schematics to label everything.

Not everything will be generated correctly, so check them manually when
labeling. Examples include the M block not being included in the generated
output, and `CCD_EN` being configured as `GPIO_IN` instead of `GPIO_OUT`.

## Keyboard backlight

Many models have backlit or RGB keyboards. The EC page in the board schematics
should be sufficient to determine the method used to control the backlight.

### DAC

One of the DACs is be used for controlling the backlight level.

Examples:

- galp4
- lemp9

These models use `KBLIGHT_ADJ` to control the backlight level.

### PWM

One PWM line is used for controlling brightness, and three others are used for
setting the red, green, and blue colors.

Examples:

- addw2
- gaze15
- oryp6

These models use `EC_PWM_LEDKB_{R,G,B}` to control the color of the keys.

### I2C

I2C is used to communicate with the MCU that controls the keyboard backlight
and color.

Examples:

- darp6
- oryp5
