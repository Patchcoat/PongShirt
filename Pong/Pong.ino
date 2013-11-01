#include <Adafruit_NeoPixel.h>

#define PIN 6

//says which are the input pins for the game
int buttonPin = 40;
//dictates the score, kept in a bianary array because it is esier
int playerScore[5] = {0,0,0,0,0};
int compScore[5] = {0,0,0,0,0};
//dictates the ball's direction
int bvd=0;//0 is down, 1 is up
int bhd=0;//0 is right, 1 is left
//dictates the balls position
int bvp = 8;
int bhp = 6;
//checks to see if the ball is in fast mode
boolean fast = false;
//checks if it is a score. 
boolean cIsPoint = false;
boolean pIsPoint = false;
//paddle positions
int cPaddle = 0;
int pPaddle = 0;
long previousMillis = 0;
long previousMillisForComp = 0;
//set the delay times for fast and slow moving ball the "Int" in normInt and fastInt is for interval
int normInt = 50;
int fastInt = 10;
//used to check for a win. 1 is player win, -1 in computer win, and 0 is nothing has happend yet
int win = 0;
//to get rid of a bug. used in the addOne statement and nowhere else.
int which = 0;
//Thing displayed before start, -1 is an escape
int pongPlay[] = {1,2,3,4,10,11,12,13,16,18,20,27,29,33,34,41,42,61,62,63,70,
             71,72,76,85,86,88,91,92,94,100,101,102,103,131,132,136,
             139,144,145,146,147,148,161,162,166,175,183,185,192,
             194,195,196,197,198,199,204,205,206,207,208,-1};
//if you win
int youWin[] = {1,2,3,4,5,16,17,18,19,20,27,33,34,39,46,47,48,49,54,55,56,57,
                58,77,78,79,84,88,91,92,93,94,95,99,103,106,110,114,118,130,
                131,132,144,145,146,147,151,162,163,168,169,170,177,181,192,
                193,204,205,206,207,-1};
//if you lose
int youLose[] = {1,3,5,16,17,18,19,20,24,26,28, 31,32,32,33,34,35,39,46,47,48,49,
                 62,65,77,78,79,84,86,88,91,94,99,103,106,110,122,123,124,
                 130,131,132,144,148,152,153,154,162,163,168,169,170,181,192,
                 193,204,205,206,207,208,-1};
//sets up the led strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(210, PIN, NEO_RGB + NEO_KHZ800);

