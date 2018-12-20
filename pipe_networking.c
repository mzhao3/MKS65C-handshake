#include "pipe_networking.h"

char *WKP = "server";


char* scream(char * input) {
  char * screamed = malloc(256 * sizeof(char));

  for (int i = 0; input[i]!= '\0'; i++){
    if((input[i]>='a' && input[i]<='z'))
      screamed[i] = input[i] - 32;
    if(input[i]>='A' && input[i]<='Z')
				screamed[i]=input[i]+32;
    if(input[i]==' ')
      screamed[i] = input[i];
    }
  //strcpy(screamed, input);
  //printf("screamed output:%s\n", screamed);
  return screamed;
}

static void sighandler(int signo) {
  if (signo == SIGINT) {
    signal( signo, sighandler);
    printf("echoing interrupted due to SIGINT.\n");
    remove(WKP);
    exit(1);
  }
}

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

  remove(WKP);
  //printf("Our brains connected. Sending brainwaves. Be on the lookout.\n");
  int w = write(*to_client, "shwooooom", strlen("shwooooom"));
  if ( w < 0)
    perror ("your writing bad");

  while(1){
    signal(SIGINT, sighandler);

    //printf("Waiting for client...\n");
    read(fd, buf, sizeof(buf));
    char message[256];
    strcpy(message, scream(buf));
    //printf("%s\n", scream(buf));
    strcpy(buf,message);
    printf("%s\n", buf);
    //free(message);

    write(*to_client, buf, sizeof(buf));

  }
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

  printf("\n\n starting up your echo chamber \n\n");

  signal(SIGINT, sighandler);


  while(1) {
    printf("Now write back!\n");
    fgets(buf, 255, stdin);
    buf[strlen(buf)] = 0;
    write(*to_server, buf, sizeof(buf));


    if (open(pipe, O_RDONLY) == -1 ) {
      perror("Cannot open pipe.");
      exit(0);
    }
    read(fd, buf, sizeof(buf));
    printf("recieved message: %s\n", buf);


  }

  remove(pipe);
  return fd;
}
