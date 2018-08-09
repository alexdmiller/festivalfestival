#include <FastLED.h>

#define LED_PIN     7
#define NUM_RODS          8
#define LEDS_PER_SIDE    18
#define NUM_ROWS         (LEDS_PER_SIDE + 2)
#define LEDS_PER_ROD     (LEDS_PER_SIDE * 4 + 2)
#define NUM_LEDS         (NUM_RODS * LEDS_PER_ROD)
#define NUM_MODES        9
#define TICKS_PER_MODE   300
#define FADE_TICKS             80

#define BRIGHTNESS  255

#define LED_TYPE    WS2813
#define COLOR_ORDER GRB

#define NUM_MODES 4

CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

extern const TProgmemPalette16 blackAndWhite PROGMEM;

const TProgmemPalette16 blackAndWhite PROGMEM =
{
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    CRGB::Black
};


extern const TProgmemPalette16 colorful PROGMEM;

const TProgmemPalette16 colorful PROGMEM =
{
    CRGB::Cyan,
    CRGB::Cyan,
    CRGB::Cyan,
    CRGB::Cyan,
    
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    
    CRGB::Cyan,
    CRGB::Cyan,
    CRGB::Cyan,
    CRGB::Cyan,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red
};

extern const TProgmemPalette16 colorful2 PROGMEM;

const TProgmemPalette16 colorful2 PROGMEM =
{
    CRGB::Purple,
    CRGB::Purple,
    CRGB::Purple,
    CRGB::Purple,
    
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    CRGB::Black
};


int mode = 0;
int tick = 0;
int ran4 = 0;
int totalModes = 0;

CRGBPalette16 currentPalette;

void setup() {
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  currentPalette = RainbowColors_p;
}

void setRow(int rod, int row, CRGB color) {
  for (int i = 0; i < 4; i++) {
    leds[getPixelIndex(rod, i, row)] = color;
  }
}

void setSide(int rod, int side, CRGB color) {
  for (int row = 0; row < NUM_ROWS; row++) {
    leds[getPixelIndex(rod, side, row)] = color;
  }
}

void setRod(int rod, CRGB color) {
  for (int side = 0; side < 4; side++) {
    for (int row = 0; row < NUM_ROWS; row++) {
      leds[getPixelIndex(rod, side, row)] = color;
    }
  }
}

void setPair(int pair, CRGB color) {
  setRod(pair, color);
  setRod(pair + 4, color);
}

void setAxis(int axis, CRGB color) {
  setRod(axis, color);
  setRod((axis + 2) % 4 + 4, color);
}

void setAxisRow(int axis, int row, CRGB color) {
  if (row < NUM_ROWS) {
    setRow(axis, NUM_ROWS - row - 1, color);
  } else {
    setRow((axis + 2) % 4 + 4, row - NUM_ROWS, color);
  }
}

void setSlice(int slice, CRGB color) {
  int rod = 0;

  if (slice < NUM_ROWS) {
    for (rod = 0; rod < 4; rod++) {
      setRow(rod, NUM_ROWS - slice - 1, color);
    }
  } else {
    for (rod = 0; rod < 4; rod++) {
      setRow(rod + 4, slice - NUM_ROWS, color);
    }
  }
}

int getPixelIndex(int rod, int side, int row) {
  // temporary!
  if (rod == 1) {
    rod = 2;
  } else if (rod == 2) {
    rod = 3;
  } else if (rod == 3) {
    rod = 1;
  }

  int rodOffset = LEDS_PER_ROD * rod;

  if (row == 0) {
    // front edge pixel
    return rodOffset + LEDS_PER_SIDE * 4 + 1;
  } else if (row <= 18) {
    // normal side pixels
    int sideOffset = 0;
    int rowOffset = 0;
    row = row - 1;

    if (side == 0) {
      sideOffset = 0;
      rowOffset = row;
    } else if (side == 1) {
      sideOffset = LEDS_PER_SIDE * 2;
      rowOffset = -row;
    } else if (side == 2) {
      sideOffset = LEDS_PER_SIDE * 2 + 1;
      rowOffset = row;
    } else if (side == 3) {
      sideOffset = LEDS_PER_SIDE * 4;
      rowOffset = -row;
    }

    return rodOffset + sideOffset + rowOffset;
  } else if (row == 19) {
    // back edge pixel
    return rodOffset + LEDS_PER_SIDE;
  } else {
    // error?
    return -1;
  }
}

void fadeIn() {
  if (tick < FADE_TICKS) {
    int fade = 255 - 255 * (float) tick / FADE_TICKS;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].fadeToBlackBy(fade);
    }
  }
}

void fadeOut(int endTick) {
  if (tick > endTick - FADE_TICKS) {
    int fade = 255 - 255 * (float) (endTick - tick) / FADE_TICKS;
    if (tick > endTick) {
      fade = 255;
    }
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].fadeToBlackBy(fade);
    }
  }
}

