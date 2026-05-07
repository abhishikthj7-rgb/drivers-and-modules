KDIR := ~/Downloads/linux
PWD  := $(shell pwd)

obj-m += char-dr.o

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
