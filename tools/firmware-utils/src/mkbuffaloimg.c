/*
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2016 FUKAUMI Naoki <naobsd@gmail.com>
 *
 *  Based on mkdniimg.c
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>     /* for unlink() */
#include <libgen.h>
#include <getopt.h>     /* for getopt() */
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>

#define DNI_HDR_LEN	128

/*
 * Globals
 */
static char *ifname;
static char *progname;
static char *ofname;
static char *version = "0.00_0.00";
static char *region = "JP";
static char *rootfs_size;
static char *kernel_size;

static char *board_id;
/*
 * Message macros
 */
#define ERR(fmt, ...) do { \
	fflush(0); \
	fprintf(stderr, "[%s] *** error: " fmt "\n", \
			progname, ## __VA_ARGS__ ); \
} while (0)

#define ERRS(fmt, ...) do { \
	int save = errno; \
	fflush(0); \
	fprintf(stderr, "[%s] *** error: " fmt ": %s\n", \
			progname, ## __VA_ARGS__, strerror(save)); \
} while (0)

void usage(int status)
{
	FILE *stream = (status != EXIT_SUCCESS) ? stderr : stdout;

	fprintf(stream, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stream,
"\n"
"Options:\n"
"  -B <board>       create image for the board specified with <board>\n"
"  -i <file>        read input from the file <file>\n"
"  -o <file>        write output to the file <file>\n"
"  -v <version>     set image version to <version>\n"
"  -r <region>      set image region to <region>\n"
"  -R <rootfs_size> set RootfsSize to <rootfs_size>\n"
"  -K <kernel_size> set KernelSize to <kernel_size>\n"
"  -h               show this screen\n"
	);

	exit(status);
}

int main(int argc, char *argv[])
{
	int res = EXIT_FAILURE;
	int buflen;
	int err;
	struct stat st;
	char *buf;
	int i;
	uint8_t csum;

	FILE *outfile, *infile;

	progname = basename(argv[0]);

	while ( 1 ) {
		int c;

		c = getopt(argc, argv, "B:i:o:v:r:R:K:h");
		if (c == -1)
			break;

		switch (c) {
		case 'B':
			board_id = optarg;
			break;
		case 'i':
			ifname = optarg;
			break;
		case 'o':
			ofname = optarg;
			break;
		case 'v':
			version = optarg;
			break;
		case 'r':
			region = optarg;
			break;
		case 'R':
			rootfs_size = optarg;
			break;
		case 'K':
			kernel_size = optarg;
			break;
		case 'h':
			usage(EXIT_SUCCESS);
			break;
		default:
			usage(EXIT_FAILURE);
			break;
		}
	}

	if (board_id == NULL) {
		ERR("no board specified");
		goto err;
	}

	if (rootfs_size == NULL) {
		ERR("no rootfs_size specified");
		goto err;
	}

	if (kernel_size == NULL) {
		ERR("no kernel_size specified");
		goto err;
	}

	if (ifname == NULL) {
		ERR("no input file specified");
		goto err;
	}

	if (ofname == NULL) {
		ERR("no output file specified");
		goto err;
	}

	err = stat(ifname, &st);
	if (err){
		ERRS("stat failed on %s", ifname);
		goto err;
	}

	buflen = st.st_size + DNI_HDR_LEN + 1;
	buf = malloc(buflen);
	if (!buf) {
		ERR("no memory for buffer\n");
		goto err;
	}

	memset(buf, 0, DNI_HDR_LEN);
	snprintf(buf, DNI_HDR_LEN, "device:%s\nversion:%s\nregion:%s\n"
		 "RootfsSize:%s\nKernelSize:%s\nInfoHeadSize:128\n",
		 board_id, version, region, rootfs_size, kernel_size);
	buf[DNI_HDR_LEN - 2] = 0x12;
	buf[DNI_HDR_LEN - 1] = 0x32;

	infile = fopen(ifname, "r");
	if (infile == NULL) {
		ERRS("could not open \"%s\" for reading", ifname);
		goto err_free;
	}

	errno = 0;
	fread(buf +  DNI_HDR_LEN, st.st_size, 1, infile);
	if (errno != 0) {
		ERRS("unable to read from file %s", ifname);
		goto err_close_in;
	}

	csum = 0;
	for (i = 0; i < (st.st_size + DNI_HDR_LEN); i++)
		csum += buf[i];

	csum = 0xff - csum;
	buf[st.st_size + DNI_HDR_LEN] = csum;

	outfile = fopen(ofname, "w");
	if (outfile == NULL) {
		ERRS("could not open \"%s\" for writing", ofname);
		goto err_close_in;
	}

	errno = 0;
	fwrite(buf, buflen, 1, outfile);
	if (errno) {
		ERRS("unable to write to file %s", ofname);
		goto err_close_out;
	}

	res = EXIT_SUCCESS;

	fflush(outfile);

 err_close_out:
	fclose(outfile);
	if (res != EXIT_SUCCESS) {
		unlink(ofname);
	}

 err_close_in:
	fclose(infile);

 err_free:
	free(buf);

 err:
	return res;
}