//setting up, turns no pin 40 for input, begins the strip, displays the
//message and the shows it.
void setup() {
  pinMode(buttonPin,INPUT);
  strip.begin();
  for (int i = 0 ; pongPlay[i]>0 ; i++){
    strip.setPixelColor(pongPlay[i], strip.Color(0, 0, 255));
  }
  strip.show();
  delay(5000);
  while (digitalRead(buttonPin) == LOW){
    //It's a trap!(until the button is pressed.)
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  clearScreen();
  //draws the paddles to show the score
  for(int i = 0 ; i < 5 ; i++){
    if (playerScore[i]==1){
      strip.setPixelColor(204-i, strip.Color(255, 255, 255));
    }else if((playerScore[i]==0)){
      strip.setPixelColor(204-i, strip.Color(0, 255, 0));
    }
    if (compScore[i]==1){
      strip.setPixelColor(5+i, strip.Color(255, 255, 255));
    }else if((compScore[i]==0)){
      strip.setPixelColor(5+i, strip.Color(255, 0, 0));
    }
  }
  //draws the ball in it's starting position
  strip.setPixelColor(98, strip.Color(255, 255, 255));
  strip.show();
  //decides if the ball is going up or going down
  while(pIsPoint == false && cIsPoint == false){
    clearScreen();
    unsigned long currentMillis = millis();
    //assigns the player paddle position
    pPaddle=checkPaddlePos();
    //checks the next position
    checkNext();
    //moving the ball
    if (fast == false){
      if(currentMillis - previousMillis > normInt){
        previousMillis = currentMillis;
        moveBall();
      }
    }else if(fast == true){
      if(currentMillis - previousMillis > fastInt){
        previousMillis = currentMillis;
        moveBall();
      }
    }
    //draw the ball
    if(bhp%2 != 0){
      strip.setPixelColor(bhp*15+14-bvp, strip.Color(255, 255, 255));
    }else if (bhp%2 == 0){
      strip.setPixelColor(bhp*15+bvp, strip.Color(255, 255, 255));
    }
    //computer paddle movement
    compMovePaddle();
    //draw's the paddles
    drawPaddles();
    //actualy put everything up.
    strip.show();
    //checking to see if a point has been scored
    if (pIsPoint == true){
      which = 0;
      bhd=0;
      addOne();
      break;
    }else if(cIsPoint == true){
      which = 1;
      bhd=1;
      addOne();
      break;
    }
  }
  if (win == 1){
    clearScreen();
    for (int i = 0 ; youWin[i]>0 ; i++){
      strip.setPixelColor(youWin[i], strip.Color(255, 0, 0));
      strip.show();
    }
    delay(5000);
    for (int i = 0; i < 5; i++){
      playerScore[i] = 0;
      compScore[i] = 0;
    }
    win = 0;
  }else if (win == -1){
    clearScreen();
    for (int i = 0 ; youLose[i]>0 ; i++){
      strip.setPixelColor(youLose[i], strip.Color(0, 255, 0));
      strip.show();
    }
    delay(5000);
    for (int i = 0; i < 5; i++){
      playerScore[i] = 0;
      compScore[i] = 0;
    }
    win = 0;
  }
  pIsPoint = false;
  cIsPoint = false;
  bvp = 8;
  bhp = 6;
  bvd=random(0,2);//0 is down, 1 is up
  fast=false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The AI for the game
void compMovePaddle(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillisForComp > 51){
    previousMillisForComp = currentMillis;
    if (bvp >= 13 && 10-cPaddle <= 10 && cPaddle > 0){
      cPaddle--;
    }else if(bvp <= 0 && 10-cPaddle >= 0 && cPaddle < 10){
      cPaddle++;
    }else if(bvp-3 > 10-cPaddle && 10-cPaddle >= 0 && cPaddle > 0){
      cPaddle--;
    }else if(bvp < 10-cPaddle && 10-cPaddle <= 10){
      cPaddle++;
    }
  }
}
//moves the ball
void moveBall(){
  if (bhd == 0){
    bhp--;
  }else if(bhd == 1){
    bhp++;
  }
  if (bvd == 0){
    bvp--;
  }else if(bvd == 1){
    bvp++;
  }
}

//adds one to the array. If whichOne is 0 it adds to the player's score. If it's 1 it adds to the computers 
void addOne(){
  if (which == 0){//if it's the player's score
    //Add one. This is the simpleist way I know how.
    for (int i=0 ; i < 5 ; i++){//loop
      if (playerScore[i] == 0){//if that value is a zero
        playerScore[i]=1;//make it 1
        break;//then go away
      }else {//if it is 1
        playerScore[i]=0;//make it zero and loop
      }
    }
  }else if(which == 1){
    for (int i=0 ; i < 5 ; i++){
      if (compScore[i] == 0){
        compScore[i]=compScore[i]+1;
        break;
      }else {
        compScore[i]=0;
      }
    }
  }
  //Checks to see if anyone won.
  if (compScore[0]==1 && compScore[2]==1 && compScore[4]==1){
    win = -1;
  }else if (playerScore[0]==1 && playerScore[2]==1 && playerScore[4]==1){
    win = 1;
  }
}

//checks the balls next position
void checkNext(){
  //check's to see if a point has been scored
  if (bhp == 13){
    pIsPoint = true;
  }else if(bhp == 0){
    cIsPoint = true;
  }
  //if a point has been scored than skip over the rest of this stuff
  if (pIsPoint == false || cIsPoint == false){
    //checks the ball verticaly
    if (bvp+bvd==15){
      bvd = 0;
    }else if(bvp+bvd==0){
      bvd = 1;
    }
    //checks the ball horizontaly
    if (bhp+bhd==13){//first on the computer side
      if (bvp >= 10-cPaddle && bvp <= 10-cPaddle+4){
        if(bhd==1){
          bhd=0;
        }else if(bhd==0){
          bhd=1;
        }
        fast = false;
      }else if(bvp+bvd == 10-cPaddle || bvp+bvd == 10-cPaddle+5){
        if(bvp != 0 && bvp !=14){
          if (bvd == 0){
            bvd=1;
          }else if(bvd == 1){
            bvd=0;
          }
          bhd=1;
        }else if (bvp == 0 || bvp == 14){
          bhd=1;
        }
        fast = false;
      }
    }else if(bhp+bhd==1){//then on the player side
      if (bvp >= pPaddle && bvp <= pPaddle+4){//is it hitting the paddle in a normal spot?
        //bounce the ball!
        if(bhd==1){
          bhd=0;
        }else if(bhd==0){
          bhd=1;
        }
      }else if(bvp+bvd == pPaddle || bvp+bvd-5 == pPaddle){//is it hitting the paddle on the corner
        if(bvp != 0 && bvp !=14){//is it not up high or down low?
          //Bounce the ball!
          if (bvd == 0){
            bvd=1;
          }else if(bvd == 1){
            bvd=0;
          }
          bhd=1;
          fast = true;
        }else if (bvp == 0 || bvp == 14){//is it up high or down low?
          //bounce the ball!
          bhd=0;
          fast = true;//set the value to later make the ball move fast
        }
      }
    }
  }
}

int checkPaddlePos(){
  //reads the pot and decides on the paddle position acordingly
  if (analogRead(A15)<93){
    return 0;
  }else if(analogRead(A15)<186){
    return 1;
  }else if(analogRead(A15)<279){
    return 2;
  }else if(analogRead(A15)<372){
    return 3;
  }else if(analogRead(A15)<465){
    return 4;
  }else if(analogRead(A15)<558){
    return 5;
  }else if(analogRead(A15)<652){
    return 6;
  }else if(analogRead(A15)<745){
    return 7;
  }else if(analogRead(A15)<837){
    return 8;
  }else if(analogRead(A15)<=931){
    return 9;
  }else if(analogRead(A15)<=2014){
    return 10;
  }
}

void drawPaddles(){
  //draw's the paddles in the correct spot and displays the score on the paddle
  for(int i = 0 ; i < 5 ; i++){
    if (playerScore[i]==1){
      strip.setPixelColor(pPaddle+i, strip.Color(255, 255, 255));
    }else if((playerScore[i]==0)){
      strip.setPixelColor(pPaddle+i, strip.Color(255, 0, 0));
    }
    if (compScore[i]==1){
      strip.setPixelColor(199+cPaddle-i, strip.Color(255, 255, 255));
    }else if((compScore[i]==0)){
      strip.setPixelColor(199+cPaddle-i, strip.Color(0, 255, 0));
    }
  }
}

void clearScreen(){
  for(int i=0; i<=210 ; i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}
