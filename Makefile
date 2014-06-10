NDK_ROOT?=
CCPREFIX=$(NDK_ROOT)/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86_64/bin/arm-linux-androideabi-
SYSROOT=$(NDK_ROOT)/platforms/android-9/arch-arm
CC=$(CCPREFIX)gcc --sysroot=$(SYSROOT)
TARGET=ptrace_dumper

$(TARGET):ptrace_dumper.o
	$(CC) -g -Wall $^ -o $@
%.o:%.c
	$(CC) -g -Wall -c $^ -o $@

clean:
	rm $(TARGET) *.o
