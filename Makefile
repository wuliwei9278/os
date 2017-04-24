#
# Makefile
#

CIRCLEHOME = .

OBJS	= main.o kernel.o

LIBS	= $(CIRCLEHOME)/lib/libcircle.a

all:
	cd $(CIRCLEHOME)/lib && $(MAKE)
	$(MAKE) $(TARGET).img 

cleanall: clean
	cd $(CIRCLEHOME)/lib && rm -f *.o *.a *.elf *.lst *.img *.cir *.map *~

include $(CIRCLEHOME)/Rules.mk