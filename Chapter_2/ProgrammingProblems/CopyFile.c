#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
  if (argc != 3) {
    write(2, "Usage: cp src dst\n", sizeof("Usage: cp src dst\n"));
    return 1;
  }

  int src = open(argv[1], O_RDONLY);
  if (src < 0) {
    write(2, "open src\n", sizeof("open src\n"));
    return 1;
  }

  int dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (dst < 0) {
    write(2, "open dst\n", sizeof("open dst\n"));
    close(src);
    return 1;
  }

  char buf[BUF_SIZE];
  ssize_t bytes;
  while ((bytes = read(src, buf, BUF_SIZE)) > 0) {
    ssize_t written = write(dst, buf, bytes);
    if (written != bytes) {
      write(2, "Write\n", sizeof("Write\n"));
      close(src);
      close(dst);
      return 1;
    }
  }

  if (bytes < 0)
    write(2, "read\n", sizeof("read\n"));

  close(src);
  close(dst);
  return 0;
}
