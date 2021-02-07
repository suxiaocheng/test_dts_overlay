CC:=gcc
ANDROID_ROOT=/work/server_me_10.92.35.129/milan

main.o: main.c
	$(CC) -g -o $@ $< -I${ANDROID_ROOT}/external/dtc/libfdt/ -I${ANDROID_ROOT}/system/libufdt/include \
		-L. -lufdt -lufdt_sysdeps -lfdt

.PHONY: clean

clean:
	rm *.o
