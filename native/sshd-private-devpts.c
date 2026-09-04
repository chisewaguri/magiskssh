#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <unistd.h>

static void
die(const char *operation)
{
	perror(operation);
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s command [argument ...]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (unshare(CLONE_NEWNS) == -1)
		die("unshare");
	if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1)
		die("mount private root");
	if (mount("devpts", "/dev/pts", "devpts", MS_NOSUID | MS_NOEXEC,
	    "newinstance,ptmxmode=0666,mode=0620,gid=2000") == -1)
		die("mount private devpts");

	execv(argv[1], &argv[1]);
	die("exec");
}
