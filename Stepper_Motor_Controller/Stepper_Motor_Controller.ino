#include <Stepper.h>

// TODO: check if it is at home at start up INPUT_PULLUP
// TODO: determine if each drawing is going to fit on the canvas.
// Changing RES effects scaling.
#define RES 1000
#define DIVISION 15
#define DBG 1

enum shapeType {
  line = 1,
  rectangle,
  circle,
  arc,
  ellipse,
  ellipticArc
};

// TODO: create structure type for thickness for speed settings and cut wait time.
// eighth
// quarter
// threeEighth
// half
// threeQuarter
// oneInch

Stepper motor1(200, 7, 8);
Stepper motor2(200, 9, 10);
int control1 = 6;
int control2 = 11;
int rpm = 2;
int curX = 0;
int curY = 0;
int listCount = 0;
int thickness;
boolean dirX;
boolean dirY;
shape *drawing;
float list[35][5];
// TODO: make sure param count is right
unsigned char shapeParam[5] = {0, 12, 12, 9, 15};

void draw();
void drawLine(float startX, float startY, float endX, float endY);
void drawRectangle(float startX, float startY, float width, float height, float degree);
void drawCircle(float centerX, float centerY, float radius);
void drawArc(float centerX, float centerY, float radius, float startDegree, float endDegree);
void drawEllipse();
void drawEllipticArc();
void moveTo(int x, int y);
void zeroSystem();

void setup() {
  pinMode(control1, OUTPUT);
  pinMode(control2, OUTPUT);
  digitalWrite(control1, LOW);
  digitalWrite(control2, LOW);
  motor1.setSpeed(rpm);
  motor2.setSpeed(rpm);
  Serial.begin(9600);
  Serial1.begin(9600);
  zeroSystem();
}
// 220 steps ~ 1 inch.
// 10 steps ~ 1/64 inch.
// about 20 steps for backlash.

int gcd(int a, int b) {
  int c;
   
  while (a) {
    c = a;
    a = b % a;
    b = c;
  }
   
  return b;
}

void zeroSystem() {
   
}

// TODO: change motor speed before cutting.
void draw() {
  int i, j;
  
  Serial.println("Drawing...");
  Serial.print("listCount: ");
  Serial.println(listCount);
  for(i = 0; i < listCount; i++) {
    Serial.println((int)list[i][0]);
    switch ((int)list[i][0]) {
      case line:
        if(DBG) {
          Serial.print("drawLine(");
          for(j = 1; j < 4; j++) {
            Serial.print(list[i][j], 5);
            Serial.print(", ");
          }
          Serial.print(list[i][j], 5);
          Serial.println(")");
        }
        // drawLine(list[i][1], list[i][2], list[i][3], list[i][4]);
        break;
      case rectangle:
        if(DBG) {
          Serial.print("drawRectangle(");
          for(j = 1; j < 5; j++) {
            Serial.print(list[i][j], 5);
            Serial.print(", ");
          }
          Serial.print(list[i][j], 5);
          Serial.println(")");
        }
        //drawRectangle(list[i][1], list[i][2], list[i][3], list[i][4], list[i][5]);
        break;
      case circle:
        if(DBG) {
          Serial.print("drawCircle(");
          for(j = 1; j < 3; j++) {
            Serial.print(list[i][j], 5);
            Serial.print(", ");
          }
          Serial.print(list[i][j], 5);
          Serial.println(")");
        }
        //drawCircle(list[i][1], list[i][2], list[i][3]);
        break;
      case arc:
        if(DBG) {
          Serial.print("drawArc(");
          for(j = 1; j < 5; j++) {
            Serial.print(list[i][j], 5);
            Serial.print(", ");
          }
          Serial.print(list[i][j], 5);
          Serial.println(")");
        }
        //drawArc(list[i][1], list[i][2], list[i][3], list[i][4], list[i][5]);
        break;
      case ellipse:
        break;
      case ellipticArc:
        break;
      default:
        // TODO: something is wrong.
        break;
    }
  }
}

