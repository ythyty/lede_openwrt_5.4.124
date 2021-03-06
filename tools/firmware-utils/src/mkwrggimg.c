/*
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2016 Stijn Tintel <stijn@linux-ipv6.be>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#define _ANSI_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <getopt.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>

#include "md5.h"

#define ERR(fmt, ...) do { \
	fflush(0); \
	fprintf(stderr, "[%s] *** error: " fmt "\n", \
			progname, ## __VA_ARGS__ ); \
} while (0)

#define ERRS(fmt, ...) do { \
	int save = errno; \
	fflush(0); \
	fprintf(stderr, "[%s] *** error: " fmt ", %s\n", \
			progname, ## __VA_ARGS__, strerror(save)); \
} while (0)

#define WRGG03_MAGIC	0x20080321

struct wrgg03_header {
	char		signature[32];
	uint32_t	magic1;
	uint32_t	magic2;
	char		version[16];
	char		model[16];
	uint32_t	flag[2];
	uint32_t	reserve[2];
	char		buildno[16];
	uint32_t	size;
	uint32_t	offset;
	char		devname[32];
	char		digest[16];
} __attribute__ ((packed));

static char *progname;
static char *ifname;
static char *ofname;
static char *signature;
static char *version;
static char *model;
static uint32_t flag = 0;
static uint32_t reserve = 0;
static char *buildno;
static uint32_t offset;
static char *devname;
static int big_endian;

void usage(int status)
{
	FILE *stream = (status != EXIT_SUCCESS) ? stderr : stdout;

	fprintf(stream, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stream,
"\n"
"Options:\n"
"  -b              create image in big endian format\n"
"  -B <buildno>    build number\n"
"  -i <file>       read input from the file <file>\n"
"  -d <name>       set device name to <name>\n"
"  -m <model>      model name\n"
"  -o <file>       write output to the file <file>\n"
"  -O <offset>     set offset to <offset>\n"
"  -s <sig>        set image signature to <sig>\n"
"  -h              show this screen\n"
	);

	exit(status);
}

static void put_u32(void *data, uint32_t val, int swap)
{
	unsigned char *p = data;

	if (swap) {
		p[0] = (val >> 24) & 0xff;
		p[1] = (val >> 16) & 0xff;
		p[2] = (val >> 8) & 0xff;
		p[3] = val & 0xff;
	} else {
		p[3] = (val >> 24) & 0xff;
		p[2] = (val >> 16) & 0xff;
		p[1] = (val >> 8) & 0xff;
		p[0] = val & 0xff;
	}
}

static void get_digest(struct wrgg03_header *header, char *data, int size)
{
	MD5_CTX ctx;

	MD5_Init(&ctx);

	MD5_Update(&ctx, (char *)&header->offset, sizeof(header->offset));
	MD5_Update(&ctx, (char *)&header->devname, sizeof(header->devname));
	MD5_Update(&ctx, data, size);

	MD5_Final(header->digest, &ctx);
}

int main(int argc, char *argv[])
{
	struct wrgg03_header *header;
	char *buf;
	struct stat st;
	int buflen;
	int err;
	int res = EXIT_FAILURE;

	FILE *outfile, *infile;

	progname = basename(argv[0]);

	while ( 1 ) {
		int c;

		c = getopt(argc, argv, "bd:i:m:o:s:v:B:O:h");
		if (c == -1)
			break;

		switch (c) {
		case 'b':
			big_endian = 1;
			break;
		case 'B':
			buildno = optarg;
			break;
		case 'd':
			devname = optarg;
			break;
		case 'i':
			ifname = optarg;
			break;
		case 'm':
			model = optarg;
			break;
		case 'o':
			ofname = optarg;
			break;
		case 's':
			signature = optarg;
			break;
		case 'v':
			version = optarg;
			break;
		case 'O':
			offset = strtoul(optarg, NULL, 0);
			break;
		case 'h':
			usage(EXIT_SUCCESS);
			break;

		default:
			usage(EXIT_FAILURE);
			break;
		}
	}

	if (signature == NULL) {
		ERR("no signature specified");
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

	if (devname == NULL) {
		ERR("no device name specified");
		goto err;
	}

	if (model == NULL) {
		ERR("no model name specified");
		goto err;
	}

	if (buildno == NULL) {
		ERR("no build number specified");
		goto err;
	}

	if (version == NULL) {
		ERR("no version specified");
		goto err;
	}

	err = stat(ifname, &st);
	if (err){
		ERRS("stat failed on %s", ifname);
		goto err;
	}

	buflen = st.st_size + sizeof(struct wrgg03_header);
	buf = malloc(buflen);
	if (!buf) {
		ERR("no memory for buffer\n");
		goto err;
	}

	infile = fopen(ifname, "r");
	if (infile == NULL) {
		ERRS("could not open \"%s\" for reading", ifname);
		goto err_free;
	}

	errno = 0;
	fread(buf + sizeof(struct wrgg03_header), st.st_size, 1, infile);
	if (errno != 0) {
		ERRS("unable to read from file %s", ifname);
		goto close_in;
	}

	header = (struct wrgg03_header *) buf;
	memset(header, '\0', sizeof(struct wrgg03_header));

	strncpy(header->signature, signature, sizeof(header->signature));
	put_u32(&header->magic1, WRGG03_MAGIC, 0);
	put_u32(&header->magic2, WRGG03_MAGIC, 0);
	strncpy(header->version, version, sizeof(header->version));
	strncpy(header->model, model, sizeof(header->model));
	put_u32(&header->flag, flag, 0);
	put_u32(&header->reserve, reserve, 0);
	strncpy(header->buildno, buildno, sizeof(header->buildno));
	put_u32(&header->size, st.st_size, big_endian);
	put_u32(&header->offset, offset, big_endian);
	strncpy(header->devname, devname, sizeof(header->devname));

	get_digest(header, buf + sizeof(struct wrgg03_header), st.st_size);

	outfile = fopen(ofname, "w");
	if (outfile == NULL) {
		ERRS("could not open \"%s\" for writing", ofname);
		goto close_in;
	}

	errno = 0;
	fwrite(buf, buflen, 1, outfile);
	if (errno) {
		ERRS("unable to write to file %s", ofname);
		goto close_out;
	}

	fflush(outfile);

	res = EXIT_SUCCESS;

close_out:
	fclose(outfile);
	if (res != EXIT_SUCCESS)
		unlink(ofname);
close_in:
	fclose(infile);
err_free:
	free(buf);
err:
	return res;
}
