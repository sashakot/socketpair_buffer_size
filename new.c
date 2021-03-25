
/*
 *
 * Compilation line: gcc new.c -std=c99 -g
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DATA_SIZE 4

void run(size_t size)
{
	int sfd[2];
	int sndbuf, sbsize = sizeof(sndbuf);

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sfd) == -1) {
		perror("error");
	}

	getsockopt(sfd[0], SOL_SOCKET, SO_SNDBUF, &sndbuf, (socklen_t*)&sbsize);

	printf("Data Size: %zd\n", size);
	char buff[size];   
	size_t wrote=0;
	for (size_t ii=0; ii < 32768; ii++) {
		if ((send(sfd[0], buff, size,MSG_DONTWAIT ) == -1) && (errno == EAGAIN)) {
			wrote = ii;
			break;
		}
	}

	printf("Wrote:     %zd\n", wrote);

	if (wrote != 0) { 
		int bpm = sndbuf/wrote;
		int oh  = bpm - size;

		printf("Bytes/msg: %i\n",  bpm);
		printf("Overhead:  %i\n",  oh);
		printf("\n");
	}

	close(sfd[0]); close(sfd[1]);
}

void print_sock_options()
{
	int sndbuf, sbsize = sizeof(sndbuf);
	int sfd[2];

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sfd) == -1) {
		perror("error");
	}

	getsockopt(sfd[0], SOL_SOCKET, SO_SNDBUF, &sndbuf, (socklen_t*)&sbsize);

	printf("Buffer Size: %i\n\n", sndbuf);
	close(sfd[0]); close(sfd[1]);
}

int main(int argc, char **argv)
{
	int ii, maxsize;

	if (argc < 2) {
		fprintf(stderr, "wrong number of arguments\n");
		exit(1);
	}

	maxsize = atoi(argv[1]);
	if (maxsize <= 0) {
		fprintf(stderr, "maxsize value is wrong: %d\n", maxsize);
		exit(1);
	}

	for (ii=4; ii <= maxsize; ii *= 2) {
		run(ii);
	}
}
