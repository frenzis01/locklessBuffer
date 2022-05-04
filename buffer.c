#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>

#define hexit					\
  do {						\
    displayHelp();				\
    return -1;					\
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

int main(int argc, char **argv) {  
  unsigned long size = 100;
  pthread_t reader_t, writer_t;
  node *buffer;
  
  if (argc > 2)
    displayHelp();
  
  if (argc == 2) size = strtol(argv[1], NULL, 10);

  if (size > LONG_MAX) size = LONG_MAX;

  srand(time(NULL)); // seed

  if((buffer = calloc (size, sizeof(node))) == NULL)
    return(-1);

  printf("Spawning threads...\n");

  thread_args x = {.buf = buffer, .size = size};
  pthread_create(&writer_t,NULL,writer,&x);
  pthread_create(&reader_t,NULL,reader,&x);
    
  pthread_join(reader_t, NULL);
  pthread_join(writer_t, NULL);  
}

void *reader(void *a) {
  thread_args arg = *(thread_args*)a;
  long size = arg.size;
  node *buf = arg.buf;
  long i = 0;
    
  while(1) {
    int *val;
      
    while (buf[i].data == NULL) {
      printf("READER -> Buffer empty\n");
      usleep(rand() % 10);
    }

    val = buf[i].data;
    printf("READER -> Read %d in buf[%ld]\n", *val, i);

    free(buf[i].data);
    buf[i].data = NULL;

    i = (i+1) % size;
    usleep(1);
  }
}

void *writer(void *a) {
  thread_args arg = *(thread_args*)a;
  long size = arg.size;
  node *buf = arg.buf;
  long i = 0;
  
  while(1) {
    int *val;
    
    while (buf[i].data != NULL) {
      printf("WRITER -> Buffer full\n");
      usleep(rand() % 10);
    }

    val = malloc (sizeof(int));
    
    if(val != NULL) {
      *val = rand() % 1000;
      buf[i].data = val;
      printf("WRITER -> Wrote %d in buf[%ld]\n", *val, i);
      
      i = (i+1) % size;
    }
    
    usleep(1);
  }
}