void moveTo(int x, int y) {
  int steps;
   
  digitalWrite(control1, HIGH);
  digitalWrite(control2, HIGH);
  
  steps = (x / DIVISION) - curX;   // WARNING: weird type issure if x and curX is flipped.
  motor1.step(steps);
  curX += steps;
   
  steps = (y / DIVISION) - curY;
  motor2.step(steps);
  curY += steps;
  Serial.println("moveTo Done!");
}

void drawLine(float startX, float startY, float endX, float endY) {
  int sX = startX * RES;
  int sY = startY * RES;
  int eX = endX * RES;
  int eY = endY * RES;
  int steps, deltaX, deltaY;
  int i, rise, run, divisor;
  int totalRun = 0;
  int totalRise = 0;
  
   // move to start location.
  Serial.print("sX: ");
  Serial.println(sX);
  Serial.print("sY: ");
  Serial.println(sY);
  moveTo(sX, sY);
  Serial.print("startX: ");
  Serial.println(curX);
  Serial.print("startY: ");
  Serial.println(curY);
  
  if (sX == eX) {                   // vertical line
    steps = (eY - sY) / DIVISION;
    motor1.step(steps);
    curX += steps;
  } else if (sY == eY) {            // horizontal line
    steps = (eX - sX) / DIVISION;
    motor2.step(steps);
    curY += steps;
  } else {                          // sloped line
    rise = eY - sY;
    run = eX - sX;
    Serial.print("run: ");
    Serial.println(run);
    Serial.print("rise: ");
    Serial.println(rise);
    divisor = abs(gcd(rise, run));
    if (divisor != 1) {
      rise /= divisor;
      run /= divisor;
    }
    deltaX = (eX - sX) / DIVISION;
    deltaY = (eY - sY) / DIVISION;
    Serial.print("run: ");
    Serial.println(run);
    Serial.print("rise: ");
    Serial.println(rise);
    Serial.print("divisor: ");
    Serial.println(divisor);
      //TODO: maybe we should use divisor as counter in a for loop
      //while (abs(totalRise) < abs(deltaY) && abs(totalRun) < abs(deltaX)) {
      // Divisor doesn't tell me anything about where it needs to go. maybe divide by RES
    for (i = 0; i < divisor/DIVISION; i++) {
      motor1.step(run);
      //totalRun += run;
      curX += run;
      motor2.step(rise);
      //totalRise += rise;
      curY += rise;
    }
    Serial.print(abs(totalRise));
    Serial.print(" < ");
    Serial.println(deltaY);
    Serial.print(abs(totalRun));
    Serial.print(" < ");
    Serial.println(deltaX);
  }
  Serial.print("endX: ");
  Serial.println(curX);
  Serial.print("endY: ");
  Serial.println(curY);
  digitalWrite(control1, LOW);
  digitalWrite(control2, LOW);
  Serial.println("Done!");
}

// TODO: needs to be tested
void drawRectangle(float startX, float startY, float width, float height, float degree) {
  int sX = startX * RES;
  int sY = startY * RES;
  int w = width * RES;
  int h = height * RES;
  int steps;
  
  moveTo(sX, sY);
  
  steps = w / DIVISION;
  motor1.step(steps);
  curX += steps;
  
  steps = h / DIVISION;
  motor2.step(steps);
  curY += steps;
  
  steps = w / DIVISION;
  motor1.step(-steps);
  curX -= steps;
  
  steps = h / DIVISION;
  motor2.step(-steps);
  curY -= steps;
  
  digitalWrite(control1, LOW);
  digitalWrite(control2, LOW);
}

