
# compile bootloader
boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin	

# create master.img and add bootloader to it
master.img: boot.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat master.img
	dd if=boot.bin of=master.img bs=512 count=1 conv=notrunc


# clean up
.PHONY: clean
clean:
	rm -rf *.bin *.img


# run bochs
.PHONY: bochs
bochs: master.img
	bochs -q
