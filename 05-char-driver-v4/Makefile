KDIR := ~/Downloads/linux
PWD  := $(shell pwd)

obj-m += char-dr-v4.o

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
