/*  xor.c - xor encryption / decryption tool
 *
 *  Copyright (C) 2008-2026 Jakob Flierl <jakob.flierl@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#define RtlGenRandom SystemFunction036
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#define CHUNK_SIZE (1024 * 1024)

void usage(void)
{
	fprintf(stderr,
		"xor - XOR encryption/decryption tool\n"
		"\n"
		"Usage:\n"
		"  xor -e <keyfile> -i <input> -o <output>  # encrypt\n"
		"  xor -d <keyfile> -i <input> -o <output>  # decrypt\n"
		"  xor -h | --help                            # show this help\n"
		"\n"
		"Options:\n"
		"  -e, --encrypt <keyfile>  Encrypt mode; generate keyfile\n"
		"  -d, --decrypt <keyfile>  Decrypt mode; use existing keyfile\n"
		"  -i, --input <file>       Input file\n"
		"  -o, --output <file>      Output file\n"
		"  -h, --help               Show this help message\n"
		"\n"
		"Examples:\n"
		"  xor -e my.key -i secret.txt -o secret.enc  # encrypt\n"
		"  xor -d my.key -i secret.enc -o secret.txt  # decrypt\n"
	);
	exit(EXIT_SUCCESS);
}

static void usage_error(void)
{
	fprintf(stderr,
		"Usage: xor -e|-d <keyfile> -i <input> -o <output>\n"
		"       xor -h | --help\n"
		"\n"
		"Use -h or --help for detailed help.\n"
	);
	exit(EXIT_FAILURE);
}

#ifdef _WIN32
#include <windows.h>
#define RtlGenRandom SystemFunction036
BOOLEAN NTAPI RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);

static int win32_random_buf(void *buf, size_t len)
{
	size_t done = 0;
	unsigned char *p = (unsigned char *)buf;
	while (done < len) {
		ULONG chunk = (len - done > 0x7FFFFFFFUL) ? 0x7FFFFFFFUL : (ULONG)(len - done);
		if (!RtlGenRandom(p + done, chunk))
			return -1;
		done += chunk;
	}
	return 0;
}
#endif

int main(int argc, char **argv)
{
	struct stat sb_input;
	FILE *finput = NULL, *frandom = NULL, *fkey = NULL, *foutput = NULL;
	int do_encrypt = 0, do_decrypt = 0;
	char *keyfile = NULL, *input = NULL, *output = NULL;
	int ret = EXIT_SUCCESS;
	uint64_t file_size;
	unsigned char *buf = NULL;
#ifdef _WIN32
	unsigned char randbuf[CHUNK_SIZE];
	size_t randbuf_used = CHUNK_SIZE;
#endif
	int i = 1;
	while (i < argc) {
#define OPTION_SET(longopt,shortopt) (strcmp(argv[i], longopt)==0 || strcmp(argv[i], shortopt)==0)
		if (OPTION_SET("--encrypt", "-e")) {
			if (++i >= argc) {
				fprintf(stderr, "Missing argument for %s\n", argv[i-1]);
				ret = EXIT_FAILURE;
				goto cleanup;
			}
			do_encrypt = 1;
			keyfile = argv[i];
		} else if (OPTION_SET("--decrypt", "-d")) {
			if (++i >= argc) {
				fprintf(stderr, "Missing argument for %s\n", argv[i-1]);
				ret = EXIT_FAILURE;
				goto cleanup;
			}
			do_decrypt = 1;
			keyfile = argv[i];
		} else if (OPTION_SET("--input", "-i")) {
			if (++i >= argc) {
				fprintf(stderr, "Missing argument for %s\n", argv[i-1]);
				ret = EXIT_FAILURE;
				goto cleanup;
			}
			input = argv[i];
		} else if (OPTION_SET("--output", "-o")) {
			if (++i >= argc) {
				fprintf(stderr, "Missing argument for %s\n", argv[i-1]);
				ret = EXIT_FAILURE;
				goto cleanup;
			}
			output = argv[i];
		} else if (OPTION_SET("--help", "-h")) {
			usage();
		} else {
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
			usage_error();
		}
		i++;
	}

	if (argc == 1) {
		usage_error();
	}

	if (input == NULL || output == NULL || do_encrypt == do_decrypt) {
		usage_error();
	}

	if (stat(input, &sb_input) == -1) {
		fprintf(stderr, "error: cannot stat '%s'. Exiting.\n", input);
		ret = EXIT_FAILURE;
		goto cleanup;
	}
	file_size = (uint64_t)sb_input.st_size;

	buf = malloc(CHUNK_SIZE);
	if (buf == NULL) {
		perror("malloc");
		ret = EXIT_FAILURE;
		goto cleanup;
	}

	if (do_encrypt) {
#ifndef _WIN32
		if (!(frandom = fopen("/dev/urandom", "rb"))) {
			perror("fopen(/dev/urandom)");
			ret = EXIT_FAILURE;
			goto cleanup;
		}
#endif

		if (!(fkey = fopen(keyfile, "wb"))) {
			perror("fopen(keyfile)");
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (!(finput = fopen(input, "rb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", input);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (!(foutput = fopen(output, "wb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", output);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		uint64_t remaining = file_size;
		while (remaining > 0) {
			size_t to_read = (remaining > CHUNK_SIZE) ? CHUNK_SIZE : (size_t)remaining;
			size_t n = fread(buf, 1, to_read, finput);
			if (n == 0) {
				fprintf(stderr, "error reading from input file\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			unsigned char keybuf[CHUNK_SIZE];
#ifdef _WIN32
			if (randbuf_used + n > sizeof(randbuf)) {
				if (win32_random_buf(randbuf, sizeof(randbuf)) != 0) {
					fprintf(stderr, "error: getting random bytes. Exiting.\n");
					ret = EXIT_FAILURE;
					goto cleanup;
				}
				randbuf_used = 0;
			}
			memcpy(keybuf, randbuf + randbuf_used, n);
			randbuf_used += n;
#else
			if (fread(keybuf, 1, n, frandom) != n) {
				fprintf(stderr, "error: reading from /dev/urandom. Exiting.\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}
#endif

			if (fwrite(keybuf, 1, n, fkey) != n) {
				fprintf(stderr, "error: writing to key file. Exiting.\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			for (size_t j = 0; j < n; j++)
				buf[j] ^= keybuf[j];

			if (fwrite(buf, 1, n, foutput) != n) {
				fprintf(stderr, "error: writing to output file. Exiting.\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			remaining -= n;
		}
	} else if (do_decrypt) {
		if (!(fkey = fopen(keyfile, "rb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", keyfile);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (!(finput = fopen(input, "rb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", input);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (!(foutput = fopen(output, "wb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", output);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		uint64_t remaining = file_size;
		while (remaining > 0) {
			size_t to_read = (remaining > CHUNK_SIZE) ? CHUNK_SIZE : (size_t)remaining;
			size_t n = fread(buf, 1, to_read, finput);
			if (n == 0) {
				fprintf(stderr, "error reading from input file\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			unsigned char keybuf[CHUNK_SIZE];
			if (fread(keybuf, 1, n, fkey) != n) {
				fprintf(stderr, "error: reading from key file '%s'. Exiting.\n", keyfile);
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			for (size_t j = 0; j < n; j++)
				buf[j] ^= keybuf[j];

			if (fwrite(buf, 1, n, foutput) != n) {
				fprintf(stderr, "error: writing to output file. Exiting.\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			remaining -= n;
	}
} else {
	usage_error();
}

cleanup:
	if (buf) {
		free(buf);
	}
	if (finput) {
		fclose(finput);
	}
	if (frandom) {
		fclose(frandom);
	}
	if (fkey) {
		fclose(fkey);
	}
	if (foutput) {
		fclose(foutput);
	}

	return ret;
}
