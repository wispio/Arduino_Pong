/*
 - This code creates a remake of the video game "Pong".
 The code creates a title screen, a menu screen, and four
 levels of difficulty of gameplay(Easy, Medium, Hard, Impossible).

 -This code was written for a system based around the
 An Arduino Uno and an Adafruit 1.8" TFT Sheild w/ microSD and Joystick

 -The whole physical system (as originally built) was made from the following materials:
 1 Arduino Uno
 1 Adafruit 1.8" TFT Sheild w/ microSD and Joystick
 1 Speaker (soldered to ground and digital pin 6 in this case)
 28 Male Conn headers (male pin extenders) *(They will fit into the outer pins of the TFT sheild)

 -The code will require these arduino libraries to be downloaded to function or to be edited:
 "Adafruit_ST7735.h" @ https://github.com/adafruit/Adafruit-ST7735-Library
 "Adafruit_GFX.h" @ https://github.com/adafruit/Adafruit-GFX-Library

-Here is a link to a video briefly explaining the code and its algorithms:
 http://youtu.be/zWp7gQaBFWY

 -This code will also need defenitions of notes so as to work as originally intended.
 -Here are the note definitions. Create a new tab in the sketch, and copy the defenitions into it.
 ///////////////////////////////////////
 //Public Constants (Note Definitions)//
 ///////////////////////////////////////
 #define NOTE_B0  31
 #define NOTE_C1  33
 #define NOTE_CS1 35
 #define NOTE_D1  37
 #define NOTE_DS1 39
 #define NOTE_E1  41
 #define NOTE_F1  44
 #define NOTE_FS1 46
 #define NOTE_G1  49
 #define NOTE_GS1 52
 #define NOTE_A1  55
 #define NOTE_AS1 58
 #define NOTE_B1  62
 #define NOTE_C2  65
 #define NOTE_CS2 69
 #define NOTE_D2  73
 #define NOTE_DS2 78
 #define NOTE_E2  82
 #define NOTE_F2  87
 #define NOTE_FS2 93
 #define NOTE_G2  98
 #define NOTE_GS2 104
 #define NOTE_A2  110
 #define NOTE_AS2 117
 #define NOTE_B2  123
 #define NOTE_C3  131
 #define NOTE_CS3 139
 #define NOTE_D3  147
 #define NOTE_DS3 156
 #define NOTE_E3  165
 #define NOTE_F3  175
 #define NOTE_FS3 185
 #define NOTE_G3  196
 #define NOTE_GS3 208
 #define NOTE_A3  220
 #define NOTE_AS3 233
 #define NOTE_B3  247
 #define NOTE_C4  262
 #define NOTE_CS4 277
 #define NOTE_D4  294
 #define NOTE_DS4 311
 #define NOTE_E4  330
 #define NOTE_F4  349
 #define NOTE_FS4 370
 #define NOTE_G4  392
 #define NOTE_GS4 415
 #define NOTE_A4  440
 #define NOTE_AS4 466
 #define NOTE_B4  494
 #define NOTE_C5  523
 #define NOTE_CS5 554
 #define NOTE_D5  587
 #define NOTE_DS5 622
 #define NOTE_E5  659
 #define NOTE_F5  698
 #define NOTE_FS5 740
 #define NOTE_G5  784
 #define NOTE_GS5 831
 #define NOTE_A5  880
 #define NOTE_AS5 932
 #define NOTE_B5  988
 #define NOTE_C6  1047
 #define NOTE_CS6 1109
 #define NOTE_D6  1175
 #define NOTE_DS6 1245
 #define NOTE_E6  1319
 #define NOTE_F6  1397
 #define NOTE_FS6 1480
 #define NOTE_G6  1568
 #define NOTE_GS6 1661
 #define NOTE_A6  1760
 #define NOTE_AS6 1865
 #define NOTE_B6  1976
 #define NOTE_C7  2093
 #define NOTE_CS7 2217
 #define NOTE_D7  2349
 #define NOTE_DS7 2489
 #define NOTE_E7  2637
 #define NOTE_F7  2794
 #define NOTE_FS7 2960
 #define NOTE_G7  3136
 #define NOTE_GS7 3322
 #define NOTE_A7  3520
 #define NOTE_AS7 3729
 #define NOTE_B7  3951
 #define NOTE_C8  4186
 #define NOTE_CS8 4435
 #define NOTE_D8  4699
 #define NOTE_DS8 4978

 Original Code is 26.942 kb, last modified on Jan 9, 2015
 Composed by Alex Nakhleh
 */


