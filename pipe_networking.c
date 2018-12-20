#include "pipe_networking.h"

char *WKP = "server";
/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  //reads first
  remove(WKP);
  char buf[256];
  int fd;

  //creates the WKP
  if (mkfifo(WKP, 0666) < 0) {
    perror("Server pipe failure.");
    exit(0);
  }

  printf("Well known pipe created.\n");

  //open WKP for reading
  printf("Waiting for client.\n");
  fd = open( WKP, O_RDONLY );
  if (fd < 0)
    perror("Cannot open pipe.");

  printf("Reading the pipe...\n");
  read(fd, buf, sizeof(buf));
  printf("This pipe says %s\n\n", buf);

  printf("Write back bro.\n");
  // opens in write mode
  *to_client = open(buf, O_WRONLY);
  if (*to_client < 0)
    perror("Writing side malfunction...\n");

  printf("Our brains connected. Sending brainwaves. Be on the lookout.\n");
  int w = write(*to_client, "shwooooom", strlen("shwooooom"));
  if ( w < 0)
    perror ("your writing bad");

  printf("Waiting for client...\n");
  read(fd, buf, sizeof(buf));
  if (strlen(buf) < 1) {
    printf("uhhhhh\n");
    return -1;
  }
  printf("We done it! %s recieved.\n", buf);

  return fd;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int fd;
  char buf[256];
  char *pipe = "private";

  if (mkfifo(pipe, 0666) < 0) {
    perror("Client pipe failure.");
    exit(0);
  }
  printf("Private pipe created.\n");

  printf("Writing to server about private pipe.\n");
  *to_server = open(WKP, O_WRONLY);
  write(*to_server, pipe, sizeof(pipe));

  fd = open(pipe, O_RDONLY);
  printf("Waiting for server. \n");

  if (fd == -1){
    perror("Cannot open pipe.");
    exit(0);
  }
  read(fd, buf, sizeof(buf));
  printf("Your message, your highness: %s\n", buf);

  remove(pipe);

  printf("Now write back!\n");
  write(*to_server, "your mom!", strlen("your mom!"));

  return fd;

}
