/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPLv2.
  See the file COPYING.
*/

/** @file
 *
 * minimal example filesystem using high-level API
 *
 * Compile with:
 *
 *     gcc -Wall hello.c `pkg-config fuse3 --cflags --libs` -o hello
 *
 * ## Source code ##
 * \include hello.c
 */


#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>

/*
 * Command line options
 *
 * We can't set default values for the char* fields here because
 * fuse_opt_parse would attempt to free() them when the user specifies
 * different values on the command line.
 */
struct options {
    const char *filename;
	const char *contents;
} options;

void *hello_init(struct fuse_conn_info *conn,
			struct fuse_config *cfg)
{
	cfg->kernel_cache = 1;
	return NULL;
}

int hello_getattr(const char *path, struct stat *stbuf,
			 struct fuse_file_info *fi)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) { // "/"
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path+1, options.filename) == 0) { // "/filename"
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(options.contents);
	} else {
		res = -ENOENT;
    }
	return res;
}

int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)
{
	if (strcmp(path, "/") != 0) {
		return -ENOENT;
    }

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);
	filler(buf, options.filename, NULL, 0, 0);

	return 0;
}

int hello_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path+1, options.filename) != 0) {
		return -ENOENT;
    }
	if ((fi->flags & O_ACCMODE) != O_RDONLY) {
		return -EACCES;
    }
	return 0;
}

int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	if(strcmp(path+1, options.filename) != 0) {
		return -ENOENT;
    }

	len = strlen(options.contents);
	if (offset < len) {
		if (offset + size > len) {
			size = len - offset;
        }
		memcpy(buf, options.contents + offset, size);
	} else {
		size = 0;
    }
	return size;
}

const struct fuse_operations hello_oper = {
	.init       = hello_init,
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
};

int main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	options.filename = strdup("hello");
	options.contents = strdup("hello, world!\n");

	if (fuse_opt_parse(&args, &options, NULL, NULL) == -1) {
		return 1;
    }

	ret = fuse_main(args.argc, args.argv, &hello_oper, NULL);
	fuse_opt_free_args(&args);
	return ret;
}