#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>
#include "pitches.h"

#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
#define SD_CS    4  // Chip select line for SD card
#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   8  // Data/command line for TFT
#define TFT_RST  -1  // Reset line for TFT (or connect to +5V)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Defenitions For Joystick Inputs.
//These interract with the command function "uint8_t readButton(void)" and "readButton()"
#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5

int NoteDuration=100;
int note=0;

int RocketSong[]={
  NOTE_GS4,NOTE_DS4,NOTE_F4,NOTE_GS4,NOTE_AS4, //Song Played when Player wins
  NOTE_DS4,NOTE_GS4,NOTE_AS4,NOTE_C5,NOTE_DS4,NOTE_GS4,NOTE_C5,
  NOTE_CS5,NOTE_F4,NOTE_GS4,NOTE_CS5,NOTE_AS4,NOTE_CS4,NOTE_F4,
  NOTE_AS4,NOTE_G4,NOTE_CS4,NOTE_F4,NOTE_GS4};

int failsong[]={
  NOTE_B4,NOTE_GS4,NOTE_F4,NOTE_A4,NOTE_FS4,NOTE_DS4,NOTE_E4}; //Song Played when player loses


int www=0; //used later to make sure rng is only used to determine whether ball is thrown left or right at the start of a point
int xxx=0; //stores random number for x-velocity
int ppp=0;//Makes sure The title, two walls, and "press joystick" are only printed once when the game resets from a loss or win
int yyy=0;//used later to make sure rng is used to deternime the angle of the balls starting movement at the start of a point
int zzz=0;//stores random number for y-velocity

int noteval=0;//will be used to begin the songs from their first note

int RocketSongNoteDuration=300;//Duration of notes of the winning song

int STAGE=0;//Level Identifier
int S1=60; //Original title screen ball coordinates and x and y speeds
int S2=105;
int p1=1;
int p2=1;

int menuX=100; //
int menuY=50;
int menuA=0;
int menuB=0;

int playerScore=0;//Player's score
int compScore=0;//AI's score

int pX=157;//Coordinates for the Player paddle and its movement speeds
int pY=50;
int pA=0;
int pB=6;

int cX=0; //Coordinates for the AI paddle and its movement speeds
float cY=50;
int cA;
float cB;

int ballX=78;//Coordinates for the ball and its speed
int ballY=61;
int ballA=3;
float ballB=0;




void setup()
{
  Serial.begin(9600);
  // Initialize 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  Serial.println("OK!");
  //Prints the Title, the two walls, and "press joystick" on the title screen
  tft.setTextSize(5);//Prints "PONG"
  tft.setCursor(6,30);
  tft.print("PONG");

  tft.setTextSize(0);//Prints "Press Joystick"
  tft.setCursor(20,140);
  tft.print("Press Joystick");

  tft.drawLine(20,90,20,120,ST7735_WHITE); //Prints the 2 walls in the title screen
  tft.drawLine(21,90,21,120,ST7735_WHITE);
  tft.drawLine(100,90,100,120,ST7735_WHITE);
  tft.drawLine(99,90,99,120,ST7735_WHITE);

  randomSeed(analogRead(0));//Set up for future uses of "RNG"
}

//READ JOYSTICK FUNCTION
////////////////////////////////////////////////////////////////////////////////////////
uint8_t readButton(void) {
  float a = analogRead(3);

  a *= 5.0;
  a /= 1024.0;

  //Serial.print("Button read analog = ");
  ///Serial.println(a);
  if (a < 0.2) return BUTTON_DOWN;
  if (a < 1.0) return BUTTON_RIGHT;
  if (a < 1.5) return BUTTON_SELECT;
  if (a < 2.0) return BUTTON_UP;
  if (a < 3.45) return BUTTON_LEFT;// a<3.2 if joystick will read left with no input, use a<3.45 if arduino is not sensing the joystick being moved left
  else return BUTTON_NONE;
}
uint8_t buttonhistory = 0;


