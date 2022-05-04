#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

#define hexit      \
  do {             \
    displayHelp(); \
    return -1;     \
  } while (0);

#define ez(x) do{ if ((x) == NULL) exit(1); } while(0);

typedef struct {
  volatile _Bool read;
  void *data;
} node;

typedef struct {
  node *buf;
  long size;
} thread_args;

void displayHelp() { printf("Usage: buffer [buffer_size] -pthread"); }

int main(int argc, char **argv) {
  if (argc > 3) hexit;
  long size = 100;
  if (argc == 3) size = strtol(argv[1], NULL, 10);
  if (size == LONG_MAX || size == LONG_MIN) hexit;

  srand(time(NULL)); // seed
  node *buffer = malloc (sizeof(node) * size);
  // ez(buffer);

  buffer[0].read = 1;
  // deploy threads
  pthread_t reader, writer;
  thread_args x = {.buf = buffer, .size = size};
  pthread_create(&writer,NULL,write,&x);
  pthread_create(&reader,NULL,read,&x);
}

void *read(void *a){
  thread_args arg = *(thread_args*)a;
  long size = arg.size;
  node *buf = arg.buf;

  long i = 0;
  while(1){
    if (buf[i].read) {
      struct timespec wait = {.tv_nsec = (100 + (rand() % 100)) * 1000000, .tv_sec = 0};
      printf("READER -> Buffer empty. Trying again in %d ms...\n", wait.tv_nsec / 1000000);
      nanosleep(wait);
    }
    int *val = buf[i].data;
    printf("READER -> Read %d in buf[%ld]\n", *val, i);
    free(buf[i].data);
    buf[i].data = NULL;
    buf->read = 0;
    i = i+1 % size;
    struct timespec wait = {.tv_nsec = (100 + (rand() % 50)) * 1000000, .tv_sec = 0};
    nanosleep(wait);
  }

}

void *write(void *a){
  thread_args arg = *(thread_args*)a;
  long size = arg.size;
  node *buf = arg.buf;

  long i = 0;
  while(1){
    if (!buf[i].read) {
      struct timespec wait = {.tv_nsec = (50 + (rand() % 200)) * 1000000, .tv_sec = 0};
      printf("WRITER -> Buffer full. Trying again in %d ms...\n", wait.tv_nsec / 1000000);
      nanosleep(wait);
    }
    int *val = malloc (sizeof(int));
    ez(val);
    *val = rand() % 1000;
    buf[i].data = val;
    //printf("WRITER -> Wrote %d in buf[%ld]\n", *val, i);
    buf->read = 0;
    i = i+1 % size;
    struct timespec wait = {.tv_nsec = (50 + (rand() % 100)) * 1000000, .tv_sec = 0};
    nanosleep(wait);
  }
}
