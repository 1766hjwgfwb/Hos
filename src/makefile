BUILD:=../build
SRC:=.


ENTRYPOINT:=0x10000


# compile bootloader
$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

# elf32 kernel
$(BUILD)/kernel/%.o: $(SRC)/kernel/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $< -o $@

# compile kernel
$(BUILD)/kernel/kernel.bin: $(BUILD)/kernel/start.o
	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

# obj
$(BUILD)/system.bin: $(BUILD)/kernel/kernel.bin
	objcopy -O binary $< $@

# map
$(BUILD)/system.map: $(BUILD)/kernel/kernel.bin
	nm $< | sort > $@


# create master.img and add bootloader to it
$(BUILD)/master.img: $(BUILD)/boot/boot.bin $(BUILD)/boot/loader.bin $(BUILD)/system.bin $(BUILD)/system.map

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


test: $(BUILD)/master.img


# clean up
.PHONY: clean
clean:
	rm -rf $(BUILD)


# run bochs
.PHONY: bochs
bochs: $(BUILD)/master.img
	bochs -q
