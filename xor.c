/*  xor.c - xor encryption / decryption tool
 *
 *  Copyright (C) 2008-2021 Jakob Flierl <jakob.flierl@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void usage(int argc, char **argv)
{
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
	FILE *frandom = 0, *fk = 0, *finput = 0, *foutput = 0;
	int do_encrypt = 0, do_decrypt = 0;
	char *keyfile = NULL, *input = NULL, *output = NULL;

	int i = 0;
	while (++i < argc) {
#define OPTION_SET(longopt,shortopt) (strcmp(argv[i], longopt)==0 || strcmp(argv[i], shortopt)==0)
#define OPTION_VALUE ((i+1 < argc)?(argv[i+1]):(NULL))
#define OPTION_VALUE_PROCESSED (i++)
		if (OPTION_SET("--encrypt", "-e")) {
			do_encrypt = 1;
			keyfile = argv[++i];
		} else if (OPTION_SET("--decrypt", "-d")) {
			do_decrypt = 1;
			keyfile = argv[++i];
		} else if (OPTION_SET("--input", "-i")) {
			input = argv[++i];
		} else if (OPTION_SET("--output", "-o")) {
			output = argv[++i];
		} else if (OPTION_SET("--help", "-h")) {
			usage(argc, argv);
		} else {
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
			return EXIT_FAILURE;
		}
	}

	if (input == NULL) {
		fprintf(stderr, "missing --input option. Exiting.\n");
		exit(EXIT_FAILURE);
	}

	if (output == NULL) {
		fprintf(stderr, "missing --output option. Exiting.\n");
		exit(EXIT_FAILURE);
	}

	if (do_encrypt) {
		if (!(frandom = fopen("/dev/urandom", "rb"))) {
			perror("fopen(/dev/urandom)");
			return EXIT_FAILURE;
		}

		if (!(fk = fopen(keyfile, "wb"))) {
			perror("fopen(keyfile)");
			return EXIT_FAILURE;
		}

		if (!(finput = fopen(input, "rb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n",
				input);
			return EXIT_FAILURE;
		}

		if (!(foutput = fopen(output, "wb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n",
				output);
			return EXIT_FAILURE;
		}

		for (;;) {
			char c, k;

			c = getc(finput);
			if (feof(finput)) {
				break;
			}

			k = getc(frandom);
			if (feof(frandom)) {
				fprintf(stderr,
					"error: reading from /dev/urandom. Exiting.\n");
				return EXIT_FAILURE;
			}

			fputc(k, fk);

			c ^= k;
			fputc(c, foutput);
		}

		if (fclose(fk)) {
			fprintf(stderr, "error: closing '%s'. Exiting.\n",
				keyfile);
			return EXIT_FAILURE;
		}

		if (fclose(frandom)) {
			fprintf(stderr,
				"error: closing '/dev/urandom'. Exiting.\n");
			return EXIT_FAILURE;
		}

		if (fclose(finput)) {
			fprintf(stderr, "error: closing '%s'. Exiting.\n",
				input);
			return EXIT_FAILURE;
		}

		if (fclose(foutput)) {
			fprintf(stderr, "error: closing '%s'. Exiting.\n",
				output);
			return EXIT_FAILURE;
		}
	} else if (do_decrypt) {
		if (!(fk = fopen(keyfile, "rb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n",
				keyfile);
			return EXIT_FAILURE;
		}

		if (!(finput = fopen(input, "rb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n",
				input);
			return EXIT_FAILURE;
		}

		if (!(foutput = fopen(output, "wb"))) {
			fprintf(stderr, "error opening '%s'. Exiting.\n",
				output);
			return EXIT_FAILURE;
		}

		for (;;) {
			char c, k;

			c = getc(finput);
			if (feof(finput)) {
				break;
			}

			k = getc(fk);
			if (feof(fk)) {
				fprintf(stderr,
					"error: reading from '%s'. Exiting.\n",
					input);
				return EXIT_FAILURE;
			}

			fputc(k, fk);

			c ^= k;
			fputc(c, foutput);
		}

		if (fclose(fk)) {
			fprintf(stderr, "error: closing '%s'. Exiting.\n",
				keyfile);
			return EXIT_FAILURE;
		}

		if (fclose(finput)) {
			fprintf(stderr, "error: closing '%s'. Exiting.\n",
				input);
			return EXIT_FAILURE;
		}

		if (fclose(foutput)) {
			fprintf(stderr, "error: closing '%s'. Exiting.\n",
				output);
			return EXIT_FAILURE;
		}
	} else {
		usage(argc, argv);
	}

	return 0;
}
