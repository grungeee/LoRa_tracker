#define BUTTON 2
#define LED 7

void setup() {
 pinMode(LED, OUTPUT); 
 pinMode(BUTTON, INPUT_PULLUP); 
}

void loop() {
digitalRead(BUTTON == LOW) ? digitalWrite(LED, HIGH) : digitalWrite(LED, LOW);
}