void loop()
{
  while(STAGE==0)//Level 0 = Title Screen
  {
    uint8_t b=readButton();//Reads button
    Serial.println(b);
    if(ppp==0)
    {
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(5);
      tft.setCursor(6,30);
      tft.print("PONG");

      tft.setTextSize(0);
      tft.setCursor(20,140);
      tft.print("Press Joystick");

      tft.drawLine(20,90,20,120,ST7735_WHITE);
      tft.drawLine(21,90,21,120,ST7735_WHITE);
      tft.drawLine(100,90,100,120,ST7735_WHITE);
      tft.drawLine(99,90,99,120,ST7735_WHITE);
      ppp++; //makes that "Pong", "press joystick" and the 2 walls are only printed once the game resets
      delay(1000);//to give the user a time to release the joystick when the game resets after a win/loss.
    }

    tft.fillRect(S1,S2,4,4,ST7735_WHITE);//Code to print and move the ball on the title screen
    delay(20);
    tft.fillRect(S1,S2,4,4,ST7735_BLACK);
    S1=S1+p1;
    S2=S2+p2;
  
    if(S1+4>=99 || S1<=22)//Wall Physics for title screen
    {
      p1=-p1;
    }
    if(S2<=90 || S2+4>=120)//Wall Physics for title screen
    {
      p2=-p2;
    }
    if(b==BUTTON_SELECT)//Press joystick to add 1 to level identifier
    {
      STAGE=STAGE+1;
    }
  }

  if(STAGE==1)//Clear screen when Level Identifier==1 (menu screen)
  {
    Serial.println("STAGE_1 INITIATED");
    tft.fillScreen(ST7735_BLACK);
    delay(250);
  }
  while(STAGE==1)
  {
    uint8_t b=readButton();//read joystick
  
    //Print the names of the level difficulties
    tft.setTextSize(2);
    tft.setTextColor(ST7735_MAGENTA);
    tft.setCursor(5,10);
    tft.print("IMPOSSIBLE");
    tft.setTextSize(3);
    tft.setTextColor(ST7735_RED);
    tft.setCursor(5, 43);
    tft.print("HARD");
    tft.setTextColor(ST7735_YELLOW);
    tft.setCursor(5, 88);
    tft.setTextSize(3);
    tft.print("MEDIUM");
    tft.setTextColor(ST7735_GREEN);
    tft.setTextSize(3);
    tft.setCursor(5, 130);
    tft.print("EASY");

    tft.fillRect(menuX,menuY,10,10,ST7735_BLACK);//Printing and movement of the selecting cursor
    menuX=menuX+menuA;//movement
    menuY=menuY+menuB;
    delay(10);
    tft.fillRect(menuX,menuY,10,10,ST7735_WHITE);
  
    if(b==BUTTON_UP)//Movement fo the Cursor based on joystick readings
    {
      menuA=0;
      menuB=-6;
    }
    if(b==BUTTON_DOWN)
    {
      menuA=0;
      menuB=6;
    }
    if(b==BUTTON_RIGHT)
    {
      menuA=6;
      menuB=0;
    }
    if(b==BUTTON_LEFT)
    {
      menuA=-6;
      menuB=0;
    }
    if(b==BUTTON_NONE)
    {
      menuA=0;
      menuB=0;
    }
//MENU SELECTIONS
/////////////////////////////////////////////////////////////////////////////////////
    if(menuY>=0 && menuY<=35)
    {
      tft.drawRect(0,0,128,35,ST7735_WHITE);//Draw a rectangle around the box and cover adjacent selection rectangles
      tft.drawRect(0,36,128,39,ST7735_BLACK);

      if(b==BUTTON_SELECT)
      {
        tft.fillScreen(ST7735_BLACK);
        delay(100);
        tft.setTextSize(2);
        tft.setTextColor(ST7735_MAGENTA);
        tft.setCursor(2,50);
        tft.print("IMPOSSIBLE");
        tft.setTextSize(0);
        tft.setTextColor(ST7735_WHITE);
        tft.setCursor(5,100);
        tft.print("Turn the device Sideways");
        delay(500);
        tft.fillScreen(ST7735_BLACK);
        tft.setRotation(tft.getRotation()+1);
        STAGE=STAGE+1;//Increase level identifier to "impossible" difficulty
      }
    }


    if(menuY>=36 && menuY<=75)
    {
      tft.drawRect(0,0,128,35,ST7735_BLACK);//Draw a rectangle around the box and cover adjacent selection rectangles
      tft.drawRect(0,76,128,43,ST7735_BLACK);
      tft.drawRect(0,36,128,39,ST7735_WHITE);

      if(b==BUTTON_SELECT)
      {
        tft.fillScreen(ST7735_BLACK);//Selection Restatement
        delay(100);
        tft.setTextSize(4);
        tft.setTextColor(ST7735_RED);
        tft.setCursor(15,50);
        tft.print("HARD");
        tft.setTextSize(0);
        tft.setTextColor(ST7735_WHITE);
        tft.setCursor(5,100);
        tft.print("Turn the device sideways.");
        delay(500);
        STAGE=STAGE+2;//Increase level identifier to "hard" difficulty
        tft.fillScreen(ST7735_BLACK);
        tft.setRotation(tft.getRotation()+1);//Alter Orientation to Landscape
      
      }

    }
    if(menuY>=76 && menuY<=116)
    {
      tft.drawRect(0,118,128,40,ST7735_BLACK);//Draw a rectangle around the box and cover adjacent selection rectangles
      tft.drawRect(0,36,128,39,ST7735_BLACK);
      tft.drawRect(0,76,128,43,ST7735_WHITE);

      if(b==BUTTON_SELECT)
      {
        tft.fillScreen(ST7735_BLACK);//Selection Restatement
        delay(100);
        tft.setTextSize(3);
        tft.setTextColor(ST7735_YELLOW);
        tft.setCursor(10,50);
        tft.print("MEDIUM");
        tft.setTextSize(0);
        tft.setTextColor(ST7735_WHITE);
        tft.setCursor(5,100);
        tft.print("Turn the device sideways");
        delay(5000);
        tft.fillScreen(ST7735_BLACK);
        tft.setRotation(tft.getRotation()+1);//Alter orientation to landscape
        STAGE=STAGE+3;//Increase level identifier to "medium" difficulty
      }
    }
    if(menuY>=118 && menuY<=160)
    {
      tft.drawRect(0,76,128,43,ST7735_BLACK);//Draw a rectangle around the box and cover adjacent selection rectangles
      tft.drawRect(0,118,128,40,ST7735_WHITE);

      if(b==BUTTON_SELECT)
      {
        tft.fillScreen(ST7735_BLACK);//Selection Restatement
        delay(100);
        tft.setTextSize(3);
        tft.setTextColor(ST7735_GREEN);
        tft.setCursor(29,50);
        tft.print("EASY");
        tft.setTextSize(0);
        tft.setTextColor(ST7735_WHITE);
        tft.setCursor(5,100);
        tft.print("Turn the device sideways.");
        delay(1500);
        tft.fillScreen(ST7735_BLACK);
        tft.setRotation(tft.getRotation()+1);//Alter screen orientation to landscape, where the joystick is on the right side of the board
        STAGE=STAGE+4;//Increase level identifier to "easy" difficulty
      }
    }
  }//Menu(Stage1) ends
  //DRAW PONG MAP, COLORS BASED ON DIFFICULTY SELECTED
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  if(STAGE==2)
  {
    tft.fillRect(0,0,tft.width(),2,ST7735_MAGENTA);
    tft.fillRect(0,126,tft.width(),2,ST7735_MAGENTA);
    tft.drawLine(tft.width()/2,2,tft.width()/2,126,ST7735_WHITE);
  }
  if(STAGE==3)
  {
    tft.fillRect(0,0,tft.width(),2,ST7735_RED);
    tft.fillRect(0,126,tft.width(),2,ST7735_RED);
    tft.drawLine(tft.width()/2,2,tft.width()/2,126,ST7735_WHITE);
  }
  if(STAGE==4)
  {
    tft.fillRect(0,0,tft.width(),2,ST7735_YELLOW);
    tft.fillRect(0,126,tft.width(),2,ST7735_YELLOW);
    tft.drawLine(tft.width()/2,2,tft.width()/2,126,ST7735_WHITE);
  }
  if(STAGE==5)
  {
    tft.fillRect(0,0,tft.width(),2,ST7735_GREEN);
    tft.fillRect(0,126,tft.width(),2,ST7735_GREEN);
    tft.drawLine(tft.width()/2,2,tft.width()/2,126,ST7735_WHITE);
  }

  while(STAGE>=2)
  {
    Serial.println("AAA");
    uint8_t b=readButton();//Reads Joystick
    //AI
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if(STAGE==2)//For Impossible
    {
      cY=ballY-12;
    }
    if(STAGE==3)//For Hard
    {
      float cB=5;
      if(ballY==cY+12)
      {
        cY=cY;//Paddle movement
      }
      if(ballY<cY+12)
      {
        cY=cY-cB;//Paddle movement
      }
      if(ballY>cY+12)
      {
        cY=cY+cB;//Paddle movement
      }

    }
    if(STAGE==4)//For Medium
    {
      float cB=4.4;
      if(ballY==cY+12)//Initiation of motion
      {
        cY=cY;
      }
      if(ballY<=cY+8)//Initiation of motion
      {
        cB=-cB;

        cY=cY+cB;//Paddle movement
        if(ballX<=cX+3)
        {
        
          cB=cB*1.4;
        }

      }
      if(ballY>=cY+16)//Initiation of motion
      {
        cB=cB;
        cY=cY+cB;//Paddle movement
        if(ballX<=cX+3)
        {
          cB=cB*1.4;
        }
    
      }
    
    }
    if(STAGE==5)//For Easy
    {
      float cB=3.01;
      if(ballY==cY+12)//Initiation of motion
      {
        cY=cY;
      }
      if(ballY<=cY+10)//Initiation of motion
      {
        cB=-cB;
        cY=cY+cB;//Paddle movement
        if(ballX<=cX+3)
        {

          cB=cB*1.43;
        }
      }
      if(ballY>=cY+14)//Initiation of motion
      {
        cB=cB;
        cY=cY+cB;
        if(ballX<=cX+3)
        {

          cB=cB*1.43;
        }
      }
    }

//SET SCOREBOARD COLORS AND SCORE
//////////////////////////////////////////////////////////////////////////
    tft.setTextSize(2);
    if(STAGE==2)
    {
      tft.setTextColor(ST7735_MAGENTA);
    }
    if(STAGE==3)
    {
      tft.setTextColor(ST7735_RED);
    }
    if(STAGE==4)
    {
      tft.setTextColor(ST7735_YELLOW);
    }
    if(STAGE==5)
    {
      tft.setTextColor(ST7735_GREEN);
    }
    tft.setCursor(50,5);//position of ai score
    tft.print(compScore);
    tft.setCursor(90,5);//position of player score
    tft.print(playerScore);
  
  
  
    //PLAYER PADDLE MOVEMENT
    ////////////////////////////////////////////////////////////////////////////////////////////
    if(b==BUTTON_LEFT)//Joystick Down
    {
      pY=pY+pB;
    }
    if(b==BUTTON_RIGHT)//Joystick Up
    {
      pY=pY-pB;
    }
  
    //PLAYER PADDLE PHYSICS & BALL START RNG'S
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(www==0)//BALL START X-Velocity RNG
    {
      xxx=random(2);//
      //If xxx=0, the ball moves right at start, if xxx=1, then it moves left
      www++;//Makes sure this is done only once per point
      if(xxx==1)
      {
        ballA=-ballA;//reverses velocity of ball
      }
    }
    if(yyy==0)
    {
      zzz=random(5);
      yyy++;
      if(zzz==0)
      {
        ballB=-1;
      }
      if(zzz==1)
      {
        ballB=-0.5;
      }
      if(zzz==2)
      {
        ballB=0;
      }
      if(zzz==3)
      {
        ballB=0.5;
      }
      if(zzz==4)
      {
        ballB=1;
      }
    }
    //DRAW PLAYER AND AI PADDLES
    ////////////////////////////////////////////////////////////////////////////////////////////
    tft.fillRect(cX,cY,3,24,ST7735_WHITE);
    tft.fillRect(pX,pY,3,24,ST7735_WHITE);
    tft.fillRect(ballX,ballY,4,4,ST7735_WHITE);
    delay(30);
    tft.fillRect(cX,cY,3,24,ST7735_BLACK);
    tft.fillRect(pX,pY,3,24,ST7735_BLACK);
    tft.fillRect(ballX,ballY,4,4,ST7735_BLACK);
    tft.drawLine(tft.width()/2,2,tft.width()/2,126,ST7735_WHITE);//middle-court line
    
//PLAYER PADDLE PHYSICS & BALL SPEED INCREASE
//////////////////////////////////////////////////////////////////////////////////////////////////
    if(ballX+4>=pX && ballY+4<=pY+4 && ballY>=pY-3)
    {
      ballX=pX-5;//realign ball
      ballB=ballB-5;
      ballA=ballA*1.36;//ball speed increase
      ballA=-ballA;//reverse ball velocity
      tone(6,NOTE_B4,NoteDuration);
    }
    if(ballX+4>=pX && ballY>=pY+1 && ballY+4<=pY+14)
    {
      ballX=pX-5;//realign ball
      ballB=ballB-2;
      ballA=ballA*1.36;//ball speed increase
      ballA=-ballA;//reverse ball velocity
      tone(6,NOTE_B4,NoteDuration);

    }
    if(ballX+4>=pX && ballY>=pY+10 && ballY+4<=pY+18)
    {
      ballX=pX-5;//realign ball
      ballA=ballA*1.36;//ball speed increase
       ballA=-ballA;//reverse ball velocity
      tone(6,NOTE_B4,NoteDuration);
    }
    if(ballX+4>=pX && ballY+4<=pY+23 && ballY>=pY+14)
    {
      ballX=pX-5;//realign ball
      ballB=ballB+2;
      ballA=ballA*1.36;//ball speed increase
      ballA=-ballA;//reverse ball velocity
      tone(6,NOTE_B4,NoteDuration);
    }
    if(ballX+4>=pX && ballY>=pY+20 && ballY+4<=pY+27)
    {
      ballX=pX-5;//realign ball
      ballB=ballB+4;
      ballA=ballA*1.36;//ball speed increase
      ballA=-ballA;
      tone(6,NOTE_B4,NoteDuration);
    }
  
    //COMPUTER PADDLE PHYSICS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(STAGE==2)//FOR IMPOSSIBLE
    {
      if(ballX<=cX+3)
      {
        ballA=-ballA;
        ballA=ballA*1.36;
        tone(6,NOTE_B4,NoteDuration);
      }
      //BALL SPEED CAP FOR IMPOSSIBLE
      if(ballA>=16.176||ballA<=-16.176)
      {
        if(ballA>0)
        {
          ballA=22;
        }
        if(ballA<0)
        {
          ballA=-22;
        }
        if(ballX<=cX+3 && ballX>=cX-20)
        {
          ballX=cX+3;
        }
      }
    }
    if(STAGE>=3)//FOR ALL OTHER LEVELS
    {
      if(ballX<=cX+3 && ballY+4<=cY+4 && ballY>=cY-3)
      {

        ballX=cX+4;
        ballB=ballB-4;
        ballA=ballA*1.36;
        ballA=-ballA;
        tone(6,NOTE_B4,NoteDuration);
      }
      if(ballX<=cX+3 && ballY>=cY+1 && ballY+4<=cY+14)
      {

        ballX=cX+4;
        ballB=ballB-2;
        ballA=ballA*1.36;
        ballA=-ballA;
        tone(6,NOTE_B4,NoteDuration);
      }
      if(ballX<=cX+3 && ballY>=cY+10 && ballY+4<=cY+18)
      {

        ballX=cX+4;
        ballA=ballA*1.36;
        ballA=-ballA;
        tone(6,NOTE_B4,NoteDuration);
      }
      if(ballX<=cX+3 && ballY+4<=cY+23 && ballY>=cY+14)
      {
        ballX=cX+4;
        ballB=ballB+2;
        ballA=ballA*1.36;
        ballA=-ballA;
        tone(6,NOTE_B4,NoteDuration);
      }
      if(ballX<=cX+3 && ballY>=cY+20 && ballY+4<=cY+27)
      {

        ballX=cX+4;
        ballB=ballB+4;
        ballA=ballA*1.36;
        ballA=-ballA;
        tone(6,NOTE_B4,NoteDuration);
      }
      Serial.println("BallA=");
      Serial.println(ballA);
    //BALL TO WALL REPAIR CODE
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(ballY<=4)
    {
      ballB=-ballB;
      tone(6,NOTE_B2,NoteDuration);
      if(STAGE==2)
      {
        tft.fillRect(0,0,tft.width(),2,ST7735_MAGENTA);
      }
      if(STAGE==3)
      {

        tft.fillRect(0,0,tft.width(),2,ST7735_RED);
      }
      if(STAGE==4)
      {

        tft.fillRect(0,0,tft.width(),2,ST7735_YELLOW);
      }
      if(STAGE==5)
      {

        tft.fillRect(0,0,tft.width(),2,ST7735_GREEN);
      }
    }
    if(ballY+4>=124)
    {
      ballB=-ballB;
      tone(6,NOTE_B2,NoteDuration);//Noise denotes that a wall has been hit
      if(STAGE==2)
      {
        tft.fillRect(0,126,tft.width(),2,ST7735_MAGENTA);
      }
      if(STAGE==3)
      {

        tft.fillRect(0,126,tft.width(),2,ST7735_RED);
      }
      if(STAGE==4)
      {

        tft.fillRect(0,126,tft.width(),2,ST7735_YELLOW);
      }
      if(STAGE==5)
      {

        tft.fillRect(0,126,tft.width(),2,ST7735_GREEN);
      }
    }
    //Paddle Interraction with wall
    /////////////////////////////////////////////////////////////////////////////////////////////
    if(pY<=0)
    {
      pY=pY+6;
    }
    if(pY+24>=128)
    {
      pY=pY-pB;
    }
    if(cY<=0)
    {
      cY=cY+4;
    }
    if(cY+24>=128)
    {
      cY=cY-6;
    }
    //MAX BALL SPEEDS FOR ALL LEVELS OTHER THAN IMPOSSIBLE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      if(STAGE==3)
      {
        if(ballA>=12.5||ballA<=-12.5)
        {
          if(ballA>0)
          {
            ballA=16;
          }
          if(ballA<0)
          {
            ballA=-16;
          }
        }
      }
      if(STAGE==4)
      {
        if(ballA>=8.088||ballA<=-8.088)
        {


          if(ballA>0)
          {
            ballA=11;
          }
          if(ballA<0)
          {
            ballA=-11;
          }
        }
      }
      if(STAGE==5)
      {
        if(ballA>=5.147||ballA<=-5.147)
        {


          if(ballA>0)
          {
            ballA=7;
          }
          if(ballA<0)
          {
            ballA=-7;
          }
        }
      }
    }


    //WIN AND LOSS OF POINT
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(ballX<=-1)//IF POINT iS WON
    {
      www=0;//So that rng can be used in the next point to determine ball's starting velocity
      yyy=0;
      tft.setTextSize(2);//BLACK OUT SCORE FOR REPRINT
      tft.setTextColor(ST7735_BLACK);
      tft.setCursor(90,5);
      tft.print(playerScore);
      playerScore=playerScore+1;//ADD 1 TO SCORE
    
      tft.fillRect(pX,pY,3,24,ST7735_WHITE);
      tft.fillRect(cX,cY,3,24,ST7735_WHITE);
      ballX=0;
      tft.fillRect(ballX,ballY,4,4,ST7735_CYAN);
      delay(250);
      tft.fillRect(ballX,ballY,4,4,ST7735_BLACK);
      delay(250);
      tft.fillRect(ballX,ballY,4,4,ST7735_CYAN);
      delay(250);
      tft.fillRect(ballX,ballY,4,4,ST7735_BLACK);
      delay(250);
      tft.fillRect(ballX,ballY,4,4,ST7735_CYAN);
      delay(500);
      tft.fillRect(ballX,ballY,4,4,ST7735_BLACK);
      tft.fillRect(pX,pY,3,24,ST7735_BLACK);
      tft.fillRect(cX,cY,3,24,ST7735_BLACK);
      delay(700);

      ballB=0;//RESET PADDLE AND BALL POSITIONS
      ballX=78;
      ballY=61;
      ballA=3;
      pY=50;
      cY=50;
      delay(2000);


    }
    if(ballX+4>=tft.width()+1)//IF POINT IS LOST
    {
      www=0;//So that rng can be used in the next point to determine ball's starting velocity
      yyy=0;
      tft.setTextSize(2);
      tft.setTextColor(ST7735_BLACK);
      tft.setCursor(50,5);
      tft.print(compScore);
      compScore=compScore+1;
    
      tft.fillRect(pX,pY,3,24,ST7735_WHITE);
      tft.fillRect(cX,cY,3,24,ST7735_WHITE);
      tft.fillRect(ballX-3,ballY,4,4,ST7735_CYAN);
      delay(250);
      tft.fillRect(ballX-3,ballY,4,4,ST7735_BLACK);
      delay(250);
      tft.fillRect(ballX-3,ballY,4,4,ST7735_CYAN);
      delay(250);
      tft.fillRect(ballX-3,ballY,4,4,ST7735_BLACK);
      delay(250);
      tft.fillRect(ballX-3,ballY,4,4,ST7735_CYAN);
      delay(500);
      tft.fillRect(ballX-3,ballY,4,4,ST7735_BLACK);
      tft.fillRect(pX,pY,3,24,ST7735_BLACK);
      tft.fillRect(cX,cY,3,24,ST7735_BLACK);
      delay(400);
    
      ballB=0;
      ballX=78;
      ballY=61;
      ballA=3;
      pY=50;
      cY=50;
      //delay(2000);
    }
    //BALL TO WALL REPOSITION
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(ballY<=4)
    {
      ballY=6;
    }
    if(ballY>=124)
    {
      ballY=122;
    }
    /////////////////////////////ACTUAL MOVEMENT
    ballX=ballX+ballA;
    ballY=ballY+ballB;
    Serial.println("BALLA=");
    Serial.print(ballA);
    //WIN AND LOSS TITLE
    ///////////////////////////////////////////////////////////////////////////WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW///////////////////////
    while(playerScore==7)
    {
      int qqq=1;
      tft.fillScreen(ST7735_BLACK);
      tft.setTextSize(4);
      while(qqq<=2) 
      {
        if(STAGE==2)
        {
          tft.setTextColor(ST7735_MAGENTA);
        }
        if(STAGE==3)
        {
          tft.setTextColor(ST7735_RED);
        }
        if(STAGE==4)0
        {
          tft.setTextColor(ST7735_YELLOW);
        }
        if(STAGE==5)
        {
          tft.setTextColor(ST7735_GREEN);
        }
        tft.setCursor(10,40);
        tft.print("PLAYER WINS!");

        if(note<24)//Plays the song loop for win
        {
          tone(6,RocketSong[note],240);
          delay(246);
          note++;
        }
        if(note>=24)
        {
          note=0;
        }
        uint8_t b=readButton();//Read joystick
        if(b==BUTTON_SELECT)//Return to title screen
        {
          tft.fillScreen(ST7735_BLACK);
          qqq=3;
          playerScore=0;
          ppp=0;
          tft.setRotation(tft.getRotation()-1);
          STAGE=0;
        }
      }
    }
//If player loses
    while(compScore==7)
    {
      int qqq=1;
      tft.fillScreen(ST7735_BLACK);
      tft.setTextSize(4);
      while(qqq<=2)
      {
        if(STAGE==2)
        {
          tft.setTextColor(ST7735_MAGENTA);
        }
        if(STAGE==3)
        {
          tft.setTextColor(ST7735_RED);
        }
        if(STAGE==4)
        {
          tft.setTextColor(ST7735_YELLOW);
        }
        if(STAGE==5)
        {
          tft.setTextColor(ST7735_GREEN);
        }
        tft.setCursor(10,40);
        tft.print("PLAYER LOSES");
        delay(300);
        //Losing song is played once
        while (noteval<=6)
        {
          tone(6,failsong[noteval],240);
          delay(260);
          noteval++;
        }
        uint8_t b=readButton();//Read joystick
        if(b==BUTTON_SELECT)//Return to title screen
        {
          tft.fillScreen(ST7735_BLACK);
          qqq=3;
          compScore=0;
          ppp=0;
          tft.setRotation(tft.getRotation()-1);
          STAGE=0;
        }
      }
    }
  }
}
