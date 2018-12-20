#include "pipe_networking.h"

static void sighandler(int signo) {
  if (signo == SIGINT) {
    signal( signo, sighandler);
    printf("server closing.\n");
    remove("server");
    exit(0);
  }
}


int main() {
  signal(SIGINT, sighandler);

  int to_client;
  int from_client;
  char buf[BUFFER_SIZE];

  while(1){
    from_client = server_handshake( &to_client );


    //printf("Waiting for client...\n");
    while(read(from_client, buf, BUFFER_SIZE) ) {
      char message[BUFFER_SIZE];
      strcpy(message, scream(buf));
      //printf("%s\n", scream(buf));
      strcpy(buf,message);
      printf("server sending: %s\n", buf);
      //free(message);

      write(to_client, buf, BUFFER_SIZE);
    }
    printf("client disconnected. handshake again?\n");
  }

}
