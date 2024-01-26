int val = 0;
int muxPins1[] = {2, 3, 4, 5};
int sensorPin1 = A4;
const int N_KEYS = 13;

byte MESSAGE_VAL[N_KEYS] = {61, 60, 63, 62, 64, 65, 66, 67, 69, 71, 72, 68, 70};

int buttonCState[N_KEYS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int buttonPState[N_KEYS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Octaves
const byte OCTAVE_BUTTON_PIN[2] = {6, 7};
int octave = 0;

int octaveCState[2] = {0, 0};
int octavePState[2] = {0, 0};

// Slider
int SLIDER_PIN = A1;
int sliCState = 0;
int sliPState = 0;

// Pots
const byte N_POTS = 3;
int POT_PIN[N_POTS] = {A1, A2, A3};
int potCState[N_POTS] = {0, 0, 0};
int potPState[N_POTS] = {0, 0, 0};
int POT_VALS[N_POTS] = {0x01, 0x07, 0x0A};

// Pads
const byte N_PADS = 8;
int PAD_PIN = A0;
int muxPinsPad[] = {11, 10, 9, 8};
int padCState[N_PADS] = {0, 0, 0, 0, 0, 0, 0, 0};
int padPState[N_PADS] = {0, 0, 0, 0, 0, 0, 0, 0};
int PADS_MIN[N_PADS] = {100, 130, 80, 170, 60, 120, 1000, 170};
int PADS_MAX[N_PADS] = {210, 205, 220, 220, 180, 190, 900, 310};
int PAD_VAL[N_PADS] = {55, 56, 57, 58, 48, 50, 52, 53};
int vel;
int diff;

void setup() {
  Serial.begin(31250);
  // Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(muxPins1[i], OUTPUT);
  }
  pinMode(sensorPin1, INPUT_PULLUP);

  for (int i = 0; i < 4; i++) {
    pinMode(muxPinsPad[i], OUTPUT);
  }

  for (int i = 0; i < 2; i++) {
    pinMode(OCTAVE_BUTTON_PIN[i], INPUT_PULLUP);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  handleKeys();
  handleOctaves();
  handlePots();
  handlePads();
}

void handleKeys() {
  for (int i = 0; i < N_KEYS; i++) {
    buttonCState[i] = pullupReadMux(i, muxPins1, sensorPin1);

    if (buttonCState[i] != buttonPState[i]) {
      if (buttonCState[i] == 1) {
        send(0x90, MESSAGE_VAL[i]+octave, 64);
      }
      else if (buttonCState[i] == 0) {
        send(0x90, MESSAGE_VAL[i]+octave, 0);
      }  
    }
    buttonPState[i] = buttonCState[i];
  }
}


void handleOctaves() {
  for (int i = 0; i < 2; i++) {
    octaveCState[i] = pullupRead(OCTAVE_BUTTON_PIN[i]);

    if (octaveCState[i] != octavePState[i] && octaveCState[i] == 1) {

      if (i == 0) {
        octave -= 12;
      }
      else if (i == 1){
        octave += 12;
      }
      
    }
    octavePState[i] = octaveCState[i];
  }
}

void handlePots() {
  for (int i = 0; i < N_POTS; i++) {
    potCState[i] = map(analogRead(POT_PIN[i]), 0, 1023, 0, 127);

    if (potCState[i] != potPState[i]) {
      send(0xB0+1, POT_VALS[i], potCState[i]);
    }
    potPState[i] = potCState[i];
  }
}


void handlePads() {
  for (int i = 0; i < N_PADS; i++) {
    padCState[i] = analogReadMux(i, muxPinsPad, PAD_PIN);
    diff = padCState[i] - padPState[i];
    if ((diff > 5 || diff < -5) && padCState[i] > PADS_MIN[i]) {
      
      vel = map(padCState[i], PADS_MIN[i], PADS_MAX[i], 0, 127);
      if (vel > 127) {
        vel = 127;
      }
      send(0x90, PAD_VAL[i], vel);
    }
    else if (diff < -10 ) {
      send(0x90, PAD_VAL[i], 0);
    }
    padPState[i] = padCState[i];
  }
}





// UTILITY FUNCTIONS

int pullupRead(int pin) {
  return 1-digitalRead(pin);
}

int analogReadMux(int number, int muxPins[], int sensorPin){
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxPins[i], bitRead(number, i));
  }

  int value = analogRead(sensorPin);
  return value;
}

int digitalReadMux(int number, int muxPins[], int sensorPin){
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxPins[i], bitRead(number, i));
  }

  int value = digitalRead(sensorPin);
  return value;
}

int pullupReadMux(int number, int muxPins[], int sensorPin){
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxPins[i], bitRead(number, i));
  }

  int value = pullupRead(sensorPin);
  return value;
}


void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

//void send(byte cmd, byte db) {
//  Serial.write(cmd);
//  Serial.write(db);
//}

void send(byte cmd, byte db1, byte db2) {
  Serial.write(cmd);
  Serial.write(db1);
  Serial.write(db2);
}

void allOff() {
  for (int i = 0; i < 128; i++) {
    send(0x90, i, 0);
  }
}
