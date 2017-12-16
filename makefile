odir   = ./build

sdir   = ./server
ssrc   = $(shell find $(sdir) -name "*.c")
sbin   = $(odir)/server
sdep   = -lsqlite3 -lm -lpthread

cdir   = ./client
csrc   = $(shell find $(cdir) -name "*.c")
cbin   = $(odir)/client
cdep   = -lpthread -lwayland-client

adir   = ./assembler
asrc   = $(shell find $(adir) -name "*.c")
abin   = $(odir)/assembler

store  = $(odir)/store/vm

warns  = -Wno-incompatible-pointer-types-discards-qualifiers -Wno-return-type

cc     = clang
cflags = -g --std=gnu11 $(warns)

.PHONY: client server build clean dirs

build: dirs $(sbin) $(cbin)
client: dirs $(cbin)
server: dirs $(sbin)
assembler: dirs $(abin)

$(sbin):
	$(cc) $(cflags) $(sdep) $(ssrc) -o $(sbin)

$(cbin):
	$(cc) $(cflags) $(cdep) $(csrc) -o $(cbin)

$(abin):
	$(cc) $(cflags) $(asrc) -o $(abin)

dirs:
	mkdir -p $(odir) $(store)

clean:
	rm -r $(odir)
