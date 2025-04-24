BUILD:=../build
SRC:=.


ENTRYPOINT:=0x10000


CFLAGS:= -m32 # 32-bit mode
CFLAGS+= -fno-builtin # disable built-in functions
CFLAGS+= -nostdinc # disable standard includes
CFLAGS+= -fno-pic # disable position-independent code
CFLAGS+= -fno-pie # disable position-independent executable
CFLAGS+= -nostdlib # disable standard libraries
CFLAGS+= -fno-stack-protector # disable stack protection
CFLAGS:= $(strip ${CFLAGS})

DEBUG:= -g
INCLUDES:= -I$(SRC)/include


# compile bootloader
$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

# elf32 kernel
$(BUILD)/%.o: $(SRC)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/%.o: $(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDES) -c $< -o $@

# compile kernel
$(BUILD)/kernel/kernel.bin: $(BUILD)/kernel/start.o \
	$(BUILD)/kernel/main.o \
	$(BUILD)/kernel/io.o \
	$(BUILD)/kernel/console.o \
	$(BUILD)/kernel/printk.o \
	$(BUILD)/kernel/assert.o \
	$(BUILD)/kernel/debug.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/lib/vsprintk.o

	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

# obj
$(BUILD)/system.bin: $(BUILD)/kernel/kernel.bin
	objcopy -O binary $< $@

# map
$(BUILD)/system.map: $(BUILD)/kernel/kernel.bin
	nm $< | sort > $@


# create master.img and add bootloader to it
$(BUILD)/master.img: $(BUILD)/boot/boot.bin \
	$(BUILD)/boot/loader.bin \
	$(BUILD)/system.bin \
	$(BUILD)/system.map

	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD)/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc


# usb image creation
.PHONY: usb
usb: $(BUILD)/boot/boot.bin /dev/sdd
	sudo dd if=/dev/sdd of=tmp.bin bs=512 count=1 conv=notrunc
	cp tmp.bin usb.bin
	sudo rm tmp.bin
	dd if=$(BUILD)/boot/boot.bin of=usb.bin bs=446 count=1 conv=notrunc
	sudo dd if=usb.bin of=/dev/sdd bs=512 count=1 conv=notrunc
	rm usb.bin

.PHONY:test
test: $(BUILD)/master.img


# clean up
.PHONY: clean
clean:
	rm -rf $(BUILD)


# run bochs
.PHONY: bochs
bochs: $(BUILD)/master.img
	bochs -q -f ./bochs/bochsrc


# run qemu
.PHONY: qemu
qemu: $(BUILD)/master.img
	qemu-system-i386 \
	-m 32M \
	-boot c \
	-hda $<

# run qemu with gdb
.PHONY: qemu-gdb
qemu-gdb: $(BUILD)/master.img
	qemu-system-i386 \
	-s -S \
	-m 32M \
	-boot c \
	-hda $< \


# run vmware
$(BUILD)/master.vmdk: $(BUILD)/master.img
	qemu-img convert -O vmdk $< $@

.PHONY: vmdk
vmdk: $(BUILD)/master.vmdk