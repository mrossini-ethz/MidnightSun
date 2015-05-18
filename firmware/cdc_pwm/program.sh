#/bin/bash
# double click reset button before execution
avrdude -patmega32u4 -cavr109 -P/dev/tty.usbmodemfa131 -b57600 -D -V -Uflash:w:./led_ctrl.hex:i
