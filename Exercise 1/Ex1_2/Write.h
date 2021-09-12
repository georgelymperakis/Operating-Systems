#ifndef WRITE_H
#define WRITE_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
void doWrite(int fd, const char *buff, int len);

void write_file(int fd, const char *infile);

#endif
