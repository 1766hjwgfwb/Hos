
# compile bootloader
%.bin: %.asm
	nasm -f bin $< -o $@

# create master.img and add bootloader to it
master.img: boot.bin loader.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat master.img
	dd if=boot.bin of=master.img bs=512 count=1 conv=notrunc
	dd if=loader.bin of=master.img bs=512 count=4 seek=2 conv=notrunc 


# usb image creation
.PHONY: usb
usb: boot.bin /dev/sdd
	sudo dd if=/dev/sdd of=tmp.bin bs=512 count=1 conv=notrunc
	cp tmp.bin usb.bin
	sudo rm tmp.bin
	dd if=boot.bin of=usb.bin bs=446 count=1 conv=notrunc
	sudo dd if=usb.bin of=/dev/sdd bs=512 count=1 conv=notrunc
	rm usb.bin


# clean up
.PHONY: clean
clean:
	rm -rf *.bin *.img


# run bochs
.PHONY: bochs
bochs: master.img
	bochs -q
