#include <Keypad.h>
#include <LiquidCrystal.h>

#define LED_Mode1 49
#define LED_Mode2 50
#define LED_Mode3 51
#define LED_Mode4 52

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
const int PLAYER_MODE = 21;
char turn = 'X';

String board[3][3] = {
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
  if (turn == player)
  {
    char key = keypad.waitForKey();
    if (validate(key))
    {
      Serial.print(key);
      char ack = readFromOtherSide();
      if (ack != '\0')
      {
        putOnBoard(key, player);
        if (turn == 'X')
          turn = 'O';
        else
          turn = 'X';
      }
    }
  }
  else
  {
    
  }
}

bool validate(char c)
{
  int n = (int) c - '1';
  // n is in range [0, 8]
  if (n > 8 || n < 0)
    return false;
  if (getFromBoard(c) == '-')
    return true;
  return false;
}

void putOnBoard(char position, char xo)
{
  int n = (int) c - '1';
  board[n/3][n%3] = xo;
}

int getFromBoard(char c)
{
  int n = (int) c - '1';
  return board[n/3][n%3];
}

char readFromOtherSide()
{
  while (!Serial.available());
  return Serial.read();
}

void displayBoard()
{
  lcd.clear();
  for (int i = 0; i < 3; ++i)
  {
    lcd.setCursor(i, 0);
    for (int j = 0; j < 3; ++j)
      lcd.print(board[i][j]);
  }
}

void turnLEDOn(int n)
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
