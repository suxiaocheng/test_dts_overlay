#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "libfdt.h"
#include "ufdt_overlay.h"

char *main_dtb = NULL;
size_t main_dtb_size = 0;
char *overlay_dtb_1 = NULL;
size_t overlay_dtb_1_size = 0;
char *overlay_dtb_2 = NULL;
size_t overlay_dtb_2_size = 0;

int readfile(char *filename, char **storage, size_t *size)
{
	int fd;
	int err = 0;
	struct stat buf;
	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		printf("open file %s fail\n", filename);
		return errno;
	}

	err = fstat(fd, &buf);
	if (err == -1) {
		printf("stat file %s fail\n", filename);
		err = errno;
		goto out;
	}

	*size = buf.st_size;

	*storage = malloc(buf.st_size);
	if (*storage == NULL) {
		printf("malloc fail\n");
		err = -ENOMEM;
		goto out;
	}

	err = read(fd, *storage, buf.st_size);
	if (err < buf.st_size) {
		printf("read buf less than except, current: %d, except: %ld\n", err, buf.st_size);
		err = -1;
		goto out;
	}
	err = 0;

out:
	close(fd);
	return err;
}

int disp_fdt_getprop_u32_array(struct fdt_header *merged_fdt, int nodeoffset,
                               const char *name, unsigned int *out_value)
{
        unsigned int i;
        unsigned int *data = NULL;
        int len = 0;

        data = (unsigned int*)fdt_getprop(merged_fdt, nodeoffset, name, &len);
        if (len > 0) {
                len = len / sizeof(unsigned int);
                for (i=0; i<len; i++) {
                        *(out_value+i) = fdt32_to_cpu(*(data+i));
                }
        } else {
                *out_value = 0;
        }

        return len;
}

int main(int argc, char **argv)
{
	int err;
	struct fdt_header *blob;
	struct fdt_header *merged_fdt = NULL;

	err = readfile("/home/user/tmp/test_dts_overlay/my_main_dt.dtb", &main_dtb, &main_dtb_size);
	if (err != 0) {
		printf("read file fail: %d\n", err);
		goto out;
	}
	readfile("/home/user/tmp/test_dts_overlay/my_overlay_dt.dtbo", &overlay_dtb_1, &overlay_dtb_1_size);
	if (err != 0) {
		printf("read file fail: %d\n", err);
		goto out;
	}
	readfile("/home/user/tmp/test_dts_overlay/my_overlay_dt_1.dtbo", &overlay_dtb_2, &overlay_dtb_2_size);
	if (err != 0) {
		printf("read file fail: %d\n", err);
		goto out;
	}

	err = fdt_open_into(main_dtb, main_dtb, main_dtb_size);
	if (err < 0) {
		printf("fdt_open_into fail: %d\n", err);
	}

	blob = ufdt_install_blob(main_dtb, main_dtb_size);
	if (blob == NULL) {
		printf("ufdt_install_blob() failed!\n");
		goto out;
	}

	merged_fdt = ufdt_apply_overlay(blob, main_dtb_size, overlay_dtb_1, overlay_dtb_1_size);
	if (merged_fdt == NULL) {
		printf("ufdt_apply_overlay() failed!\n");
		goto out;
	}

	err = fdt_pack((void *)merged_fdt);
	if (err < 0) {
		printf("fdt_pack(merged_fdt) failed !\n");
		if (merged_fdt){
			free(merged_fdt);
			merged_fdt = NULL;
			goto out;
		}
	}
#if 1
	blob = ufdt_install_blob(merged_fdt, fdt_totalsize(merged_fdt));
	if (blob == NULL) {
		printf("ufdt_install_blob() failed!\n");
		goto out;
	}

	merged_fdt = ufdt_apply_overlay(blob, fdt_totalsize(merged_fdt), overlay_dtb_2, overlay_dtb_2_size);
	if (merged_fdt == NULL) {
		printf("ufdt_apply_overlay() failed!\n");
		goto out;
	}

	err = fdt_pack((void *)merged_fdt);
	if (err < 0) {
		printf("fdt_pack(merged_fdt) failed !\n");
		if (merged_fdt){
			free(merged_fdt);
			merged_fdt = NULL;
			goto out;
		}
	}
#endif
	/* test if the function is ok */
	{
		int offset;
		int len = 0;
		const char *data = NULL;
		int value;
		offset = fdt_node_offset_by_compatible(merged_fdt, -1, "my_node");
		printf("offset is: %d\n", offset);
		data = fdt_getprop(merged_fdt, offset, "status", &len);
		if (len > 0) {
			printf("status = %s\n", data);
		} else {
			printf("status not found\n");
		}

		offset = fdt_node_offset_by_compatible(merged_fdt, -1, "my_child");
		printf("offset is: %d\n", offset);
		disp_fdt_getprop_u32_array(merged_fdt, offset, "value", &value);
		if (len > 0) {
			printf("value = %x\n", value);
		} else {
			printf("value not found\n");
		}
	}

out:
	if (merged_fdt != NULL) {
		free(merged_fdt);
	}
	if (main_dtb != NULL) {
		free(main_dtb);
	}
	if (overlay_dtb_1 != NULL) {
		free(overlay_dtb_1);
	}
	if (overlay_dtb_2 != NULL) {
		free(overlay_dtb_2);
	}
	return 0;
}

