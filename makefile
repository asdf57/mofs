CC=gcc
CMDS=`pkg-config fuse --cflags --libs`
BIN=bin
MNT=mpt
OUT=bin
EXC=$(OUT)/myfs

.PHONY: all build clean run

all: clean build run

$(EXC): src/main.c src/chan.c src/hash.c src/file.c src/logger.c src/root.c
	mkdir -p $(BIN)
	${CC} -g -o $@ $^ ${CMDS}

build: $(EXC)

clean:
	findmnt -T $(MNT) && (umount $(MNT) || true) || true
	rm -rf $(MNT)
	rm -rf $(OUT)

run: build
	findmnt -T $(MNT) && (umount $(MNT) || true) || true
	mkdir -p $(MNT)
	$(EXC) -f $(MNT)
	umount $(MNT)
