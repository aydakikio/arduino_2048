#include <U8g2lib.h>
#include <EEPROM.h>

// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, -1, A5, A4);

// Button pin definitions
#define BTN_RIGHT 10
#define BTN_DOWN  9
#define BTN_LEFT  7
#define BTN_UP    6

// EEPROM
#define EEPROM_ADDR_HIGHSCORE 0

// ===== GAME DATA STRUCTURES =====
struct Block {
  int x;
  int y;
  int w;
  int h;
  int value;
};

// Game grid: 4 rows x 6 columns
Block blocks[4][6];

// Game state variables
int score = 0;
int highScore = 0;
bool game_over = false;

// ===== FUNCTION DECLARATIONS =====
void draw_game();
void draw_gameover();
void initialize_game();
void add_random_block();
bool move_left();
bool move_right();
bool move_up();
bool move_down();
bool canMove();
void draw_boxes();

// ===== SETUP FUNCTION =====
void setup() {
  u8g2.begin();

  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_DOWN,  INPUT_PULLUP);
  pinMode(BTN_LEFT,  INPUT_PULLUP);
  pinMode(BTN_UP,    INPUT_PULLUP);

  randomSeed(analogRead(A0));

  EEPROM.get(EEPROM_ADDR_HIGHSCORE, highScore);
  if (highScore < 0 || highScore > 100000) highScore = 0;

  initialize_game();
}

// ===== MAIN LOOP =====
void loop() {
  
  if (!game_over) {

    static unsigned long lastPress = 0;
    unsigned long now = millis();

    if (now - lastPress > 200) {
      bool moved = false;

      if (digitalRead(BTN_LEFT) == LOW)  { moved = move_left();  lastPress = now; }
      else if (digitalRead(BTN_RIGHT) == LOW) { moved = move_right(); lastPress = now; }
      else if (digitalRead(BTN_UP) == LOW)    { moved = move_up();    lastPress = now; }
      else if (digitalRead(BTN_DOWN) == LOW)  { moved = move_down();  lastPress = now; }

    if (moved && canMove()) {
      add_random_block();
    }

    // ALWAYS check after an input attempt
    if (!canMove()) {
      game_over = true;
      if (score > highScore) {
        highScore = score;
        EEPROM.put(EEPROM_ADDR_HIGHSCORE, highScore);
      }
    }

    }

    draw_game();
    delay(40);

  } else {

    draw_gameover();

    if (digitalRead(BTN_LEFT)  == LOW ||
        digitalRead(BTN_RIGHT) == LOW ||
        digitalRead(BTN_UP)    == LOW ||
        digitalRead(BTN_DOWN)  == LOW) {

      game_over = false;
      initialize_game();
      delay(300);
    }
  }

}

// ===== GAME INITIALIZATION =====
void initialize_game() {
  score = 0;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      blocks[i][j].value = 0;
      blocks[i][j].x = j * 16;
      blocks[i][j].y = i * 16;
      blocks[i][j].w = 14;
      blocks[i][j].h = 14;
    }
  }

  add_random_block();
  add_random_block();
}

// ===== ADD RANDOM TILE =====
void add_random_block() {
  int empty_count = 0;

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 6; j++)
      if (blocks[i][j].value == 0) empty_count++;

  if (empty_count == 0) return;

  int target = random(empty_count);
  int count = 0;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      if (blocks[i][j].value == 0) {
        if (count == target) {
          blocks[i][j].value = (random(10) < 9) ? 2 : 4;
          return;
        }
        count++;
      }
    }
  }
}

// ===== CAN MOVEMent Utility =====
bool canMove() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      if (blocks[i][j].value == 0) return true;
      if (j < 5 && blocks[i][j].value == blocks[i][j+1].value) return true;
      if (i < 3 && blocks[i][j].value == blocks[i+1][j].value) return true;
    }
  }
  return false;
}

// ===== MOVE FUNCTIONS =====
bool move_left() {
  bool moved = false;

  for (int i = 0; i < 4; i++) {
    int writePos = 0;
    int lastMerged = -1;

    for (int j = 0; j < 6; j++) {
      if (blocks[i][j].value != 0) {
        if (writePos > 0 &&
            blocks[i][writePos-1].value == blocks[i][j].value &&
            lastMerged != writePos-1) {

          blocks[i][writePos-1].value *= 2;
          score += blocks[i][writePos-1].value;
          blocks[i][j].value = 0;
          lastMerged = writePos-1;
          moved = true;
        } else {
          if (writePos != j) {
            blocks[i][writePos].value = blocks[i][j].value;
            blocks[i][j].value = 0;
            moved = true;
          }
          writePos++;
        }
      }
    }
  }
  return moved;
}

