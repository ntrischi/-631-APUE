/* Neal Trischitta
 * tcp.c - CS 631 Stevens Institute of Technology
 *
 * The tcp utility copies the contents of the source to target. If target is a
 * directory,tcp  will copy source into this directory.
 * Unlike tcp, tcpm uses mmap(2) and memcpy(2) instead of read(2) and write(2)
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <fcntl.h>           /* Definition of AT_* constants */

int main(int argc, char** argv)
{
	int source_fd, target_fd;
	char *source;
	char *target;
	struct stat st_source;
	struct stat st_target;
	
	char *source_absolute_path;
	char *target_absolute_path;

	char buf_realpath_src[PATH_MAX];
	char buf_realpath_tar[PATH_MAX];
	
	size_t pagesize;
	size_t remain_blocks;
	size_t wrote=0;

	if(argc != 3)
	{
		printf("USAGE: ./tcp SOURCE TARGET\n");
		exit(EXIT_FAILURE);
	}
	if(argv[1] == NULL || argv[2] == NULL)
	{
		perror("NULL input was given\n");
		exit(EXIT_FAILURE);
	}
	if((stat(argv[1],&st_source) == -1) || (!S_ISREG(st_source.st_mode)))
	{
		perror("source file is not a regular file\n");
		exit(EXIT_FAILURE);
	}
	if((stat(argv[2],&st_target) == -1) ||
		(st_source.st_ino == st_target.st_ino))
	{
		perror("target file not found or is equivalent to source file");
		exit(EXIT_FAILURE);
	}
	if((source_fd = open(argv[1], O_RDONLY)) == -1)
	{
		perror("could not open source file\n");
		exit(EXIT_FAILURE);
	}
	if((target_fd=open(argv[2], O_DIRECTORY)) >= 0)
	{
		if((source_absolute_path=realpath(argv[1],buf_realpath_src)) == NULL)
		{
			perror("couldn't resolve working directory\n");
			exit(EXIT_FAILURE);
		}
		if((target_absolute_path=realpath(argv[2],buf_realpath_tar)) == NULL)
		{
			perror("couldn't resolve target directory\n");
			exit(EXIT_FAILURE);
		}
		if(strcmp(dirname(source_absolute_path),target_absolute_path) == 0)
		{
			exit(EXIT_SUCCESS);
		}
		if((target_fd = openat(target_fd, basename(argv[1]),
			O_RDWR | O_CREAT | O_TRUNC, st_source.st_mode)) == -1)
		{
			perror("could not open target directory\n");
			exit(EXIT_FAILURE);
		}
	}
	else if((target_fd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,
		st_source.st_mode)) == -1)
	{
			perror("could not open target file\n");
			exit(EXIT_FAILURE);
	}
	/** MEMCPY **/
	pagesize=getpagesize();
	if(ftruncate(target_fd,st_source.st_size) == -1)
	{
			perror("target could not be resized\n");
			exit(EXIT_FAILURE);
	}
	while(wrote <(size_t)st_source.st_size) 
	{
		if(pagesize < (st_source.st_size - wrote))
		{
			remain_blocks=pagesize;
		}
		else
		{
			remain_blocks=(st_source.st_size - wrote);
		}
	    if((source = mmap(NULL, remain_blocks, PROT_READ, MAP_SHARED,
	          source_fd, wrote)) == MAP_FAILED)
		{
				perror("source: mmap failed.");
				exit(EXIT_FAILURE);
			
		}
		if((target = mmap(NULL, remain_blocks, PROT_WRITE, MAP_SHARED,
		     target_fd, wrote)) == MAP_FAILED)
		{
				perror("source: mmap failed.");
				exit(EXIT_FAILURE);

		}
		memcpy(target, source, remain_blocks);
		if(munmap(source, remain_blocks) == -1)
		{
				perror("source: munmap failed.");
				exit(EXIT_FAILURE);

		}
	   	if (munmap(target, remain_blocks) == -1)
		{
				perror("target: munmap failed.");
				exit(EXIT_FAILURE);

		}
		wrote += remain_blocks;
	}

	if(close(source_fd) == -1)
	{
		perror("Could not close source file");
		exit(EXIT_FAILURE);
	}
	if(close(target_fd) == -1)
	{
		perror("Could not close target file");
		exit(EXIT_FAILURE);
	}
	return 0;
}
