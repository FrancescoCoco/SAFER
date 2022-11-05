/*
******************************************
      ____   ______  _____  _____ _______
    / ___/ /  __   // ____/ / ___//  __   )
    (___ )/  /_/  // /__   / /__ /  /__/  /
   _____//__/ /__//  __/  / ___//  __   _/
                 / /     / /__ /  /  \  \
                /_/     /____//__/    \__\

  AUTHORS: FLAVIO VINCENZO CONDEMI &
           FRANCESCO MARIA COCO

  ELEMENTS:

    - LCD SCREEN
    - POTENTIOMETER
    - IR REMOTE CONTROLLER
    - IR RECEIVER
    - SERVO MOTOR
    - PUSH BUTTON
    - ELEGOO BOARD


******************************************
*/

/*
   LIBRARIES
*/

//IR SENSOR
#include "IRremote.h"
//LCD SCREEN
#include <LiquidCrystal.h>
//SERVO MOTOR
#include <Servo.h>
#include <EEPROM.h>

/*
   COSTANTS
*/

#define PIN_RESETBTN 5
#define PIN_SERVO 6
#define PIN_IR 13
#define PIN_LED 11
#define SIZE_PSW 10

/*
   VARIABLES
*/

char pincode;
int startpref = 0; //start password configuration of the safe
int attemptcount; //
char lcdpsw[SIZE_PSW]; // aux array where are stored values selected by the IR Remote Controller
int grade = 1;
int countcl; //column count of lcd screen
bool pswIsCorrect;
bool isPswReset;
int resetBtnSt;
int pswsize; //it indicates the size of the password

/*
   OBJECTS
*/

Servo servo;
IRrecv irrecv(PIN_IR);
decode_results results;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

/*
   * *************************************
     asciiDecode() is a function that
     take as parameters byte values sent
     by IR Remote Controller and it
     returns them as Integer values
   * *************************************
*/

int arraylength(char arr[]) {

}

char asciiDecode(byte value)
{
  switch (value)
  {
    case 48:
      return '0';
    case 49:
      return '1';
    case 50:
      return '2';
    case 51:
      return '3';
    case 52:
      return '4';
    case 53:
      return '5';
    case 54:
      return '6';
    case 55:
      return '7';
    case 56:
      return '8';
    case 57:
      return '9';
    case 58:
      return 'R';
    default:
      return '0';
  }
}

/*
   * *************************************
     translateIR() takes action based on
     IR code received, describing Remote
     IR codes
   * *************************************
*/
char translateIR()
{
  switch (results.value)
  {
    case 0xFFA857:
      return 'V'; //vol-
    case 0xFF9867:
      return 'E'; //eq
    case 0xFF6897:
      return '0';
    case 0xFF30CF:
      return '1';
    case 0xFF18E7:
      return '2';
    case 0xFF7A85:
      return '3';
    case 0xFF10EF:
      return '4';
    case 0xFF38C7:
      return '5';
    case 0xFF5AA5:
      return '6';
    case 0xFF42BD:
      return '7';
    case 0xFF4AB5:
      return '8';
    case 0xFF52AD:
      return '9';
    case 0xFFFFFFFF:
      return 'R';

    default:
      return 'R';
  } // End Case

} //END translateIR

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_RESETBTN, INPUT);
  resetBtnSt = 0;
  isPswReset = false;

  startpref = EEPROM.read(0); //Setting a new password for the safe
  pswsize = EEPROM.read(1); //assigning the right password length
  attemptcount = EEPROM.read(2); //assigning 3 attempts at the beginning

  Serial.println("Welcome");
  lcd.begin(16, 2);
  lcd.print("INSERT PIN: ");

  /*
   * *************************************
     - startpref = 0 -> the user have to
     set the new password

     - startpref = 1 -> the password is just
     setted and the user have to insert
     only the pin to access on the safe
   * *************************************
  */

  if (startpref == 1)
  {
    Serial.println("Welcome");
    //show welcome at the beginning
    lcd.begin(16, 2);
    lcd.print("INSERT PIN: ");
  }
  else if (startpref == 255 || startpref == 0)
  {
    Serial.println("Welcome");
    lcd.begin(16, 2);
    lcd.print("WELCOME TO SAFE");
    delay(2000);
    lcd.begin(16, 2);
    lcd.print("NEW PASSWORD");
  }

  irrecv.enableIRIn(); //Enabling IR receiver
}

