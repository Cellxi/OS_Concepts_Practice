#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
  /*
      argc = argument count
      argv = argument vector
      argc is the number of parameters passed by shell, including the command name itself
      argv[i] is an argument string ended with ASCII Null
      e.g. cp sourcefile targetfile
          in this example, argc = 3, argv[0] = "cp", argv[1] = "sourcefile"
  */
  //  Error handling
  if (argc != 3) {
    write(2, "Usage: cp src dst\n", sizeof("Usage: cp src dst\n"));
    return 1;
  }

  /*  
      Prototype:  int open(const char *pathname, int flags, ...);
      the function accepts variadic arguments just like printf()
      it returns the file descriptor assigned by kernel
      file descriptor usually starts from 3 because 0,1,2 are used to represent std I/O and error

      Prototype:  ssize_t write(int fd, const void *buf, size_t count);
      size_t = size type (an unsigned integer representing the size in bytes)
      ssize_t = signed size type
      fd = file descriptor
  */
  //  Open the source file for read only mode. It locates at the path given by argv[1]
  int src = open(argv[1], O_RDONLY);
  //  Error handling
  if (src < 0) {
    write(2, "open src\n", sizeof("open src\n"));
    return 1;
  }

  /*  
      0644 is a Unix file permission value, which defines who can read or write to the file
      in this case, we open the file located at "argv[2]" for create mode
      if it doesn't exist we'll create it and let everyone read it but only owner can write
      if it exists we'll empty the old one first
  */
  int dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
  //  Error handling
  if (dst < 0) {
    write(2, "open dst\n", sizeof("open dst\n"));
    close(src);
    return 1;
  }

  /*
      Prototype:  ssize_t read(int fd, void *buf, size_t count);
      it returns how many bytes it has read, -1 for errors
  */
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
