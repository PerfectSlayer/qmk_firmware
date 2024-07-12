# PerfectSlayer keymap for Redox

## Arduino Pro Micro variant (default)

Configure default configuration to build the firmware:
```shell
user.keyboard=redox/rev1
user.keymap=perfectslayer
```

Then build the firmware using `qmk compile` or flash both half keyboard parts using default `qmk flash` commands.

## RP2040 Variant

```shell
user.keyboard=redox/rev1/rp2040
user.keymap=perfectslayer
```

Then build the firmware using `qmk compile`, or flash both half keyboard parts using  `qmk flash -bl uf2-split-left` and `qmk flash -bl uf2-split-right` commands and double-tap the reset button for uploading the firmware.
