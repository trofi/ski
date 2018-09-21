/* fake-xterm
 * Behaves like xterm -S, but logs the output to a file.
 *
 * Original idea by Matthew Chapman <matthewc@cse.unsw.edu.au>
 * This by Ian Wienand <ianw@gelato.unsw.edu.au> (C) 2003
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	int fd = 0;
	int c;
	char buf[BUFSIZ];
	int fileout_flag = 0;
	int fileout_fd = 1;

	opterr = 0;

	while ((c = getopt(argc, argv, "S:o:h")) != -1)
		switch (c) {
		case 'h':
			fprintf(stdout,
				"fake-xterm -o file [arguments to xterm, must include -S]\n");
			return 0;
		case 'S':
			/* the format of -S arg to xterm is 
			   -Sppnn
			   where 
			   pp is the pseduo terminal to use in slave mode.
			   nn is the file descriptor you get your input from
			 */
			fd = atoi(&optarg[2]);
			break;
		case 'o':
			if (fileout_flag)	/* -geo 80x25 matches this too unfortunately */
				continue;
			fileout_flag = 1;
			fileout_fd =
			    open(optarg, O_CREAT | O_RDWR | O_TRUNC,
				 S_IRUSR | S_IWUSR);
			if (fileout_fd == -1) {
				perror("open file");
				return 1;
			}
			break;
		case '?':
			break;
		default:
			abort();
		}

	if (fd <= 0) {
		fprintf(stderr, "fake-xterm: Must specifiy -S\n");
		return 1;
	}

	/* xterms write back their window id, apparently.
	   (undocumented feature) */
	if (write(fd, "0000000\n\r", 9) == -1) {
		perror("writeback");
		return 1;
	}

	while ((c = read(fd, buf, sizeof(buf)))) {
		if (c == -1) {
			perror("xread");
			return 1;
		}

		if (write(fileout_fd, buf, c) == -1) {
			perror("log write");
			return 1;
		}

	}

	close(fileout_fd);
	close(fd);

	exit(0);

}
