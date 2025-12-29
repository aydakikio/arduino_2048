#include <U8g2lib.h>

// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, -1, A5, A4);

#define BTN_ACTION 12
#define BTN_DOWN 9
#define BTN_UP 6
#define BTN_RESET 4

//Game objects 
struct Block{
  int x;
  int y;
  int w;
  int h;
};

Block blocks[20];
int score=2;

//Forward declaration 
void darw_game();
void initialize_game();


void setup() {

  u8g2.begin();


  // Initialize button pins
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
}

void loop() {
  darw_game();
}



void darw_game(){
  u8g2.firstPage();
  do {

    u8g2.drawFrame(0,0, 16, 16);
    u8g2.setFont(u8g2_font_5x7_mf);
    u8g2.setCursor(5, 11);
    u8g2.print(score);

    u8g2.drawFrame(0, 0, 96, 64);

  } while (u8g2.nextPage());
}
