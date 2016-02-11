//**********************************************************************************************************//
//**********************************************************************************************************//

char board[8][8]; // Pálya 2D
char contener[64]; // Pálya 1D

//**********************************************************************************************************//

// Játékosok kezdő "adatai"
int home_points = 2; 
int away_points = 2;

char home_char = 'X';
char away_char = 'O';

//**********************************************************************************************************//

// globális i és j változom a 8 analizis függvényekhez
int i = 0;
int j = 0;

int point_counter = 0; // Ellenféltől hány bábút és pontot szerzek meg

// Bekért sor és oszlop
int line = 0; 
int column = 0; 

int step = 0; // logikai változoként szerepel

//**********************************************************************************************************//
//**********************************************************************************************************//

// 8 irányba történő analizisek 
void analysis_1();
void analysis_2();
void analysis_3();
void analysis_4();
void analysis_5();
void analysis_6();
void analysis_7();
void analysis_8();

//**********************************************************************************************************//

// Tábla létrehozó, 1D-s tömbbe konvertálo
void init_board(); 
void convert();

//**********************************************************************************************************//

int next_round(); // swap
void set_data(int l, int c); // Setter függvény
int insert(); // érvényes lépésem volt e(8 analizis)
void restart(); // analizis-ekhez segédfüggvény

//**********************************************************************************************************//
//**********************************************************************************************************//

void init_board() {
  int i, j;
  for (i = 0; i < 8; ++i)
    for (j = 0; j < 8; ++j)
      board[i][j] = ':';

  board[3][3] = 'X';
  board[3][4] = 'O';
  board[4][3] = 'O';
  board[4][4] = 'X';
}
//**********************************************************************************************************//

void convert() {
  int i, j;
  int elem = 0;
  for (i = 0; i < 8; ++i)
    for (j = 0; j < 8; ++j)
      contener[elem++] = board[i][j];
}
//**********************************************************************************************************//

int next_round() {
  char temp = home_char;
  home_char = away_char;
  away_char = temp;
  return 1;
}
//**********************************************************************************************************//

void set_data(int l, int c) {
  line = l;
  column = c;
}
//**********************************************************************************************************//

int insert() {
  
  analysis_1();
  analysis_2();
  analysis_3();
  analysis_4();
  analysis_5();
  analysis_6();
  analysis_7();
  analysis_8();

  return step; // Volt-e lépés
}
//**********************************************************************************************************//