void loop()
{  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black; //ColorFromPalette(currentPalette, tick, BRIGHTNESS, LINEARBLEND);
  }
  
  mode = 10;
  
  if (mode == 0) {
    setSlice(tri(tick, NUM_ROWS * 2), CRGB::White);
    modeCheck(NUM_ROWS * 8);
  } else if (mode == 1) {
    setPair((tick / 8) % 4, CRGB::White);
    modeCheck(4 * 32);
  } else if (mode == 2) {
    setAxis((tick / 8) % 4, CRGB::White);
    modeCheck(4 * 32);
  } else if (mode == 3) {
    setAxisRow(ran4, tri(tick, NUM_ROWS * 2), CRGB::White);
    modeCheck(NUM_ROWS * 8);
  } else if (mode == 4) {
    for (int i = 0; i < NUM_ROWS * 2; i++) {
      setSlice(i, ColorFromPalette(currentPalette, i * 4 + tick, BRIGHTNESS, LINEARBLEND));
    }

    fadeIn(); fadeOut(NUM_ROWS * 16);
    modeCheck(NUM_ROWS * 16);
  } else if (mode == 5) {
    for (int rod = 0; rod < NUM_RODS; rod++) {
      for (int side = 0; side < 4; side++) {
        setSide(rod, side, ColorFromPalette(currentPalette, side * 40 + tick / 2, BRIGHTNESS, LINEARBLEND));
      }
    }

    fadeIn(); fadeOut(NUM_ROWS * 32);
    modeCheck(NUM_ROWS * 32);
  } else if (mode == 6) {
    for (int rod = 0; rod < NUM_RODS; rod++) {
      for (int row = 0; row < NUM_ROWS; row++) {
        setRow(rod, row, ColorFromPalette(currentPalette, row * 2 + tick * 1, BRIGHTNESS, LINEARBLEND));
      }
    }

    fadeIn(); fadeOut(NUM_ROWS * 16);
    modeCheck(NUM_ROWS * 16);
  } else if (mode == 7) {
    for (int rod = 0; rod < 4; rod++) {
      setRod(rod, ColorFromPalette(currentPalette, tick * 1, BRIGHTNESS, LINEARBLEND));
    }

    for (int rod = 4; rod < 8; rod++) {
      setRod(rod, ColorFromPalette(currentPalette, 50 + tick * 1, BRIGHTNESS, LINEARBLEND));
    }
    
    fadeIn(); fadeOut(NUM_ROWS * 16);
    modeCheck(NUM_ROWS * 16);
  } else if (mode == 8) {
    for (int rod = 0; rod < NUM_RODS; rod++) {
      for (int row = 0; row < tri(tick, NUM_ROWS); row++) {
        setRow(rod, row, CRGB::White);
      }
    }

    modeCheck(NUM_ROWS * 2);
  } else if (mode == 9) {
    for (int rod = 0; rod < NUM_RODS; rod++) {
      for (int row = 0; row < tri(tick, NUM_ROWS); row++) {
        setRow(rod, NUM_ROWS - row - 1, CRGB::White);
      }
    }
    
    modeCheck(NUM_ROWS * 2);
  } else if (mode == 10) {
    for (int rod = 0; rod < NUM_RODS; rod++) {
      for (int row = 0; row < tri(tick + rod * 10, NUM_ROWS); row++) {
        setRow(rod, row, CRGB::White);
      }
    }

    modeCheck(NUM_ROWS * 2);
  } else if (mode == 0) {
    setSlice(tri(tick, NUM_ROWS * 2), CRGB::White);
    modeCheck(NUM_ROWS * 8);
  } else if (mode == 1) {
    setPair((tick / 8) % 4, CRGB::White);
    modeCheck(4 * 32);
  } else if (mode == 2) {
    setAxis((tick / 8) % 4, CRGB::White);
    modeCheck(4 * 32);
  } else if (mode == 3) {
    setAxisRow(ran4, tri(tick, NUM_ROWS * 2), CRGB::White);
    modeCheck(NUM_ROWS * 8);
  } 
  
  tick++;

  FastLED.show();
//  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void modeCheck(int thresh) {
  if (tick > thresh) {
    tick = 0;
    mode = random(6);
    ran4 = (ran4 + 1) % 4;
    totalModes++;

    switch (random(3)) {
      case 0: currentPalette = blackAndWhite;
      break;
      case 1: currentPalette = colorful;
      break;
      case 2: currentPalette = colorful2;
    }
  }
}

int tri(int t, int m) {
  if ((t / m) % 2 == 0) {
    return t % m;
  } else {
    return m - t % m - 1;
  }
}


 

