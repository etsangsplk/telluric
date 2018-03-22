#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/stat.h>

// wrapper over posix nonsense
bool wait_read(file fd, int secs) {
  fd_set r;

  struct timeval delay = {secs, 0};

  FD_SET(fd, &r);

  select(fd + 1, &r, NULL, NULL, &delay);

  if (FD_ISSET(fd, &r)) {
    return true;
  } else {
    return false;
  }
}

void overwrite_file(char* f, void* content, size_t len) {
  unlink(f);

  file o = open(f, O_WRONLY | O_CREAT);

  if (write(o, content, len) != len) {
    // error prop here
  }

  close(o);
}

void read_file(char* f, void* content, size_t* len) {
  file o = open(f, O_RDONLY);

  struct stat s;
  fstat(o, &s);

  read(o, content, s.st_size);
  *len = s.st_size;

  close(o);
}
