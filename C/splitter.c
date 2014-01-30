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
 *  @file	splitter.c
 *  @brief	File splitter tool. Splits based on file size or number of lines.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUFF	4096
#define ENDL		0x0A	// Linux end of line (LF)

/*
 * Get number of lines of a file.
 */
int get_lines(char* filename) {
	int l = 0;
	int n, i;
	unsigned char* buff = NULL;

	int fd = open(filename, O_RDONLY);
	if (fd == -1) {
		perror("Error opening file");
		return -1;
	}

	buff = (unsigned char*)malloc(MAX_BUFF);
	if (!buff) {
		perror("Error allocating buffer");
		close(fd);
                return -1;
	}

	while ((n = read(fd, buff, MAX_BUFF)) > 0) {
		for (i=0; i<n; i++)
			if (buff[i] == ENDL)
				l++;
	}

	close(fd);
	
	free(buff);

	return l;
}

/*
 * Split the file using low-level calls to open/read/write/close.
 */
void split_v2(char* filename, int lines, int chunks) {
        int lpc, cch, cl;
	int st, i;
        int rd, fid, fod = -1;
	unsigned char* buff = NULL;

	buff = (unsigned char*)malloc(MAX_BUFF);
        if (!buff) {
                perror("Error allocating buffer");
                return;
        }
	
	i = strlen(filename) + 8;     // allow 8 more chars!
        printf("Allocate %d chars for output filename!\n", i);
        char* foname = (char*)malloc(i);
        if (!foname) {
                perror("Error on allocating output filename");
		free(buff);
                return;
        }

	fid = open(filename, O_RDONLY);
	if (fid == -1) {
		perror("Error opening input file");
		free(foname);
		free(buff);
		return;
	}

	lpc = lines/chunks;
        cch = -1;
        cl = lpc + 1;           // such that it will enter output file creation at first iteration
	st = 0;

        while (1) {
		// create new output file if needed
                if (cl >= lpc) {
                        cl = 0;
                        cch++;
                        if (cch == chunks-1) {
                                lpc += lines % chunks;
                        }
			if (cch == chunks)
				break;
                        if (fod != -1)
                                close(fod);
                        sprintf(foname, "%s-%d", filename, cch);
                        fod = open(foname, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                        if (fod == -1) {
                                perror("Error opening output file");
                                close(fid);
                                free(foname);
                                free(buff);
                                return;
                        }
                        printf("\r-> %3.2lf %%", 100*(cch+1)/((float)chunks));
                        fflush(stdout);			
                }
		// read new buffer
		if (st == 0) {
			rd = read(fid, buff, MAX_BUFF);
			if (rd <= 0)
				break;
		}
		// count new lines and write output
		for (i=st; i<rd; i++)
                        if (buff[i] == ENDL) {
                                cl++;
				if (cl >= lpc) {
					// write first part
					write(fod, buff+st, i-st+1);
					st = i+1;
					break; 
				}
			}
		if (cl < lpc) {
			write(fod, buff+st, rd-st);
			st = 0;
		}
        }


	close(fod);
	close(fid);
	free(foname);
	free(buff);
}
	
/*
 * Split the file using higher-level call to fopen/fprintf/fclose.
 */
void split_v1(char* filename, int lines, int chunks) {
	int lpc, cch, cl;
        char* line = NULL;
        size_t len = 0;
        int rd;
	FILE* fi = NULL;
	FILE* fo = NULL;

        len = strlen(filename) + 8;	// allow 8 more chars!
        printf("Allocate %d chars for output filename!\n", len);
        char* foname = (char*)malloc(len); 
        if (!foname) {
                perror("Error on allocating output filename");
                return;
        }

	fi = fopen(filename, "ro");
        if (!fi) {
                perror("Error opening input file");
                free(foname);
		return;
        }

	lpc = lines/chunks;
	cch = -1;
	cl = lpc + 1;		// such that it will enter output file creation at first iteration

	while ((rd = getline(&line, &len, fi)) != -1) {
		if (cl >= lpc) {
                        cl = 0;
                        cch++;
                        if (cch == chunks-1) {
                                lpc += lines % chunks;
                        }
                        if (fo != NULL)
                                fclose(fo);
                        sprintf(foname, "%s-%d", filename, cch);
                        fo = fopen(foname, "w");
                        if (!fo) {
                                perror("Error opening output file");
                                fclose(fi);
                                free(foname);
                                free(line);
                                return;
                        }
                        printf("\r-> %3.2lf %%", 100*(cch+1)/((float)chunks));
                        fflush(stdout);
                }
//              printf("wrinting in %p line %p of size %d\n", fo, line, len);
                fprintf(fo, "%s", line);
                cl++;
        }

        if (fo != NULL)
                fclose(fo);
        fclose(fi);
        free(foname);
}

int main(int argc, char** argv) {
	int chunks = 10;
	int lines = 0;
	char* finame = NULL;

	if (argc < 4) {
		printf("Usage: %s <input_file> <chunks> -b | -l [lines]\n\t-b split file based on size (bytes)\n\t-l split file based on number of lines\n", argv[0]);
		return -1;
	}

	finame = argv[1];
	chunks = atoi(argv[2]);

	switch(argv[3][1]) {
	case 'b':
		
		break;
	case 'l':
		if (argc > 4) {
                	lines = atoi(argv[4]);
        	} else {
                	lines = get_lines(finame);
	                printf("No of lines: %d\n", lines);
        	}

		printf("Splitting file %s with %d lines in %d chunks!\n", finame, lines, chunks);
		split_v2(finame, lines, chunks);
		printf("\nDone.\n");

		break;
	default:
		printf("Unsupported option %s\n", argv[3]);
		return -2;
	}

	return 0;
}
