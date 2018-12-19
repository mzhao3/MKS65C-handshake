#include "pipe_networking.h"


char *WKP = "WKPipe";
/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/

int server_handshake(int *to_client) {
  remove(WKP);
  char buf[256];
  int read_end, write_end;
  if (mkfifo(WKP, 0666) == 1)
    perror("Server pipe failure.");

  printf("Opening up the pipe...\n");
  read_end = open( WKP, O_RDONLY );
  if (read_end < 0)
    perror("Cannot open pipe.");

  printf("Reading the pipe...\n");
  read(read_end, buf, 256);
  printf("This pipe says %s\n\n", buf);

  printf("Write back bro.\n");
  write_end = open(buf, O_WRONLY);
  if (write_end < 0)
    perror("Writing side malfunction...\n");

  printf("Our brains connected. Sending brainwaves. Be on the lookout.\n");
  if ( write(write_end, "shwooooom", strlen("shwooooom")) < 0)
    perror ("your writing bad");

  read(read_end, buf, 256);
  if (strlen(buf) < 1) {
    printf("uhhhhh\n");
    return -1;
  }
  printf("We done it! %s recieved.\n", buf);

  *to_client = write_end;
  return read_end;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  remove (WKP);
  int read_end, write_end;
  char buf[256];
  char *pipe = "client";

  if (mkfifo(pipe, 0666) == -1)
    perror("Client pipe failure.");

  printf("Pipe created.\n");

  printf("Write something\n");
  write_end = open(pipe, O_WRONLY);
  if (write_end == -1)
    perror("Why can't you write correctly -_-.\n");

  write(write_end, pipe, strlen(pipe)+1);

  read_end = open(pipe, O_RDONLY);
  if (read_end == -1)
    perror("Cannot open pipe.");
  printf("waiting for the pipe.\n");
  read(read_end, buf, 256);
  printf("Your message, your highness: %s\n", buf);

  printf("Now write back!\n");
  write(write_end, "your mom!", strlen("your mom!"));
  remove(pipe);

  *to_server = write_end;
  return read_end;

  return 0;
}
