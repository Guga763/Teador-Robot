#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NewPing.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define TRIGGER_PIN 9
#define ECHO_PIN 8
#define MAX_DISTANCE 200

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Переменные состояния и таймеров
int angrycount = 0;
bool blangry = false;

unsigned long sleepTimer = 0;
bool isSleep = false;
const unsigned long SLEEP_TIMEOUT = 10000; // 10 секунд бездействия до засыпания

unsigned long blinkTimer = 0;
unsigned long zzzTimer = 0;
int zzzOffset = 0; // Смещение для непрерывной анимации Zzz

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.clearDisplay();
  sleepTimer = millis();
}

// Базовая функция рисования глаза EMO
void drawEmoEye(int x, int y, int w, int h, int r) {
  display.fillRoundRect(x, y, w, h, r, SSD1306_WHITE);
}

// ==========================================
// НАБОР ЭМОЦИЙ И АНИМАЦИЙ (ЖИВАЯ ДИНАМИКА)
// ==========================================

// 1. Обычный взгляд
void drawEmoNormal(int offsetX = 0, int offsetY = 0) {
  display.clearDisplay();
  drawEmoEye(24 + offsetX, 11 + offsetY, 30, 42, 10);
  drawEmoEye(74 + offsetX, 11 + offsetY, 30, 42, 10);
  display.display();
}

// 2. Любопытство (8–20 см): Плавное «вытягивание» глаз к объекту
void drawEmoCurious() {
  for (int i = 0; i <= 6; i += 2) {
    display.clearDisplay();
    drawEmoEye(24 - (i / 2), 11 - i, 30 + i, 42 + i, 10);
    drawEmoEye(74 - (i / 2), 11 - i, 30 + i, 42 + i, 10);
    display.display();
    delay(15);
  }
}

// 3. Испуг / Шок (< 8 см): Дрожание (Jitter) + расширенные зрачки
void drawEmoScared() {
  for (int j = 0; j < 3; j++) {
    int shakeX = random(-2, 3);
    int shakeY = random(-2, 3);

    display.clearDisplay();
    drawEmoEye(24 + shakeX, 7 + shakeY, 28, 49, 10);
    drawEmoEye(76 + shakeX, 7 + shakeY, 28, 49, 10);
    
    display.fillRoundRect(33 + shakeX, 23 + shakeY, 10, 16, 4, SSD1306_BLACK);
    display.fillRoundRect(85 + shakeX, 23 + shakeY, 10, 16, 4, SSD1306_BLACK);
    display.display();
    delay(20);
  }
}

// 4. Счастье / Радость (20–35 см): Кинематографичный прищур
void drawEmoHappy() {
  for (int cut = 0; cut <= 24; cut += 6) {
    display.clearDisplay();
    drawEmoEye(24, 11, 30, 42, 10);
    drawEmoEye(74, 11, 30, 42, 10);
    
    display.fillRect(15, 53 - cut, 98, cut + 5, SSD1306_BLACK);
    display.display();
    delay(15);
  }
}

// 5. Моргание
void emoBlink() {
  for (int h = 42; h >= 4; h -= 10) {
    display.clearDisplay();
    int y = 32 - (h / 2);
    drawEmoEye(24, y, 30, h, h < 10 ? 2 : 8);
    drawEmoEye(74, y, 30, h, h < 10 ? 2 : 8);
    display.display();
    delay(10);
  }
  for (int h = 4; h <= 42; h += 10) {
    display.clearDisplay();
    int y = 32 - (h / 2);
    drawEmoEye(24, y, 30, h, h < 10 ? 2 : 8);
    drawEmoEye(74, y, 30, h, h < 10 ? 2 : 8);
    display.display();
    delay(10);
  }
}

// 6. Анимация злости
void animateAngry() {
  for (int i = 0; i <= 15; i += 3) {
    display.clearDisplay();
    drawEmoEye(24, 15, 30, 38, 8);
    drawEmoEye(74, 15, 30, 38, 8);
    
    // Брови
    display.fillTriangle(20, 10 + i, 60, 25 + i, 20, 25 + i, SSD1306_BLACK);
    display.fillTriangle(108, 10 + i, 68, 25 + i, 108, 25 + i, SSD1306_BLACK);

    display.display();
    delay(30);
  }
  delay(1500);
}

