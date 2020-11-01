#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "include/liburing.h"

#define N 4
#define COPY_SIZE (1L << 18)

off_t file_size(int fd) {
    struct stat st;
    if(fstat(fd, &st) < 0) {
        return 0;
    }
    if(S_ISREG(st.st_mode)) {
        return st.st_size;
    } else if(S_ISBLK(st.st_mode)) { 
        // block has const size, so we can find it, using ioctl
        off_t size;
        ioctl(fd, BLKGETSIZE64, &size);
        return size;
    }
    return 0;
}

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("Args error\n");
        return -1;
    }
    // argv[1] <- file that we are copying
    // argv[2] <- file to which we copy the first one

    int fd_out = 0;
    int fd_in = 0;
    if((fd_in = open(argv[1], O_RDONLY)) < 0) {
        perror("open first file error");
        return -1;
    }
    if((fd_out = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0) {
        perror("open second file error");
        return -1;
    }

    off_t fsize = 0;
    if((fsize = file_size(fd_in)) == 0) {
        perror("file_size error\n");
        return -1;
    }

    struct io_uring ring;
    if(io_uring_queue_init(N, &ring, 0) < 0) {
        printf("io_uring_queue_init error\n");
        return -1;
    }

    struct io_uring_cqe* cqe;
    struct io_uring_sqe* sqe; 
    sqe = io_uring_get_sqe(&ring);
    // TODO cpy (^_^)

    

    close(fd_out);
    close(fd_in);
    io_uring_queue_exit(&ring);
    return 0;
}
