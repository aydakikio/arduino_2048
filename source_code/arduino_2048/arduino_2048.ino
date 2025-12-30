#include <U8g2lib.h>

// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, -1, A5, A4);

// Button pin definitions
#define BTN_RIGHT 10
#define BTN_DOWN 9
#define BTN_LEFT 7
#define BTN_UP 6

// ===== GAME DATA STRUCTURES =====
struct Block {
  int x;
  int y;      
  int w;     
  int h;     
  int value;  
};

// Game grid: 4 rows x 6 columns = 24 tiles total
Block blocks[4][6];

// Game state variables
int score = 0;           
bool gameStarted = false; 
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
void draw_boxes();

// ===== SETUP FUNCTION =====
void setup() {
  u8g2.begin();
  
  // Configure all button pins
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  
  randomSeed(analogRead(A0));
  
  initialize_game();
}

// ===== MAIN LOOP =====
void loop() {
  if (!game_over) {
      static unsigned long lastPress = 0;  
    unsigned long currentTime = millis(); 
    
    if (currentTime - lastPress > 200) {
      bool moved = false; 
      
      if (digitalRead(BTN_LEFT) == LOW) {
        moved = move_left();
        lastPress = currentTime;
      }
      else if (digitalRead(BTN_RIGHT) == LOW) {
        moved = move_right();
        lastPress = currentTime;
      }
      else if (digitalRead(BTN_UP) == LOW) {
        moved = move_up();
        lastPress = currentTime;
      }
      else if (digitalRead(BTN_DOWN) == LOW) {
        moved = move_down();
        lastPress = currentTime;
      }
      
      // If player successfully moved tiles, add a new random tile
      if (moved) {
        add_random_block();
      }
    }
    
    // Redraw the screen with current game state
    draw_game();
    delay(50);  
  }else {
    
    draw_gameover();

    if (condition) {
    statements
    }
    
  }

}

// ===== GAME INITIALIZATION =====
void initialize_game() {
  score = 0;  
  
  // Loop through all tiles 
  for (int i = 0; i < 4; i++) {      // i = row number (0-3)
    for (int j = 0; j < 6; j++) {    // j = column number (0-5)
  
      // Set all tiles to empty (value = 0)
      blocks[i][j].value = 0;
      
      // Calculate position: each tile is 15 pixels wide/tall
      blocks[i][j].x = j * 16;  // 16px per column
      blocks[i][j].y = i * 16;  // 16px per row
      
      // Set size of each tile
      blocks[i][j].w = 14;  
      blocks[i][j].h = 14;  
    }
  }
  
  // Start the game with two random tiles
  add_random_block();
  add_random_block();
  gameStarted = true;
}

// ===== ADD RANDOM TILE =====
void add_random_block() {

  int empty_count = 0;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      if (blocks[i][j].value == 0) {
        empty_count++; 
      }
    }
  }
  
  // If board is full, can't add a tile
  if (empty_count == 0){
    game_over = true;
  }
  
  // STEP 2: Pick a random empty space (0 to empty_count-1)
  int target = random(empty_count);
  int count = 0;
  
  // STEP 3: Find that specific empty space and place a tile there
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      if (blocks[i][j].value == 0) {
        if (count == target) {
          // 90% chance of 2, 10% chance of 4
          // random(10) gives 0-9, so 0-8 = 2, and 9 = 4
          blocks[i][j].value = (random(10) < 9) ? 2 : 4;
          return;  // Done! Exit function
        }
        count++;
      }
    }
  }
}

