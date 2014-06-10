/* Use ptrace syscall to dump memory from another process.
 * Copyright (C) 2014  吴潍浠
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
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<unistd.h>
#include<sys/ptrace.h>
#include<errno.h>
int main(int argc, char **argv) {
    if(argc != 5) {
        printf("usage:%s <pid> <start> <end> <out_file>\n", argv[0]);
        return -1;
    }
    printf("debug: argv[2]=%s argv[3]=%s\n", argv[2], argv[3]);
    unsigned long pid = strtoul(argv[1], NULL, 10),
                  start = strtoul(argv[2], NULL, 16),
                  end = strtoul(argv[3], NULL, 16),
                  len = 0;
    len = end - start;
    printf("pid=%ld start=%lx end=%lx len=%ld\n", pid, start, end, len);
    const char *filepath = argv[4];
    int fd = open(filepath, O_RDWR|O_CREAT, 00660);
    if(fd == -1) {
        perror("open out_file error");
        return -1;
    }
    if(ftruncate(fd, len) == -1) {
        perror("ftruncate error");
        close(fd);
        unlink(filepath);
        return -1;
    }
    unsigned char *buf = NULL;
    if((buf = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror("mmap error");
        close(fd);
        unlink(filepath);
        return -1;
    }
    if(ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach error");
        munmap(buf, len);
        close(fd);
        unlink(filepath);
        return -1;
    }
    unsigned long n = 0;
    long *ptr = (long *)buf;
    while(n < len) {
        if((*ptr = ptrace(PTRACE_PEEKDATA, pid, (void *)(start + n), NULL)) == -1) {
            if(errno != 0) {
                perror("ptrace peek error");
                munmap(buf, len);
                close(fd);
                unlink(filepath);
                return -1;
            }
        }
        n += sizeof(long);
        ptr++;
    }
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    munmap(buf, len);
    close(fd);
    return 0;
}
