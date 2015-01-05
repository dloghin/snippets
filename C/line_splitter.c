/* Copyright 2014 Dumi Loghin <dumiloghin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** 
 *  @file	line_splitter.c
 *  @brief	Splits lines exceeding <max-line-size> size.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFF	(1<<22)

int main(int argc, char** argv) {
	char buff[MAX_BUFF];
	char c, *ps, *pe;
	int split, nsplit = 0;
	char outfn[128];
	FILE *fin, *fout;
	long len, maxl, flen, clen, progress, old_progress;
	int ret = 0;

	if (argc < 3) {
		printf("Usage: %s <file> <max-line-size>\n", argv[0]);
		return -1;
	}

	maxl = strtol(argv[2], NULL, 10);
	if (maxl < 1) {
		printf("Invalid <max-line-size> %s\n", argv[2]); 
		ret = -1;
		goto lbl_close;
	}
	
	fin = fopen(argv[1], "rt");
	if (!fin) {
		perror("Error opening input file");
		ret = -1;
		goto lbl_close;
	}

	sprintf(outfn, "lines-%s", argv[1]);	
	fout = fopen(outfn, "wt");
        if (!fout) {
                perror("Error opening output file");
                ret = -1;
		goto lbl_close;
        }

	fseek(fin, 0L, SEEK_END);
	flen = ftell(fin);
	fseek(fin, 0L, SEEK_SET);
	clen = 0;
	old_progress = -5;

	printf("Start splitting file %s (size %ld) into lines smaller then %ld characters...\n", argv[1], flen, maxl);

	while (!feof(fin)) {
		fgets(buff, MAX_BUFF, fin);
		len = strlen(buff);
		if (len >= MAX_BUFF-1) {
			printf("Read buffer too small (%d kB)!", MAX_BUFF/1024);
		}
		ps = buff;
		pe = ps + (maxl-2);
		split = 0;

		clen += len;
		progress = (long)(100.0 * clen / (double)flen);
		if (progress - old_progress > 4) {
			printf("Split %ld%%\r", progress);
			fflush(stdout);
			old_progress = progress;
		}

		split = 0;
		while (len > maxl) {
			c = *pe;
			*pe = '\0';
			fprintf(fout, "%s\n", ps);
			*pe = c;
			ps = pe;
			pe = ps + (maxl-2);
			len -= (maxl-1);
			split++;
		}

		fprintf(fout, "%s", ps);
		nsplit += split;
	}

	printf("New lines added: %d\n", nsplit);

lbl_close:
	fclose(fin);
	fclose(fout);

	return ret;
}
