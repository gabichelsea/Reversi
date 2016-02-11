#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "game.c"

#define BUFFER_SZIE 1024

//*********************************************************************//
//*********************************************************************//

void problem(char *message); // Hibaüzenet és kilépés
int create_server(uint16_t port); // Szerver létrehozása

//*********************************************************************//
//*********************************************************************//

int main(int argc, char **argv) {
  
  if (argc != 2)
    problem("How to run: ./server port_number\n");

  printf("Server is creating...\n");
  struct sockaddr_in client;

  char buffer[BUFFER_SZIE];

  const char welcome[] = {"welcome"};
  const char active[] = {"active"};
  const char passive[] = {"passive"};
  const char invalid_step [] = {"invalid_step"};
  const char wrong_answer[] = {"wrong_answer"};
  
  // Eredmény
  const char loose[] = {"loose"};
  const char winner[] = {"winner"};
  const char draw[] = {"draw"};

  int server_port = atoi(argv[1]);
  sleep(2);
  
  int sock = create_server(server_port); // socket létrehozása + bind + listen "akadály"
  printf("Server is runnig...\n\n");
  
  sleep(1);
  printf("Waiting for the connection(2 players)...\n");
  int client_length = sizeof(client);

  int first_stream = 0; // Első játékoshoz az elfogadott kapcsolat kezelőszáma --> accept függvény visszatérési értéke
  first_stream = accept(sock, (struct sockaddr *) &client, &client_length); // Kapcsolódás(kliens) elfogadása az adott socketen 
  if (first_stream == -1)
    problem("ERROR: Accept");  
  printf("1. player connected!\n");
  
  if (send(first_stream, welcome, sizeof(welcome), 0) == -1)
    problem("ERROR: Send");
  
  int second_stream = 0; // Második játékoshoz az elfogadott kapcsolat kezelőszáma
  second_stream = accept(sock, (struct sockaddr *) &client, &client_length);
  if (second_stream == -1)
    problem("ERROR: Accept");
  printf("2. player connected!\n");
  sleep(1);
  printf("Start!\n\n\n");
  
  if (send(second_stream, welcome, sizeof(welcome), 0) == -1)
    problem("ERROR: Send");

  init_board(); // Pálya létrehozása

  char result[5]; // Eredményt tároló tömb

  int pass_senzor_1 = 0; // 1. játékos passzolt-e
  int pass_senzor_2 = 0; // 2. játékos passzolt-e
  int miss = 0; // Változik ha vki passzolt
  
  int end = 0; // 1. vagy 2. játékos feladta e? 0-1-2 értékek
  int i = 4; // "Lépésszámláló": 1.játékos--> páros érték-------------------2.játékos--> páratlan érték
  
  for (;;) {
    convert(); // pálya konvertálása 1D-s tömbbe
    miss = 0;

    if (pass_senzor_1 && pass_senzor_2) // Ha mindkettő passzolt
      break;

    if (i % 2 == 0) { // 1.játékos

      send(second_stream, passive, sizeof(passive), 0); // 2.játékos waiting modba kerül

      send(first_stream, contener, sizeof(contener), 0); // Pályát megkapja az 1.játékos
      recv(first_stream, buffer, BUFFER_SZIE, 0);

      // Eredmény konvertálása
      if (home_points >= 10 && away_points >= 10) // Mindkét játékos kétjegyű ponttal rendelkezik
        sprintf(result, "%d,%d", home_points, away_points);
      else if (home_points < 10 && away_points < 10) // Mindkét játékos egyjegyű ponttal rendelkezik
        sprintf(result, "0%d,0%d", home_points, away_points);
      else if (home_points < 10 && away_points >= 10) // 1.játékos egyjegyű, 2.játékos 2 jegyű ponttal rendelkezik
        sprintf(result, "0%d,%d", home_points, away_points);
      else if (home_points >= 10 && away_points < 10) // 1.játékos kétjegyű, 2.játékos 1 jegyű ponttal rendelkezik
        sprintf(result, "%d,0%d", home_points, away_points);


      send(first_stream, result, sizeof(result), 0); // 1.játékos megkapja az állást
      recv(first_stream, buffer, BUFFER_SZIE, 0);


      send(first_stream, active, sizeof(active), 0); // Majd active módba kerül és léphet
      recv(first_stream, buffer, BUFFER_SZIE, 0);

      while (1) {
        if ((strncmp (buffer, "pass", 4) == 0) && buffer[4] == '\0') { // Ha passzolt:
          printf("First player sent: pass\n");
          pass_senzor_1 = 1; // 1-es player passzolt
          miss = next_round(); // miss értéke 1-es
          break;
        }
        pass_senzor_1 = 0; // Ha nem passzolt, akkor visszaállitjuk "false"-ra

        if ((strncmp(buffer, "give_up", 7) == 0) && buffer[7] == '\0') { // Ha feladta:
          printf("First player sent: give_up\n");
          end = 1; // 1.player adta fel
          break;
	  
        } else if (buffer[1] == ',' && buffer[2] != '\0' && buffer[3] == '\0') { // Ha megfelelő formátumban küldte a két párt:
          int line = buffer[0] - '0'; // 1.koordináta számmá alakítása
          int column = buffer[2] - '0'; // 2.koordináta számmá alakítása
 
          --line; // 0...7
          --column; // 0...7

          if (line < 0 || column < 0 || line > 7 || column > 7 || board[line][column] != ':') { // Ha nem a pályán lévő mezőt választotta vagy ott már van bábú
	    printf("1. player has an invalid step\n");
            send(first_stream, invalid_step, sizeof(invalid_step), 0);
            recv(first_stream, buffer, BUFFER_SZIE, 0);
            continue; // akkor átugorjuk a while (1) hátralévő részét
          }


          set_data(line, column); // line, column beállítása
          int sign = insert(); // Érvényes lépés volt e? (foglaltunk e le bábút az ellenféltől)

          if (sign) { // Ha igen
            step = 0; // akkor nullázzuk
            next_round(); // swappolunk
            break;
          } else {
	    printf("1. player has an invalid step\n");
            send(first_stream, invalid_step, sizeof(invalid_step), 0); // Ha nem sikeres a lépésünk
            recv(first_stream, buffer, BUFFER_SZIE, 0);
          }
        } else { // Ha nem megfelelő a formátum: x,y
	  printf("1. player has a wrong wrong answer\n");
          send(first_stream, wrong_answer, sizeof(wrong_answer), 0); 
          recv(first_stream, buffer, BUFFER_SZIE, 0);
        }
      }
    }


// Ha a 2.játékosunk jön:
    else { 
      send(first_stream, passive, sizeof(passive), 0); // 1.játékos waiting módba kerül

      send(second_stream, contener, sizeof(contener), 0); // 2.játékos megkapja a pályát
      recv(second_stream, buffer, BUFFER_SZIE, 0);
      

      // Eredmény
      if(home_points >= 10 && away_points >= 10)
        sprintf(result, "%d,%d", home_points, away_points);
      else if(home_points < 10 && away_points < 10)
        sprintf(result, "0%d,0%d", home_points, away_points);
      else if(home_points < 10 && away_points >= 10)
        sprintf(result, "0%d,%d", home_points, away_points);
      else if(home_points >= 10 && away_points < 10)
        sprintf(result, "%d,0%d", home_points, away_points);

      
      send(second_stream, result, sizeof(result), 0);
      recv(second_stream, buffer, BUFFER_SZIE, 0);


      send(second_stream, active, sizeof(active), 0);
      recv(second_stream, buffer, BUFFER_SZIE, 0);

      while (1) {
        if ((strncmp (buffer, "pass", 4) == 0) && buffer[4] == '\0') {
          printf("Second player sent: pass\n");
          pass_senzor_2 = 1; // 2-es player passzolt
          miss = next_round();
          break;
        }
        pass_senzor_2 = 0;
        if ((strncmp(buffer, "give_up", 7) == 0) && buffer[7] == '\0') {
          printf("Second player sent: give_up\n");
          end = 2;
          break;
        } else if (buffer[1] == ',' && buffer[2] != '\0' && buffer[3] == '\0') {
          int line = buffer[0] - '0';
          int column = buffer[2] - '0';
          --line;
          --column;

          if (line < 0 || column < 0 || line > 7 || column > 7 || board[line][column] != ':') {
	    printf("2. player has an invalid step\n");
            send(second_stream, invalid_step, sizeof(invalid_step), 0);
            recv(second_stream, buffer, BUFFER_SZIE, 0);
	    continue;
          }

          set_data(line,column);
          int sign = insert();

          if (sign) {
            step = 0;
            next_round();
            break;
          } else {
	    printf("2. player has an invalid step\n");
            send(second_stream, invalid_step, sizeof(invalid_step), 0);
            recv(second_stream, buffer, BUFFER_SZIE, 0);
          }
        } else {
	  printf("2. player has a wrong answer\n");
          send(second_stream, wrong_answer, sizeof(wrong_answer), 0);
          recv(second_stream, buffer, BUFFER_SZIE, 0);
        }
      } // while vége

    }


    if (!miss) { // Ha nem volt passz
      if (i % 2 == 0) // 1.játékos rakta le a bábút
        ++home_points;
      else // 2.játékos tette le a bábút
        ++away_points;
    }


    if (end) // Ha vki feladta, érték : 1 vagy 2
      break;

    
    printf("\nRound: %d\n", ++i);
    printf("Line: %d & Column: %d\n", line + 1, column + 1);
    printf("Home points: %d\n", home_points);
    printf("Away points: %d\n\n\n", away_points);
    
    if ((home_points + away_points) == 64) // Ha betelt a tábla
      break;

  }
  
  int who_win = 0;

  if (end == 1) { // Ha az 1.játékos adta fel
    send(first_stream, loose, sizeof(loose), 0);
    send(second_stream, winner, sizeof(winner),0);
    who_win = 2;
    
  } else if (end == 2) { // Ha a 2.játékos adta fel
    send(second_stream, loose, sizeof(loose), 0);
    send(first_stream, winner, sizeof(winner),0);
    who_win = 1;
    
  } else if (home_points > away_points) { // Ha 1.játékos pontja több
    send(second_stream, loose, sizeof(loose), 0);
    send(first_stream, winner, sizeof(winner),0);
    who_win = 1;
    
  } else if (home_points < away_points) { // Ha 2.játékos pontja több
    send(first_stream, loose, sizeof(loose), 0);
    send(second_stream, winner, sizeof(winner),0);
    who_win = 2;
    
  } else if (home_points == away_points) { // Ha egyenlő
    send(first_stream, draw, sizeof(draw), 0);
    send(second_stream, draw, sizeof(draw),0);
  }
  
  
 

  close(first_stream); // Zárjuk a kapcsolatot
  close(second_stream); // Zárjuk a kapcsolatot
  close(sock); // Zárjuk a szervert
  
  switch(who_win) {
    case 0: printf("Result: draw!\n"); break;
    case 1: printf("Result: 1. player won!\n"); break;
    case 2: printf("Result: 2. player won!\n"); break;
    default: printf("Impossible!\n"); break;
  }
  
  sleep(1);
  printf("Server shutdown\n");
  return 0;
}

//*********************************************************************//
//*********************************************************************//


void problem(char *message) {
  fprintf(stderr,"%s\n", message);
  exit(EXIT_FAILURE);
}
//*********************************************************************//

int create_server(uint16_t port) {

  int server_number = socket(PF_INET, SOCK_STREAM, 0); // Socket létrehozása
  if (server_number == -1)
    problem("ERROR: Socket");

  // Szerver adatai
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY: aktuális IP címem használata
  memset(&(server.sin_zero), '\0', 8); // nulla a strutúra maradék részében
  
  if (bind(server_number, (struct sockaddr *) &server, sizeof(server)) == -1) // Socket hozzárendelése a portszámhoz
    problem("ERROR: Bind");

  if (listen(server_number, 3) == -1) // Hallgatózás a socketen
    problem("ERROR: Listen");

  return server_number;
}
