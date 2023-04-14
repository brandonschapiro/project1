#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Sources
// https://stackoverflow.com/questions/15883568/reading-from-stdin - Used to
// understand how to read / write files with a buffer https://linux.die.net/ -
// Used to understand read(2) write(2) and warn(3)
// https://piazza.com/class/lfua1864d8r1uv/post/41 - Used for handling write
// errors
int main(int argc, char *argv[]) {
  // Bool to store whether one or more args were an invalid file name
  bool failure = false;
  // Create buffer for reading from files
  const int BUFFER_SIZE = 750000;
  char buffer[BUFFER_SIZE];
  int file;

  // If there are no args submitted, just reads from standard input and writes
  // to standard output
  if (argc == 1) {
    while (true) {
      int count = read(STDIN_FILENO, buffer, sizeof(buffer));
      if (count == 0) {
        break;
      } else {
        int w = write(STDOUT_FILENO, buffer, count);
        if (w == -1) {
          err(1, "write error: Broken pipe");
        }
      }
    }
  }

  // If there is at least one arg, it will go through them and read from
  // standard input if it's a '-' or read from the specified text file if it
  // exists
  for (int i = 1; i < argc; i++) {
    // If the arg is "-", the program reads from standard input then writes it
    // to standard output
    if (strcmp(argv[i], "-") == 0) {
      while (true) {
        int count = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (count == 0) {
          break;
        } else {
          int w = write(STDOUT_FILENO, buffer, count);
          if (w == -1) {
            err(1, "write error: Broken pipe");
          }
        }
      }
    }
    // If the arg isnt "-", the program will try to open the file
    else {
      file = open(argv[i], O_RDONLY);
      // If there is no file with the given arg name, it spits out an error and
      // failure is set to true so the correct exit_status can be returned
      if (file == -1) {
        failure = true;
        warn("%s", argv[i]);
      } else {
        // If the file is opened, keep reading from the file and writing to
        // standard output until the file has been fully read
        while (true) {
          int readLen = read(file, buffer, sizeof(buffer));
          if (readLen == 0) {
            break;
          } else {
            int w = write(STDOUT_FILENO, buffer, readLen);
            if (w == -1) {
              err(1, "write error: Broken pipe");
            }
          }
        }
        // Close file
        close(file);
      }
    }
  }

  // If any files fail to open, failure is set to true and will return the
  // exit_failure status code, if there are no failures then it will return
  // exit_success
  return failure ? EXIT_FAILURE : EXIT_SUCCESS;
}
