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

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void usage(int argc, char **argv)
{
	(void)argc;
	fprintf(stderr, "usage: %s ...\n", argv[0]);
	fprintf(stderr,
		"  to encrypt, run: '%s -e abc.key -i abc.txt -o abc.enc'\n",
		argv[0]);
	fprintf(stderr,
		"  to decrypt, run: '%s -d abc.key -i abc.enc -o abc.txt'\n",
		argv[0]);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int finput = -1;
	struct stat sb_input;
	FILE *frandom = NULL, *fk = NULL, *foutput = NULL;
	int do_encrypt = 0, do_decrypt = 0;
	char *keyfile = NULL, *input = NULL, *output = NULL;
	char *fin = NULL;
	int ret = EXIT_SUCCESS;

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
			usage(argc, argv);
		} else {
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
			ret = EXIT_FAILURE;
			goto cleanup;
		}
		i++;
	}

	if (input == NULL) {
		fprintf(stderr, "missing --input option. Exiting.\n");
		ret = EXIT_FAILURE;
		goto cleanup;
	}

	if (output == NULL) {
		fprintf(stderr, "missing --output option. Exiting.\n");
		ret = EXIT_FAILURE;
		goto cleanup;
	}

	if (do_encrypt == do_decrypt) {
		fprintf(stderr, "must specify either --encrypt or --decrypt, not both\n");
		ret = EXIT_FAILURE;
		goto cleanup;
	}

	if (do_encrypt) {
		if (!(frandom = fopen("/dev/urandom", "rb"))) {
			perror("fopen(/dev/urandom)");
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (!(fk = fopen(keyfile, "wb"))) {
			perror("fopen(keyfile)");
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if ((finput = open(input, O_RDONLY)) == -1) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", input);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (fstat(finput, &sb_input) == -1) {
			perror("fstat");
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (sb_input.st_size == 0) {
			if (!(foutput = fopen(output, "wb"))) {
				fprintf(stderr, "error opening '%s'. Exiting.\n", output);
				ret = EXIT_FAILURE;
				goto cleanup;
			}
			goto cleanup;
		}

		fin = mmap(NULL, sb_input.st_size, PROT_READ, MAP_PRIVATE, finput, 0);
		if (fin == MAP_FAILED) {
			perror("mmap");
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (!(foutput = fopen(output, "wb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", output);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		for (__off_t i = 0; i < sb_input.st_size; ++i) {
			char c, k;

			c = fin[i];

			k = getc(frandom);
			if (feof(frandom)) {
				fprintf(stderr, "error: reading from /dev/urandom. Exiting.\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			if (fputc(k, fk) == EOF) {
				fprintf(stderr, "error: writing to key file. Exiting.\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			c ^= k;
			if (fputc(c, foutput) == EOF) {
				fprintf(stderr, "error: writing to output file. Exiting.\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}
		}
	} else if (do_decrypt) {
		if (!(fk = fopen(keyfile, "rb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", keyfile);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if ((finput = open(input, O_RDONLY)) == -1) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", input);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (fstat(finput, &sb_input) == -1) {
			perror("fstat");
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (sb_input.st_size == 0) {
			if (!(foutput = fopen(output, "wb"))) {
				fprintf(stderr, "error opening '%s'. Exiting.\n", output);
				ret = EXIT_FAILURE;
				goto cleanup;
			}
			goto cleanup;
		}

		fin = mmap(NULL, sb_input.st_size, PROT_READ, MAP_PRIVATE, finput, 0);
		if (fin == MAP_FAILED) {
			perror("mmap");
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		if (!(foutput = fopen(output, "wb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n", output);
			ret = EXIT_FAILURE;
			goto cleanup;
		}

		for (__off_t i = 0; i < sb_input.st_size; ++i) {
			char c, k;

			c = fin[i];

			k = getc(fk);
			if (feof(fk)) {
				fprintf(stderr, "error: reading from '%s'. Exiting.\n", keyfile);
				ret = EXIT_FAILURE;
				goto cleanup;
			}

			c ^= k;
			if (fputc(c, foutput) == EOF) {
				fprintf(stderr, "error: writing to output file. Exiting.\n");
				ret = EXIT_FAILURE;
				goto cleanup;
			}
		}
	} else {
		usage(argc, argv);
	}

cleanup:
	if (fin && fin != MAP_FAILED) {
		munmap(fin, sb_input.st_size);
	}
	if (finput != -1) {
		close(finput);
	}
	if (frandom) {
		fclose(frandom);
	}
	if (fk) {
		fclose(fk);
	}
	if (foutput) {
		fclose(foutput);
	}

	return ret;
}
