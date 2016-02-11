#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 512

//**********************************************************************************************************//

// Tábla és eredmény kiíró függvények
void print_board(char field[]);
void print_result(char result[]); 

// Csatlakozásért és kommunikációért felelős függvények
int client(int port); 
int transmission(int gate); 

//**********************************************************************************************************//
//**********************************************************************************************************//

int main(int argc, char **argv) {
  
  int gate;
  int port;
  char port_number;
  int readed;

  printf("Give me the port, where the server is runnig!\n");
  scanf("%s", &port_number);

  port = atoi(&port_number); // Konvert számmá

  gate = client(port); // Csatlakozás

  if ((readed = transmission(gate)) < 1) {
    close(gate);
    exit(0);
  }
  
  return 0;
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void print_board(char content[]) { 

  printf("\n\n\t  F I E L D \n\n\n");
  int i, j;
  int c = 0;
  int index = 0;
  
  for (c = 1; c <= 8; ++c) 
    printf("#%d  ", c);
  printf("\n");
  
  for (c = 0; c < 8; ++c) 
    printf("==  ");
  printf("\n\n");
  c = 0;
  
  for (i = 0; i < 8; ++i) {
    for (j = 0; j < 8; ++j) {
      printf("%c   ", content[index++]); 
    }
    if (i != 7) 
      printf("||#%d\n\n", ++c); 
    else
      printf("||#%d\n", ++c); 
  }
  printf("\n");
  
  for (c = 1; c <= 8; ++c)
    printf("==  "); 
  printf("\n");
  
  for (c = 1; c <= 8; ++c)
    printf("#%d  ", c); 
  printf("\n\n");
}
//**********************************************************************************************************//

void print_result(char content[]) { 
  int temp = 0;
  int result = 0;

  temp = content[0] - '0'; // 10-es helyiérték
  result += (10 * temp); 

  temp = content[1] - '0'; // 1-es helyiérték
  result += temp;
  printf("Home player's point(s) : %d\n", result);
  result = 0; 

  temp = content[3] - '0'; // 10-es helyiérték
  result += (10 * temp); 

  temp = content[4] - '0'; // 1-es helyiérték
  result += temp; 
  printf("Away player's point(s) : %d\n", result);  
}
//**********************************************************************************************************//

int transmission (int gate) {

  char buffer[BUFFER_SIZE]; // Szerver üznete
  int reader; // Hiba kód kezelő
  char answer[BUFFER_SIZE]; // Kliens üzenete

  memset(buffer, 0, BUFFER_SIZE); // Buffer kinullázása
  if ((reader = read(gate, buffer, BUFFER_SIZE)) > 0) { // Ha sikeres az olvasás, akkor :
    if (strncmp(buffer, "welcome", 7) == 0) { 
      printf("Welcome\n");
      printf("Valid step format is : x,y\n");
      printf("If you can't step : 'pass' \n");
      printf("If you want to give up : 'give_up'\n");
      printf("START\n");
      printf("Waiting...\n");
      transmission (gate);
      
    } else if (strncmp(buffer, "loose", 5) == 0) {
      printf("You loose!\n");
      write(gate, answer, BUFFER_SIZE);
      transmission(gate);
      
    } else if(strncmp(buffer, "winner", 6) == 0) {
      printf("You won!\n");
      write(gate, answer, BUFFER_SIZE);
      transmission(gate);
      
    } else if(strncmp(buffer, "draw", 4) == 0) {
      printf("Draw!\n");
      write(gate, answer, BUFFER_SIZE);
      transmission(gate);
      
    } else if(buffer[63] != '\0' && buffer[64] == '\0') { // pálya
      print_board(buffer);
      write(gate, answer, BUFFER_SIZE);
      transmission(gate);
      
    } else if(buffer[4] != '\0' && buffer[5] == '\0') { // eredmény
      print_result(buffer);
      write(gate, answer, BUFFER_SIZE);
      transmission(gate);
      
    } else if(strncmp(buffer, "active", 6) == 0) {
      printf("You turn:");
      scanf("%s", answer);
      write(gate, answer , BUFFER_SIZE);
      transmission(gate);

    } else if(strncmp(buffer, "passive", 7) == 0) {
      printf ("Waiting...\n");
      transmission (gate);
      
    } else if(strncmp(buffer, "invalid_step", 12) == 0) {
      printf("Invalid step!\n");
      printf("Try again: ");
      scanf("%s", answer);
      write(gate, answer, BUFFER_SIZE);
      transmission(gate);
      
    } else if(strncmp(buffer, "wrong_answer", 12) == 0) {
      printf("Wrong answer!\n");
      printf("Try again: ");
      scanf("%s", answer);
      write(gate, answer, BUFFER_SIZE);
      transmission (gate);

    } else {
      printf("%s\n", buffer);
      transmission(gate);
    }
    return 0;
  }
  return reader;
}
//**********************************************************************************************************//

int client (int port) {
  
  int gate;
  struct sockaddr_in server;

  // Szerver adatai:
  server.sin_family = AF_INET; // címcsalád
  server.sin_port = htons(port); // port szám, HOST TO NETWORK SHORT
  server.sin_addr.s_addr = htonl(INADDR_ANY); // internet cím, HOST TO NETWORK LONG

  inet_aton("127.0.0.1", &(server.sin_addr)); // aton: Ascii to Network, 
  memset(&(server.sin_zero), '\0', 8); // nulla a strutúra maradék részében

  if ((gate = socket(PF_INET, SOCK_STREAM, 0)) == -1) { // Socket létrehozása
    perror("ERROR: Socket");
    return -1;
  }

  if (connect(gate, (const struct sockaddr *) &server, sizeof(server)) == -1) { // Kapcsolódás kezdeményezése a socketen keresztül a szerverhez
    perror("ERROR: Connect");
    return -1;
    
  }
  return gate;
}
//**********************************************************************************************************//
//**********************************************************************************************************//