void drawCircle(float centerX, float centerY, float radius) {
  int x = centerX * RES;
  int y = centerY * RES;
  int r = radius * RES;
  int i, divisor, degree, nextX, nextY;
  int deltaX = 0, deltaY = 0;
   
  // TODO: center might be wrong...
  moveTo(x + r, y);
  Serial.print("startX: ");
  Serial.println(curX);
  Serial.print("startY: ");
  Serial.println(curY);
  // TODO: #define 361
  for (degree = 0; degree < 361; degree++) {
//   if (degree == 0 || degree == 90 || degree == 180 || degree == 270) {
//      digitalWrite(control1, LOW);
//      digitalWrite(control2, LOW);
//      while (!Serial.available());
//      Serial.read();
//      digitalWrite(control1, HIGH);
//      digitalWrite(control2, HIGH);
//    }
    Serial.print("Degree: ");
    Serial.println(degree);
    nextX = ((centerX + radius * cos(degree * (PI / 180))) * RES) / DIVISION;
    nextY = ((centerY + radius * sin(degree * (PI / 180))) * RES) / DIVISION;
//      Serial.print("nextX: ");
//      Serial.println(nextX);
//      Serial.print("nextY: ");
//      Serial.println(nextY);
    deltaX = nextX - curX;
    deltaY = nextY - curY;
    if (deltaX && deltaY) {
      divisor = abs(gcd(deltaX, deltaY));
    } else {
      divisor = 1;
    }
    Serial.print("NextX: ");
    Serial.println(nextX);
    Serial.print("CurX: ");
    Serial.println(curX);
    motor1.step(deltaX);
    // TODO: this needs to be flip to negative.
    motor2.step(deltaY);
    curX += deltaX;
    curY += deltaY;
    if (divisor != 1) {
      deltaX /= divisor;
      deltaY /= divisor;
    }
    Serial.print("(");
    Serial.print(deltaX);
    Serial.print(", ");
    Serial.print(deltaY);
    Serial.println(")");
    Serial.print("divisor: ");
    Serial.println(divisor);
    // TODO: need to fix this somehow. use top steps for backup.
//      for (i = 0; i < divisor; i++) {
//         if (!i) {
//            Serial.println("Second for loop");
//         }
//         motor1.step(deltaX);
//         motor2.step(deltaY);
//         curX += deltaX;
//         curY += deltaY;
//      }
   //delay(5000);
  }
   
  Serial.print("endX: ");
  Serial.println(curX);
  Serial.print("endY: ");
  Serial.println(curY);
   
  digitalWrite(control1, LOW);
  digitalWrite(control2, LOW);
  Serial.println("Done!");
}

void drawArc(float centerX, float centerY, float radius, float startDegree, float endDegree) {

}

void drawEllipse() {

}

void drawEllipticArc() {

}

void loop() {
  int exit, loopCount, code, i, j, k;
  int buffer[20];
  boolean cut = false;
  boolean convert = false;

  if(Serial1.available() > 1) {
    code = Serial1.read() << 8;
    code |= Serial1.read();
    
    if(DBG) {
      Serial.print("code: ");
      Serial.println(code);
    }
    
    i = 0;
    convert = false;
    if(code == 0) {
      exit = 0;
      while(!exit) {
        if(Serial1.available() > 1) {
          thickness = Serial1.read() << 8;
          thickness |= Serial1.read();
          exit = 1;
          if(DBG) {
            Serial.print("thickness: ");
            Serial.println(thickness, BIN);
          }
        }
      }
    } else if(code >= line && code <= ellipticArc) {
      buffer[i++] = code;
      while(i <= shapeParam[code]) {
        if(Serial1.available() > 1) {
          buffer[i] = Serial1.read() << 8;
          buffer[i++] |= Serial1.read();
        }
      }
      convert = true; 
    } else if(code == 0xFFFF) {
      cut = true;
    } else {
      // TODO: something is wrong. blinking LED or something
    }

    if(cut) {
      draw();
      listCount = 0;
      // TODO: send interrupt to screen.
    } else if(convert) {
      loopCount = shapeParam[code] / 3;
      list[listCount][0] = buffer[0];

      for(j = 0; j <= loopCount; j++) {
        list[listCount][j + 1] = buffer[(j * 3) + 3];
        while((int) list[listCount][j + 1]) {
          list[listCount][j + 1] /= 10;
        }
        for(k = 0; k < buffer[(j * 3) + 2]; k++) {
          list[listCount][j + 1] /= 10;
        }
        list[listCount][j + 1] += buffer[(j * 3) + 1];
        Serial.println(list[listCount][j + 1]);
      }
      
      if(loopCount) {
        listCount++;
      }
    }
  }
}
