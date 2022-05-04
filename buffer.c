#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define hexit      \
  do {             \
    displayHelp(); \
    return -1;     \
  } while (0);

typedef struct {
  //  volatile _Bool read;
  void *data;
} node;

typedef struct {
  node *buf;
  long size;
} thread_args;

void displayHelp() {
  printf("Usage: buffer [buffer_size]\n");
  exit(1);
}
void *writer(void *a);
void *reader(void *a);
long roundUp2Power(long n);


int main(int argc, char **argv) {
  unsigned long size = 128;
  pthread_t reader_t, writer_t;
  node *buffer;

  if (argc > 2) displayHelp();

  if (argc == 2) size = strtol(argv[1], NULL, 10);

  if (size > LONG_MAX) size = LONG_MAX;
  size = roundUp2Power(size);

  srand(time(NULL));  // seed

  if ((buffer = calloc(size, sizeof(node))) == NULL) return (-1);

  printf("Spawning threads...\n");

  thread_args x = {.buf = buffer, .size = size};
  pthread_create(&writer_t, NULL, writer, &x);
  pthread_create(&reader_t, NULL, reader, &x);

  pthread_join(reader_t, NULL);
  pthread_join(writer_t, NULL);
}

void *reader(void *a) {
  thread_args arg = *(thread_args *)a;
  long size = arg.size;
  node *buf = arg.buf;
  long i = 0;
  while (1) {
    int *val;

    while (buf[i].data == NULL) {  // reader reached writer position
      printf("READER -> Buffer empty\n");
      usleep(rand() % 10);
    }

    val = buf[i].data;
    printf("READER -> Read %d in buf[%ld]\n", *val, i);

    free(buf[i].data);
    buf[i].data =
        NULL;  // Important! Writer now knows the node has been emptied

    i = (i + 1) & size;
    usleep(1);
  }
}

void *writer(void *a) {
  thread_args arg = *(thread_args *)a;
  long size = arg.size;
  node *buf = arg.buf;
  long i = 0;

  while (1) {
    int *val;

    while (buf[i].data != NULL) {  // reader is ahead of writer
      printf("WRITER -> Buffer full\n");
      usleep(rand() % 10);  // wait until the reader consumes the buffer
    }

    val = malloc(sizeof(int));

    if (val != NULL) {
      *val = rand() % 1000;
      buf[i].data = val;
      printf("WRITER -> Wrote %d in buf[%ld]\n", *val, i);

      i = (i + 1) & size;
    }

    usleep(1);
  }
}

/**
 * 
 * @note n must be > 0 && < LONG_MAX
 * @return 'n' nearest greater power of 2 or lesser if n == LONG_MAX
 */
long roundUp2Power(long n) {
  if (n <= 0)
    return 0;
  n = n - 1;

  // unset rightmost bit until only one is left
  while (n & n - 1) {
    n = n & n - 1;
  }

  // n is a power of two but lesser than the given parameter
  // left shift to get next power of 2 if not overflow
  return n << 1 < 0 ? n : n << 1;
}