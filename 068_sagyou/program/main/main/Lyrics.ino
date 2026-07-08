// =====================
// 音ごとの歌詞パーツ
// Processing側の melody[] / drumType[] と同じ順番・同じ数にする
// =====================

const char* lyricParts[] = {
  "KA", "E", "RU", "NO", "U", "TA", "GA",

  "KI", "KO", "E", "TE", "KU", "RU", "YO",

  "GUWA", "GUWA", "GUWA", "GUWA",

  "KE", "RO", "KE", "RO", "KE", "RO", "KE", "RO",

  "GUWA", "GUWA", "GUWA"
};

const int lyricPartCount = sizeof(lyricParts) / sizeof(lyricParts[0]);


// =====================
// 表示部分
// =====================

uint8_t frame[8][12];

bool guwaPending = false;
unsigned long guwaStartTime = 0;
const unsigned long guwaSplitMs = 180;

const uint8_t font[][5] = {
  {0, 0, 0, 0, 0},        // space
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
  {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
  {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
  {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X
  {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
  {0x61, 0x51, 0x49, 0x45, 0x43}  // Z
};

int getFontIndex(char c) {
  if (c == ' ') return 0;
  if (c >= 'A' && c <= 'Z') return c - 'A' + 1;
  if (c >= 'a' && c <= 'z') return c - 'a' + 1;
  return 0;
}

int getTextWidth(const char* text) {
  int count = 0;

  while (text[count] != '\0') {
    count++;
  }

  return count * 6;
}

bool getPixelFromText(const char* text, int x, int y) {
  if (x < 0) return false;

  int charIndex = x / 6;
  int column = x % 6;

  if (text[charIndex] == '\0') return false;
  if (column == 5) return false;

  int fontIndex = getFontIndex(text[charIndex]);

  return bitRead(font[fontIndex][column], y);
}

void clearFrame() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 12; x++) {
      frame[y][x] = 0;
    }
  }
}

void showText(const char* text) {
  clearFrame();

  int textWidth = getTextWidth(text);
  int startX = (12 - textWidth) / 2;

  if (startX < 0) {
    startX = 0;
  }

  for (int y = 0; y < 7; y++) {
    for (int x = 0; x < 12; x++) {
      int textX = x - startX;
      frame[y][x] = getPixelFromText(text, textX, y);
    }
  }

  matrix.renderBitmap(frame, 8, 12);
}

void showLyricPart(int index) {
  if (index < 0 || index >= lyricPartCount) return;

  if (strcmp(lyricParts[index], "GUWA") == 0) {
    showText("GU");
    guwaPending = true;
    guwaStartTime = millis();
  } else {
    guwaPending = false;
    showText(lyricParts[index]);
  }
}

void initLyricsDisplay() {
  showText("HB");
}

void updateLyricDisplay() {
  if (guwaPending && millis() - guwaStartTime >= guwaSplitMs) {
    showText("WA");
    guwaPending = false;
  }

  while (Serial.available() > 0) {
    int command = Serial.read();

    if (command >= 0 && command < lyricPartCount) {
      showLyricPart(command);
    }

    if (command == 100) {
      guwaPending = false;
      clearFrame();
      matrix.renderBitmap(frame, 8, 12);
    }
  }
}