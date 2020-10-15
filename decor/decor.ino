int pin = 0;
int tt = 1;
int basetime = 100;
int pinSitu = HIGH;
int runpr = 0;
bool isRunning = false;
int runCount = 0;
int pinSituArray[5] = {LOW, LOW, LOW, LOW, LOW};

void setup() {
  // put your setup code here, to run once:
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}

void doCount(int runningTime){
  runCount += 1;
  if (runCount >= runningTime){
    isRunning = false;
  }
}

void doReset(){
  pin = 0;
  for (int i=0;i<5; i++){
    digitalWrite(i, LOW);
  }  
}

void doTurnOn(){
  for (int i=0; i<5 ; i++){
    digitalWrite(i, HIGH);
    delay(1000);
  }
}

void pr0(int rTime){
  pin = random(5);
  digitalWrite(pin, HIGH);
  delay(basetime * tt);
  //delay(1000);
  digitalWrite(pin, LOW);
  doCount(rTime);
}

void pr1(int rTime){
    if (pin>4) {
      pin = 0;
      doCount(rTime);
    }
    digitalWrite(pin, HIGH);
    delay(basetime * tt);
    digitalWrite(pin, LOW);
    pin +=1;
}

void pr2(int rTime){
  
  if (pin>4){
    pinSitu = LOW;
    doCount(rTime);
    pin = 4;
  }
  if (pin<0){
    pinSitu = HIGH;
    pin = 0;
  }
  
  digitalWrite(pin, pinSitu);
  delay(basetime * tt);

  if (pinSitu == HIGH) pin += 1;
  if (pinSitu == LOW) pin -= 1; 
}

void pr3(int rTime){
  bool AllOn = true;
  
  for (int i=0; i<5; i++){
      if (pinSituArray[i] == LOW) AllOn = false;      
  }
  if (AllOn == true){
    if (pinSitu == HIGH){ 
        pinSitu = LOW;
      } 
      else {
        pinSitu = HIGH;
        doCount(rTime);
        
      }
    for (int i=0; i<5; i++){
      pinSituArray[i] = LOW;
    }
    
  } else {
    int pinToTurn = random(0,5);
    
    while (pinSituArray[pinToTurn] == HIGH) {
      pinToTurn = random(0,5);
    }
    digitalWrite(pinToTurn, pinSitu);
    pinSituArray[pinToTurn] = HIGH;
    delay(basetime * tt);
  }
}

void loop() {
  /*
  doTurnOn();
  delay(1000);
  doReset();
  delay(1000);
  */
  if (isRunning == false) {
    doReset();
    runpr = random(0,4);
    isRunning = true;
    runCount = 0;
    pin=0;
    pinSitu =HIGH;
    tt = random(1,101);
  }

  if (runpr == 0) {
    pr0(5);
  }
  if (runpr == 1) {
    pr1(2);
  }
  if (runpr == 2) {
    pr2(2);
  }
  if (runpr == 3) {
    pr3(2);
  }

}
