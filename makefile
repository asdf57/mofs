all:
	gcc main.c -o myfs `pkg-config fuse --cflags --libs`