void loop()
{
  /*
   * *************************************
     THIS CONDITION RESET THE PASSWORD
     WHEN THE RESET BUTTON IS PRESSED!
   * *************************************
  */
  if (digitalRead(PIN_RESETBTN) == HIGH) {
    if (resetBtnSt == 0) {
      Serial.println("Passowrd resetting..");
      isPswReset = true;
      delay(200);
      resetBtnSt = 1;

      Serial.println("Welcome");
      lcd.begin(16, 2);
      lcd.print("WELCOME TO SAFE");
      delay(2000);
      lcd.begin(16, 2);
      lcd.print("NEW PASSWORD");
    }
  }


  startpref = EEPROM.read(0); // Startpref read the value from the EEPROM

  /*
   * *************************************
     attemptcount variable checks how many
     times the password inserted is
     incorrect.
     A the beginning attemptcount is
     setted to 3. If attemptcount is less
     of 0 the safe have to block it and it
     will start the time.
     The user could insert again the right
     password after 30 seconds
   * *************************************
  */

  if (attemptcount > 0)
  {

    /*
    * *************************************
      - startpref = 1 -> the password is just
      setted and the user have to insert
      only the pin to access on the safe

      - isPswReset == false -> in this case
      th user has not clicked on the reset
      button
    * *************************************
    */

    if (startpref == 1 && !isPswReset)
    {

      /*
      * *************************************
        irrecv.decode() answer the question:
        have you typed on a button of the
        IR Remote Controller?

        Return function: true or false
      * *************************************
      */

      if (irrecv.decode(&results))
      {

        /*
        * *************************************
          pincode takes a char value returned
          by translateIR() function
        * *************************************
        */
        pincode = translateIR();

        /*
        * *************************************
          this condition avoid to print
          'E', 'V', 'R' chars in LCD screen
        * *************************************
        */

        if (pincode != 'R' && pincode != 'V' && pincode != 'E')
        {
          lcd.setCursor(countcl, 1);
          lcd.print(pincode);
          lcdpsw[countcl] = pincode;
          Serial.println(pincode);
          countcl++;
          delay(500);
          irrecv.resume();
        }
        /*
        * *************************************
          If it is clicked 'Vol-' in IR Remote
          Controller you can remove the pin
          inserted
        * *************************************
        */
        if (pincode == 'V')
        {
          countcl--;
          lcd.setCursor(countcl, 1);
          lcd.print(" ");
          Serial.println(pincode);
          delay(500);
          irrecv.resume();
        }

        /*
        * *************************************
          If it is clicked 'EQ' in IR Remote
          Controller, you are confirming to
          check if the password is correct.
          If the password is right the door
          wil be opened by the servo motor.
          After 3 seconds, will be closed the
          door automatically
        * *************************************
        */

        if (pincode == 'E')
        {

          /*
          * *************************************
            this is a veryfing password phase.
            The loop check if the password
            inserted is equal to the right
            password setted at the beginning
          * *************************************
          */

          if (countcl == pswsize) {
            for (int i = 3; i <= countcl + 2; i++)
            {
              /*
              * *************************************
                checking if the password inserted
                is equal to the right password
              * *************************************
              */

              if (lcdpsw[i - 3] != asciiDecode(EEPROM.read(i)))
              {
                pswIsCorrect = false;
              }
              else
              {
                Serial.println(asciiDecode(EEPROM.read(i)));
                pswIsCorrect = true;
              }
              Serial.println(lcdpsw[i - 3]);
            }
          } else {
            pswIsCorrect = false;
          }


          if (attemptcount > 0)
          {
            if (pswIsCorrect == true)
            {

              attemptcount = 3;
              EEPROM.write(2, attemptcount);// -> resetting of the number of attempts
              Serial.println("Password correct");

              //displaying in lcd screen that the password is correct
              lcd.begin(16, 2);
              lcd.print("PSW CORRECT");
              delay(3000);

              /*
              * *************************************
                Servo motor is active if pswIsCorrect.
                in this case the servo motor open
                the door and after 5 second it close
                it
              * *************************************
              */

              if (grade > 90)
              {
                servo.attach(PIN_SERVO);
                for (grade = 180; grade >= 90; grade--)
                {
                  servo.write(grade);
                  delay(20);
                }
                servo.detach();
              }

              delay(5000);

              if (grade < 180)
              {
                servo.attach(PIN_SERVO);
                for (grade = 90; grade <= 180; grade++)
                {
                  servo.write(grade);
                  delay(20);
                }
                servo.detach();
              }

              //cleaning of lcd screen
              for (int i = 16; i >= 0; i--)
              {
                lcd.setCursor(i, 1);
                lcd.print(" ");
              }
              countcl = 0;
              lcd.begin(16, 2);
              lcd.print("INSERT PIN: ");
            }
            else
            {
              attemptcount--; //-> one attempt less
              EEPROM.write(2, attemptcount);
              Serial.println("Password not correct");
              lcd.begin(16, 2);
              lcd.print("PSW NOT CORRECT");
              delay(3000);
              //reset of lcd
              for (int i = 16; i >= 0; i--)
              {
                lcd.setCursor(i, 1);
                lcd.print(" ");
              }
              countcl = 0;
              lcd.begin(16, 2);
              lcd.print("INSERT PIN: ");
            }
          }
          else
          {
            //START COUNTING
            EEPROM.write(1, 0);
            lcd.begin(16, 2);
            lcd.print("SAFE BLOCKED");
            for (int i = 30; i >= 0; i--)
            {
              if (i == 10) {
                lcd.setCursor(1, 1);
                lcd.print(" ");
              }

              lcd.setCursor(2, 1);
              lcd.print(i);
              delay(1000);

            }
            attemptcount = 3;
            EEPROM.write(2, attemptcount);
            lcd.begin(16, 2);
            lcd.print("INSERT PIN: ");
          }

          irrecv.resume();
        }
        irrecv.resume();
      }
    }

    /*
    * *************************************
       you'll find in this condition only
       if you are at the beginning and you
       have to set a new password for the
       safe or you have clicked the reset
       button to change the original
       password
    * *************************************
    */
    else if (isPswReset || startpref == 0)

    {
      if (irrecv.decode(&results))
      {
        pincode = translateIR();
        if (pincode != 'R' && pincode != 'V' && pincode != 'E')
        {
          lcd.setCursor(countcl, 1);
          lcd.print(pincode);
          lcdpsw[countcl] = pincode;
          Serial.println(pincode);
          irrecv.resume();
          countcl++;
        }
        if (pincode == 'V')
        {
          countcl--;
          lcd.setCursor(countcl, 1);
          lcd.print(" ");
          irrecv.resume();
          Serial.println(pincode);
        }
        if (pincode == 'E')
        {
          EEPROM.write(0, 1); // ->  startpref = 1;
          EEPROM.write(1, countcl); // -> storing password size

          for (int i = 0; i <= countcl; i++)
          {
            Serial.println(lcdpsw[i]);
          }

          /*
          * *************************************
            the loop store the password in
            the EEPROM memory
          * *************************************
                     |
                     |
                     v
          */

          for (int i = 3; i <= countcl + 2; i++)
          {
            EEPROM.write(i, lcdpsw[i - 3]);
            Serial.println(EEPROM.read(i));
          }

          //reset of lcd
          for (int i = 16; i >= 0; i--)
          {
            lcd.setCursor(i, 1);
            lcd.print(" ");
          }

          // reset count
          pswsize = countcl;
          countcl = 0;
          Serial.println(EEPROM.read(1));

          lcd.begin(16, 2);
          lcd.print("INSERT PIN: ");
          irrecv.resume();
          isPswReset = false;
          resetBtnSt = 0;
          delay(200);
        }

        irrecv.resume();
      }

    }
  }
  else
  {
    /*
    * *************************************
       if the number of attempts are less
       then 3, the safe block itself and
       the user must wait 30 seconds before
       he can insert tha password again
    * *************************************
    */

    lcd.begin(16, 2);
    lcd.print("SAFE BLOCKED");
    Serial.println("The safe is blocked");
    EEPROM.write(1, 0);
    for (int i = 30; i >= 0; i--)
    {
      if (i == 9) {
        Serial.println("è arrivato a 10");
        lcd.setCursor(1, 1);
        lcd.print(" ");
      }

      lcd.setCursor(0, 1);
      lcd.print(i);
      delay(1000);

    }
    attemptcount = 3;
    EEPROM.write(1, 3);
    lcd.begin(16, 2);
    lcd.print("INSERT PIN: ");
  }
}