bool move_right() {
  bool moved = false;

  for (int i = 0; i < 4; i++) {
    int writePos = 5;
    int lastMerged = -1;

    for (int j = 5; j >= 0; j--) {
      if (blocks[i][j].value != 0) {
        if (writePos < 5 &&
            blocks[i][writePos+1].value == blocks[i][j].value &&
            lastMerged != writePos+1) {

          blocks[i][writePos+1].value *= 2;
          score += blocks[i][writePos+1].value;
          blocks[i][j].value = 0;
          lastMerged = writePos+1;
          moved = true;
        } else {
          if (writePos != j) {
            blocks[i][writePos].value = blocks[i][j].value;
            blocks[i][j].value = 0;
            moved = true;
          }
          writePos--;
        }
      }
    }
  }
  return moved;
}

bool move_up() {
  bool moved = false;

  for (int j = 0; j < 6; j++) {
    int writePos = 0;
    int lastMerged = -1;

    for (int i = 0; i < 4; i++) {
      if (blocks[i][j].value != 0) {
        if (writePos > 0 &&
            blocks[writePos-1][j].value == blocks[i][j].value &&
            lastMerged != writePos-1) {

          blocks[writePos-1][j].value *= 2;
          score += blocks[writePos-1][j].value;
          blocks[i][j].value = 0;
          lastMerged = writePos-1;
          moved = true;
        } else {
          if (writePos != i) {
            blocks[writePos][j].value = blocks[i][j].value;
            blocks[i][j].value = 0;
            moved = true;
          }
          writePos++;
        }
      }
    }
  }
  return moved;
}

bool move_down() {
  bool moved = false;

  for (int j = 0; j < 6; j++) {
    int writePos = 3;
    int lastMerged = -1;

    for (int i = 3; i >= 0; i--) {
      if (blocks[i][j].value != 0) {
        if (writePos < 3 &&
            blocks[writePos+1][j].value == blocks[i][j].value &&
            lastMerged != writePos+1) {

          blocks[writePos+1][j].value *= 2;
          score += blocks[writePos+1][j].value;
          blocks[i][j].value = 0;
          lastMerged = writePos+1;
          moved = true;
        } else {
          if (writePos != i) {
            blocks[writePos][j].value = blocks[i][j].value;
            blocks[i][j].value = 0;
            moved = true;
          }
          writePos--;
        }
      }
    }
  }
  return moved;
}

// ===== DRAWING =====
void draw_boxes() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      if (blocks[i][j].value > 0) {

        int x = blocks[i][j].x + 2;
        int y = blocks[i][j].y + 2;
        int w = blocks[i][j].w;
        int h = blocks[i][j].h;

        // Clear tile area (IMPORTANT)
        u8g2.setDrawColor(0);
        u8g2.drawBox(x, y, w, h);

        // Draw frame
        u8g2.setDrawColor(1);
        u8g2.drawFrame(x, y, w, h);

        // Draw number
        if (blocks[i][j].value < 128){
          u8g2.setFont(u8g2_font_5x7_mf);
        } else{
          u8g2.setFont(u8g2_font_4x6_mf);
        }

        char buf[8];
        itoa(blocks[i][j].value, buf, 10);

        int textWidth = strlen(buf) * 5;
        int textX = x + (w - textWidth) / 2;
        int textY = y + 11;

        u8g2.setCursor(textX, textY);
        u8g2.print(buf);
      }
    }
  }
}


void draw_game() {
  u8g2.firstPage();
  do {
    u8g2.drawFrame(0, 0, 96, 64);
    draw_boxes();

    u8g2.setFont(u8g2_font_5x7_mf);
    u8g2.drawStr(100, 10, "Score");
    u8g2.setCursor(100, 20);
    u8g2.print(score);

    u8g2.drawStr(100, 40, "Best");
    u8g2.setCursor(100, 50);
    u8g2.print(highScore);

  } while (u8g2.nextPage());
}

void draw_gameover() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_7x14B_tr);
    u8g2.drawStr(24, 20, "Game Over");

    u8g2.setFont(u8g2_font_5x7_mf);

    if(score > highScore){
      u8g2.drawStr(12, 40 ,  "New Score!");
    }

    u8g2.drawStr(12, 60, "Press any button...");

  }while (u8g2.nextPage());
}