void restart() {
  i = line;
  j = column;
  point_counter = 0;
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void analysis_1() { // Adott poziciotol jobbra lévő elemek elfoglalásának tesztje, csak a "j"-vel lépkedek jobbra
  restart();
  for (j += 1; (j <= 7 && j >= 0) && !(i < 0 || i > 7 || j < 0 || j > 7) && (board[i][j] == away_char); ++j)
    ++point_counter;
  if ((point_counter != 0) && (board[i][j] == home_char)) { // Ekkor átváltom az ő korongjait az enyémekre
    step = 1;
    int y;
    for ( y = column; y < j; ++y)
      board[i][y] = home_char;

    if (home_char == 'X') {
      home_points += point_counter;
      away_points -= point_counter;
    } else { // HHa csere van és a következő (vendég)játékos probálkozik akkor itt is toljuk a cserét
      home_points -= point_counter;
      away_points += point_counter;
    }
  }
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void analysis_2() { // Adott poziciotol balra lévő elemek elfoglalásának tesztje, csak a "j"-vel lépkedek balra
  restart();
  for (j -= 1; (j <= 7 && j >= 0) && (board[i][j] == away_char); --j)
    ++point_counter;
  if ((point_counter != 0) && !(i < 0 || i > 7 || j < 0 || j > 7) && (board[i][j] == home_char)) { // Ekkor átváltom az ő korongjait az enyémekre
    step = 1;
    int y;
    for (y = column; y > j; --y)
      board[i][y] = home_char;

    if (home_char == 'X') {
      home_points += point_counter;
      away_points -= point_counter;
    } else { // HHa csere van és a következő (vendég)játékos probálkozik akkor itt is toljuk a cserét
      home_points -= point_counter;
      away_points += point_counter;
    }
  }
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void analysis_3() { // Adott poziciotol felfelé(nagyobbtol a kisebbfelé) lévő elemek elfoglalásának tesztje, csak a "i"-vel lépkedek felfelé(csökken)
  restart();
  for (i -= 1; (i <= 7 && i >= 0) && (board[i][j] == away_char); --i)
    ++point_counter;
  if ((point_counter != 0) && !(i < 0 || i > 7 || j < 0 || j > 7) && (board[i][j] == home_char)) { // Ekkor átváltom az ő korongjait az enyémekre
    step = 1;
    int x;
    for (x = line; x > i; --x)
      board[x][j] = home_char;

    if (home_char == 'X') {
      home_points += point_counter;
      away_points -= point_counter;
    } else { // HHa csere van és a következő (vendég)játékos probálkozik akkor itt is toljuk a cserét
      home_points -= point_counter;
      away_points += point_counter;
    }
  }
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void analysis_4() { // Adott poziciotol lefelé(kisebbtol a nagyobbfelé) lévő elemek elfoglalásának tesztje, csak a "i"-vel lépkedek lefelé(nő)
  restart();
  for (i += 1; (i <= 7 && i >= 0) && (board[i][j] == away_char); ++i)
    ++point_counter;

  if ((point_counter != 0) && !(i < 0 || i > 7 || j < 0 || j > 7) && (board[i][j] == home_char)) { // Ekkor átváltom az ő korongjait az enyémekre
    step = 1;
    int x;
    for (x = line; x < i; ++x)
      board[x][j] = home_char;

    if (home_char == 'X') {
      home_points += point_counter;
      away_points -= point_counter;
    } else { // HHa csere van és a következő (vendég)játékos probálkozik akkor itt is toljuk a cserét
      home_points -= point_counter;
      away_points += point_counter;
    }
  }
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void analysis_5() { // Adott poziciotol jobbra fel(átló) lévő elemek elfoglalásának tesztje
  restart();
  for (i -= 1, j += 1; (i <= 7 && i >= 0) && (j <= 7 && j >= 0) && (board[i][j] == away_char); --i, ++j)
    ++point_counter;
  if ((point_counter != 0) && !(i < 0 || i > 7 || j < 0 || j > 7) && (board[i][j] == home_char)) { // Ekkor átváltom az ő korongjait az enyémekre
    step = 1;
    int x;
    int y;
    for (x = line, y = column; x > i, y < j ; --x, ++y)
      board[x][y] = home_char;

    if (home_char == 'X') {
      home_points += point_counter;
      away_points -= point_counter;
    } else { // HHa csere van és a következő (vendég)játékos probálkozik akkor itt is toljuk a cserét
      home_points -= point_counter;
      away_points += point_counter;
    }
  }
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void analysis_6() { // Adott poziciotol jobbra lefelé(átló) lévő elemek elfoglalásának tesztje
  restart();
  for (i += 1, j += 1; (i <= 7 && i >= 0) && (j <= 7 && j >= 0) && (board[i][j] == away_char); ++i, ++j)
    ++point_counter;

  if ((point_counter != 0) && !(i < 0 || i > 7 || j < 0 || j > 7) && (board[i][j] == home_char)) { // Ekkor átváltom az ő korongjait az enyémekre
    step = 1;
    int x,y;
    for (x = line, y = column; x < i, y < j ; ++x, ++y)
      board[x][y] = home_char;

    if (home_char == 'X') {
      home_points += point_counter;
      away_points -= point_counter;
    } else { // HHa csere van és a következő (vendég)játékos probálkozik akkor itt is toljuk a cserét
      home_points -= point_counter;
      away_points += point_counter;
    }
  }
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void analysis_7() { // Adott poziciotol balra le(átló) lévő elemek elfoglalásának tesztje
  restart();
  for (i += 1, j -= 1; (i <= 7 && i >= 0) && (j <= 7 && j >= 0) && (board[i][j] == away_char); ++i, --j)
    ++point_counter;

  if ((point_counter != 0) && !(i < 0 || i > 7 || j < 0 || j > 7) && (board[i][j] == home_char)) { // Ekkor átváltom az ő korongjait az enyémekre
    step = 1;
    int x,y;
    for (x = line, y = column; x < i, y > j ; ++x, --y)
      board[x][y] = home_char;

    if (home_char == 'X') {
      home_points += point_counter;
      away_points -= point_counter;
    } else { // HHa csere van és a következő (vendég)játékos probálkozik akkor itt is toljuk a cserét
      home_points -= point_counter;
      away_points += point_counter;
    }
  }
}
//**********************************************************************************************************//
//**********************************************************************************************************//

void analysis_8() { // Adott poziciotol balra fel(átló) lévő elemek elfoglalásának tesztje
  restart();
  for (i -= 1, j -= 1; (i <= 7 && i >= 0) && (j <= 7 && j >= 0) && (board[i][j] == away_char); --i, --j)
    ++point_counter;

  if ((point_counter != 0) && !(i < 0 || i > 7 || j < 0 || j > 7) && (board[i][j] == home_char)) { // Ekkor átváltom az ő korongjait az enyémekre
    step = 1;
    int x,y;
    for (x = line, y = column; x > i, y > j ; --x, --y)
      board[x][y] = home_char;

    if (home_char == 'X') {
      home_points += point_counter;
      away_points -= point_counter;
    } else { // HHa csere van és a következő (vendég)játékos probálkozik akkor itt is toljuk a cserét
      home_points -= point_counter;
      away_points += point_counter;
    }
  }
}
//**********************************************************************************************************//
//**********************************************************************************************************//
