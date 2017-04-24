#
# Rules.mk
#
# Circle - A C++ bare metal environment for Raspberry Pi
# Copyright (C) 2014-2017  R. Stange <rsta2@o2online.de>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

ifeq ($(strip $(CIRCLEHOME)),)
CIRCLEHOME = ..
endif

-include $(CIRCLEHOME)/Config.mk

RASPPI	?= 2
PREFIX	?= arm-eabi-

CC	= $(PREFIX)gcc
CPP	= $(PREFIX)g++
AS	= $(CC)
LD	= $(PREFIX)ld
AR	= $(PREFIX)ar

ifeq ($(strip $(RASPPI)),1)
ARCH	?= -march=armv6k -mtune=arm1176jzf-s -mfloat-abi=hard
TARGET	?= kernel
else ifeq ($(strip $(RASPPI)),2)
ARCH	?= -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard
TARGET	?= kernel7
else
ARCH	?= -march=armv8-a -mtune=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard
TARGET	?= kernel8-32
endif

OPTIMIZE ?= -O2

INCLUDE	+= -I $(CIRCLEHOME)/include -I $(CIRCLEHOME)/addon -I $(CIRCLEHOME)/app/lib

DEBUG   ?= -DDEBUG -g

AFLAGS	+= $(ARCH) -DRASPPI=$(RASPPI) $(INCLUDE)

CFLAGS	+= $(ARCH) -Wall -fsigned-char -fno-builtin -nostdinc -nostdlib -D__circle__ -DRASPPI=$(RASPPI) $(INCLUDE) $(OPTIMIZE) $(DEBUG)

CPPFLAGS+= $(CFLAGS) -fno-exceptions -fno-rtti -std=c++14

%.o: %.S
	$(AS) $(AFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

$(TARGET).img: $(OBJS) $(LIBS) $(CIRCLEHOME)/lib/startup.o $(CIRCLEHOME)/circle.ld
	$(LD) -o $(TARGET).elf -Map $(TARGET).map -T $(CIRCLEHOME)/circle.ld $(CIRCLEHOME)/lib/startup.o $(OBJS) $(LIBS)
	$(PREFIX)objdump -d $(TARGET).elf | $(PREFIX)c++filt > $(TARGET).lst
	$(PREFIX)objcopy $(TARGET).elf -O binary $(TARGET).img
	wc -c $(TARGET).img

.PHONY: all,clean,cleanall,run,runimg,install,dockerenv,dockerbuildimage,dockerupdateimage,code

clean:
	rm -f *.o *.a *.elf *.lst *.img *.cir *.map *~ $(EXTRACLEAN)

run : 
	qemu-system-arm -M raspi2 -m 128M -serial stdio -kernel kernel7.elf

runimg :
	qemu-system-arm -M raspi2 -m 128M -bios kernel7.img

#FILES DURING PI BOOT PROCESS
bootcode.bin :
	wget -nc https://raw.githubusercontent.com/raspberrypi/firmware/master/boot/bootcode.bin
start.elf :
	wget -nc https://raw.githubusercontent.com/raspberrypi/firmware/master/boot/start.elf	 
config.txt :
	echo '' > config.txt

install : kernel7 bootcode.bin start.elf config.txt 
	echo 'unimplemented! copy files to sd card on real RP2'

dockerenv:
	test ! -f /.dockerenv # echo 'Run the command outside the docker container!'

code : dockerenv
	docker run --rm --name arm -i -t -p 5900:5900 -v "$(shell pwd)":/work arm zsh

dockerbuildimage : dockerenv ./docker/Dockerfile 
	docker build -t arm docker

dockerupdateimage : dockerenv ./docker/Dockerfile
	docker commit arm
	docker attach arm
