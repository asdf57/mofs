all:
	gcc main.c root.c channel.c utils.c -o myfs `pkg-config fuse --cflags --libs`

clean:
	rm -rf mpt
	rm myfs

run:
	-@findmnt -T mpt && umount mpt
	-@[ ! -d mpt ] && mkdir mpt
	./myfs -f mpt
	umount mpt