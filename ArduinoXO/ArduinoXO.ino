#include <Keypad.h>
#include <LiquidCrystal.h>

#define LED_YELLOW 18
#define LED_GREEN 19
#define LED_RED 20
#define PLAYER_MODE 21

#define RS_PIN 41
#define EN_PIN 42
#define D4_PIN 43
#define D5_PIN 44
#define D6_PIN 45
#define D7_PIN 46

LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

const byte rows = 4;
const byte cols = 3;

char key[rows][cols] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[rows] = {50, 49, 48, 47};
byte colPins[cols] = {53, 52, 51};

Keypad keypad = Keypad(makeKeymap(key), rowPins, colPins, rows, cols);


int led_up[9] = {31, 32, 33, 34, 35, 36, 37, 38, 39};
int led_down[9] = {2, 3, 4, 5, 6, 7, 8, 9, 10};

char player;
char turn = 'X';

char board[3][3] = {
  {'-', '-', '-'},
  {'-', '-', '-'},
  {'-', '-', '-'}
  };

void setup()
{
  Serial.begin(9600);

  pinMode(50, OUTPUT);
  digitalWrite(50, HIGH);
  pinMode(49, OUTPUT);
  digitalWrite(49, HIGH);
  pinMode(48, OUTPUT);
  digitalWrite(48, HIGH);
  pinMode(47, OUTPUT);
  digitalWrite(47, HIGH);

  pinMode(53, INPUT_PULLUP);
  pinMode(52, INPUT_PULLUP);
  pinMode(51, INPUT_PULLUP);

  for (int i = 0; i < 10; i++)
  {
    pinMode(led_up[i], OUTPUT);
    pinMode(led_down[i], OUTPUT);
  }

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  // Setting Player from player pin
  pinMode(PLAYER_MODE, INPUT);
  if (digitalRead(PLAYER_MODE) == HIGH)
    player = 'X';
  else
    player = 'O';
  
  lcd.begin(20, 4);
  lcd.clear();
}

void loop()
{
  displayBoard();
  if (turn == player)
  {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
    char key = keypad.waitForKey();
    
    if (validate(key))
    {
      Serial.print(key);
      char ack = readFromOtherSide();
      
      if (ack != 'z')
      {
        putOnBoard(key, player);
        updateDisplay();
        checkFinish();
        changeTurn();
      }
    }
  }
  else
  {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    char key = readFromOtherSide();
    if (!validate(key))
    {
      Serial.print('z');
      return;
    }
    Serial.print('1');
    putOnBoard(key, turn);
    updateDisplay();
    checkFinish();
    changeTurn();
  }
}

bool validate(char c)
{
  // n is in range [0, 8]
  if (c > '9' || c < '0')
    return false;
  if (getFromBoard(c) == '-')
    return true;
  return false;
}

void checkFinish()
{
  // Row check
  for (int i = 0; i < 3; ++i)
  {
    char c = board[i][0];
    if (c != '-' && board[i][1] == c && board[i][2] == c)
    {
      finish(c);
      return;
    }
  }

  // Column check
  for (int j = 0; j < 3; ++j)
  {
    char c = board[0][j];
    if (c != '-' && board[1][j] == c && board[2][j] == c)
    {
      finish(c);
      return;
    }
  }

  // X
  char c = board[1][1];
  if (c != '-' && board[0][0] == c &&  board[2][2] == c)
  {
    finish(c);
    return;
  }
  if (c != '-' && board[0][2] == c &&  board[2][0] == c)
  {
    finish(c);
    return;
  }

  // Handling Draw
  bool found = false;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      if (board[i][j] == '-')
      {
        found = true;
        break;
      }
  if (!found)
  {
    finish('-');
    return;
  }
}

void finish(char winner)
{
  if (winner == '-')
  {
    lcd.setCursor(0, 4);
    lcd.print("Draw!");
  }
  else if (winner == player)
  {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, HIGH);
    lcd.setCursor(0, 4);
    lcd.print("You Win!");
  }
  else
  {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    lcd.setCursor(0, 4);
    lcd.print("You Lose!");
  }
  
  keypad.waitForKey();
  // Reset
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      board[i][j] = '-';
}

void changeTurn()
{
  if (turn == 'X')
    turn = 'O';
  else
    turn = 'X';
}

void putOnBoard(char pos, char xo)
{
  int n = (int) pos - '1';
  board[n/3][n%3] = xo;
}

int getFromBoard(char c)
{
  int n = (int) c - '1';
  return board[n/3][n%3];
}

char readFromOtherSide()
{
  digitalWrite(LED_YELLOW, HIGH);
  while (Serial.available() <= 0);
  digitalWrite(LED_YELLOW, LOW);
  int s = Serial.read();
  delay(10);
  return (char) s;
}

void updateDisplay()
{
  displayBoard();
  if (player == 'X')
    for (int i = 0; i < 9; ++i)
      if (board[i/3][i%3] != '-')
        turnLEDOn(i, board[i/3][i%3]);
}

void displayBoard()
{
  lcd.clear();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
    {
      lcd.setCursor(j, i);
      lcd.print(board[i][j]);
    }
}

void turnLEDOn(int n, int player)
{
  if (player == 'X')
  {
    digitalWrite(led_up[n], HIGH);
    digitalWrite(led_down[n], LOW);
  }
  else
  {
    digitalWrite(led_down[n], HIGH);
    digitalWrite(led_up[n], LOW);
  }
}
