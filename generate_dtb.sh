#!/bin/bash

if [ $# -ge 1 ]; then
	if [ $1 == "clean" ]; then
		rm *.img *.dtb *.dtbo
	else
		echo "unknown parmater $1"
	fi
	exit 0
fi

dtc -@ -O dtb -o my_main_dt.dtb my_main_dt.dts
dtc -@ -O dtb -o my_overlay_dt.dtbo my_overlay_dt.dts
dtc -@ -O dtb -o my_overlay_dt_1.dtbo my_overlay_dt_1.dts

# ~/bin/mkdtimg create main_dt.img my_main_dt.dtb
~/bin/mkdtimg create my_overlay_dt.img my_overlay_dt.dtbo
~/bin/mkdtimg create my_overlay_dt_1.img my_overlay_dt_1.dtbo


dtc -@ -O dtb -o lcm_parmater.dtbo lcm_parmater.dts
mkdtimg create lcm_parmater.img lcm_parmater.dtbo