// 7. Однократное плавное закрытие глаз при засыпании
void animateCloseEyes() {
  for (int h = 38; h >= 4; h -= 4) {
    display.clearDisplay();
    int y = 15 + (38 - h) / 2;
    display.fillRoundRect(24, y, 30, h, 4, SSD1306_WHITE);
    display.fillRoundRect(74, y, 30, h, 4, SSD1306_WHITE);
    display.display();
    delay(30);
  }
}

// 8. НЕПРЕРЫВНАЯ АНИМАЦИЯ Zzz (Вызывается постоянными циклами)
void drawContinuousSleepZzz() {
  display.clearDisplay();

  // Спящие глаза-щёлочки
  display.fillRoundRect(24, 32, 30, 4, 2, SSD1306_WHITE);
  display.fillRoundRect(74, 32, 30, 4, 2, SSD1306_WHITE);

  display.setTextColor(SSD1306_WHITE);

  // Маленькая 'z'
  display.setTextSize(1);
  display.setCursor(104, 28 - zzzOffset);
  display.print("z");

  // Средняя 'Z'
  if (zzzOffset >= 4) {
    display.setTextSize(1);
    display.setCursor(112, 20 - zzzOffset);
    display.print("Z");
  }

  // Большая 'Z'
  if (zzzOffset >= 8) {
    display.setTextSize(2);
    display.setCursor(118, 12 - zzzOffset);
    display.print("Z");
  }

  display.display();

  // Таймер движения букв
  if (millis() - zzzTimer > 70) {
    zzzOffset += 2;
    if (zzzOffset > 18) { 
      zzzOffset = 0; // Сброс цикла: буквы заново вылетают снизу
    }
    zzzTimer = millis();
  }
}

// ==========================================
// ОСНОВНОЙ ЦИКЛ
// ==========================================

void loop() {
  int distance = sonar.ping_cm();
  unsigned long now = millis();

  // При обнаружении движения сбрасываем сон
  if (distance > 0 && distance <= 35) {
    sleepTimer = now;
    if (isSleep) {
      isSleep = false; // Просыпаемся!
      zzzOffset = 0;
      emoBlink();
    }
  }

  // 1. РЕЖИМ БЕСКОНЕЧНОГО СНА
  if (now - sleepTimer > SLEEP_TIMEOUT) {
    if (!isSleep) {
      animateCloseEyes(); // Плавно закрываем глаза
      isSleep = true;
    }
    drawContinuousSleepZzz(); // Бесконечно зацикленный полёт Zzz
  } 
  // 2. ИСПУГ / ЗЛОСТЬ (< 8 см)
  else if (distance > 0 && distance <= 8) {
    if (!blangry) {
      blangry = true;
      angrycount++;
    }

    if (angrycount >= 3) {
      animateAngry();
      angrycount = 0;
    } else {
      drawEmoScared();
    }
  } 
  // 3. ЛЮБОПЫТСТВО (8–20 см)
  else if (distance > 8 && distance <= 20) {
    blangry = false;
    drawEmoCurious();
  } 
  // 4. РАДОСТЬ (20–35 см)
  else if (distance > 20 && distance <= 35) {
    blangry = false;
    drawEmoHappy();
  } 
  // 5. ОБЫЧНОЕ СОСТОЯНИЕ
  else {
    blangry = false;
    
    if (now - blinkTimer > 3000) {
      int randAction = random(0, 3);
      if (randAction == 0) {
        emoBlink();
      } else if (randAction == 1) {
        drawEmoNormal(-10, 0);
        delay(400);
      } else {
        drawEmoNormal(10, 0);
        delay(400);
      }
      blinkTimer = now;
    } else {
      drawEmoNormal(0, 0);
    }
  }

  delay(30);
}
