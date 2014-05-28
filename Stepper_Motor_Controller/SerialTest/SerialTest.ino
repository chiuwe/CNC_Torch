unsigned char shapeParam[5] = {0, 13, 0, 0, 0};
int buffer[50];

void setup() {
  char i;
  int test[3] = {4, 2, 34};
  float result;
  
  Serial.begin(9600);
  Serial1.begin(9600);
  result = test[2];
  while((int) result) {
    result /= 10;
  }
  for(i = 0; i < test[1]; i++) {
    result /= 10;
  }
  result += test[0];
  Serial.println(result, 5);
}

void loop() {
  int shapeCode, j, i = 0;
 
  if(Serial1.available() > 1) {
    shapeCode = Serial1.read() << 8;
    shapeCode |= Serial1.read();
    
    Serial.print("shapeCode: ");
    Serial.println(shapeCode);
    
    switch(shapeCode) {
      case 1:
        Serial.println("case1");
        buffer[i++] = shapeCode;
        delay(1000);
        while(Serial1.available() > 1 && i < shapeParam[1]) {
          buffer[i] = Serial1.read() << 8;
          buffer[i] |= Serial1.read();
          Serial.println(buffer[i - 1]);
          i++;
        }
      break;
      case 2:
      break;
      case 0xFFFF:
        Serial.println("CUT!");
      break;
    }

    Serial.print("Available: ");
    Serial.println(Serial1.available());
    
    int loopCount = shapeParam[1] / 3;
    float result;
    
    Serial.print("loopCount: ");
    Serial.println(loopCount);
  
    for(i = 0; i < loopCount; i++) {
      result = buffer[(i * 3) + 3];
      while((int) result) {
        result /= 10;
      }
      for(j = 0; j < buffer[(i * 3) + 2]; j++) {
        result /= 10;
      }
      result += buffer[(i * 3) + 1];
      Serial.println(result, 5);
    }
  }
} 
