/***********************************************************
Touch board mapping for keyboard
based on Bare Conductive example code
************************************************************/
// touch includes
#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>

// keyboard includes
#include <Keyboard.h>

// keyboard variables
char key;

// keyboard behaviour constants
const bool HOLD_KEY = false;  // 
const char KEY_MAP[12] = {'Q', 'W', 'A', 'S', 'D', 'E', 'T', 'Y', 'U', 'I', 'O', 'P'};
// const char KEY_MAP[12] = {KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, ' ', KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_PAGE_UP, KEY_PAGE_DOWN};
// const char KEY_MAP[12] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'Q', 'W', 'E'};  // NUM

// touch constants
const uint32_t BAUD_RATE = 115200;
const uint8_t MPR121_ADDR = 0x5C;
const uint8_t MPR121_INT = 4;

// MPR121 datastream behaviour constants
const bool MPR121_DATASTREAM_ENABLE = false;

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!MPR121.begin(MPR121_ADDR)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }

  MPR121.setInterruptPin(MPR121_INT);

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121.restoreSavedThresholds();
    MPR121_Datastream.begin(&Serial);
  } else {
    MPR121.setTouchThreshold(40);
    MPR121.setReleaseThreshold(20);
  }

  MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);
  MPR121.setGlobalCDT(CDT_4US);  // reasonable for larger capacitances
  
  digitalWrite(LED_BUILTIN, HIGH);  // switch on user LED while auto calibrating electrodes
  delay(1000);
  MPR121.autoSetElectrodes();  // autoset all electrode settings
  digitalWrite(LED_BUILTIN, LOW);
  
  Keyboard.begin();
}

void loop() {
  MPR121.updateAll();

  for (int i=0; i < 12; i++) {  // check which electrodes were pressed
    key = KEY_MAP[i];
    
    if (MPR121.isNewTouch(i)) {
      digitalWrite(LED_BUILTIN, HIGH);
      Keyboard.press(key);  // press the appropriate key on the "keyboard" output

      if (!HOLD_KEY) {
        Keyboard.release(key);  // if we don't want to hold the key, immediately release it
      }
    } else {
      if (MPR121.isNewRelease(i)) {
        digitalWrite(LED_BUILTIN, LOW);

        if (HOLD_KEY) {
          Keyboard.release(key);  // if we have a new release and we were holding a key, release it
        }
      }
    }
  }

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
}
