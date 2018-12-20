#include "pipe_networking.h"

static void sighandler(int signo) {
  if (signo == SIGINT) {
    signal( signo, sighandler);
    printf("client closing.\n");
    remove("server");
    exit(0);
  }
}

int main() {

  int to_server;
  int from_server;
  char buf[BUFFER_SIZE];

  while(1){
    from_server = client_handshake( &to_server );

    printf("Now write back!\n");
    fgets(buf, 255, stdin);
    buf[strlen(buf)] = 0;

    write(to_server, buf, BUFFER_SIZE);
    read(from_server, buf, BUFFER_SIZE);
    printf("recieved message: %s\n", buf);



  }
}