// ===== HORIZENTAL MOVING =====
bool move_left() {
  bool moved = false;
  
  // Process each row 
  for (int i = 0; i < 4; i++) {
    int writePos = 0;      
    int lastMerged = -1;   
    
    // Scan columns from left to right
    for (int j = 0; j < 6; j++) {

      // If this tile not empty
      if (blocks[i][j].value != 0) {
        if (writePos > 0 && 
            blocks[i][writePos-1].value == blocks[i][j].value && 
            lastMerged != writePos-1) {

          // MERGE! Double the value of left tile

          blocks[i][writePos-1].value *= 2;
          score += blocks[i][writePos-1].value;  
          blocks[i][j].value = 0;                
          lastMerged = writePos - 1;             
          moved = true;
        } 

        // NO MERGE: Just slide the tile left
        else {

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
    
    // Scan from right to left
    for (int j = 5; j >= 0; j--) {
      if (blocks[i][j].value != 0) {
        if (writePos < 5 && 
            blocks[i][writePos+1].value == blocks[i][j].value && 
            lastMerged != writePos+1) {
          blocks[i][writePos+1].value *= 2;
          score += blocks[i][writePos+1].value;
          blocks[i][j].value = 0;
          lastMerged = writePos + 1;
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

// ===== VERTICAL MOVING ======
bool move_up() {
  bool moved = false;
  
  // Process each column 
  for (int j = 0; j < 6; j++) {
    int writePos = 0;      
    int lastMerged = -1;
    
    // Scan from top to bottom
    for (int i = 0; i < 4; i++) {
      if (blocks[i][j].value != 0) {
        // Try to merge with tile above
        if (writePos > 0 && 
            blocks[writePos-1][j].value == blocks[i][j].value && 
            lastMerged != writePos-1) {
          blocks[writePos-1][j].value *= 2;
          score += blocks[writePos-1][j].value;
          blocks[i][j].value = 0;
          lastMerged = writePos - 1;
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
    
    // Scan from bottom to top
    for (int i = 3; i >= 0; i--) {
      if (blocks[i][j].value != 0) {
        // Try to merge with tile below
        if (writePos < 3 && 
            blocks[writePos+1][j].value == blocks[i][j].value && 
            lastMerged != writePos+1) {
          blocks[writePos+1][j].value *= 2;
          score += blocks[writePos+1][j].value;
          blocks[i][j].value = 0;
          lastMerged = writePos + 1;
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

// ===== DRAW ALL TILES =====
void draw_boxes() {
  // Loop through all tiles
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      // Only draw tiles that have a value
      if (blocks[i][j].value > 0) {
        // Draw the tile border (rectangle outline)
        u8g2.drawFrame(blocks[i][j].x + 2, 
                      blocks[i][j].y + 2, 
                      blocks[i][j].w, 
                      blocks[i][j].h);
        
        // Prepare to draw the number
        u8g2.setFont(u8g2_font_5x7_mf);  

        // Convert number to string
        char buf[5];  // Buffer to hold the text
        itoa(blocks[i][j].value, buf, 10);  // Convert int to string (base 10)
        
        // CENTER THE TEXT in the tile
        int textWidth = strlen(buf) * 5;  // Each character is ~5 pixels wide
        int textX = blocks[i][j].x + 2 + (blocks[i][j].w - textWidth) / 2;
        int textY = blocks[i][j].y + 11;  // Vertical center
        
        // Draw the number
        u8g2.setCursor(textX, textY);
        u8g2.print(buf);
      }
    }
  }
}

// ===== RENDER GAME SCENSES =====
void draw_game() {
  u8g2.firstPage();
  do {
    // Draw the game board border (96x64 pixels)
    u8g2.drawFrame(0, 0, 96, 64);
    
    // Draw all the tiles
    draw_boxes();
    
    // Draw the score
    u8g2.setFont(u8g2_font_5x7_mf);
    u8g2.drawStr(100, 10, "Score");
    u8g2.setCursor(100, 20);
    u8g2.print(score);
    
  } while (u8g2.nextPage());  
}

void draw_gameover(){
  u8g2.firstPage();
  do {
    
    u8g2.setFont(u8g2_font_7x14B_tr);
    u8g2.drawStr(100, 10, "GameOver");
  }while (u8g2.nextPage());
}