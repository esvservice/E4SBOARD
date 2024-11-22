
// ************************************************************
// ***** testprogramma voor E-4S Board ESP32 trainerboard *****
// ***** versie 1.0                                       *****
// ************************************************************

// ---- include library & files --------------------------------
#include "E4SBoard.h"
#include "LiquidCrystal_I2C.h"
#include "Temperature_LM75_Derived.h"
// --------------------------------------------------------------

// ---- initialise components -----------------------------------
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);   // base adress=0x27 of 3F
Generic_LM75 temperature(0x48);                    // base adress=0x48
E4SClass E4SBoard;
// -------------------------------------------------

// ---- def & init variables -----------------------
#define MuxA0 15        // MUX inputselsectie A0
#define MuxA1 0         // MUX inputselsectie A1
#define MuxA2 5         // MUX inputselsectie A2
#define MuxA3 14        // MUX inputselsectie A3
#define MuxDin 39       // MUX data in

#define ADC1 34         // AD-converter 1 input
#define ADC2 35         // AD-converter 2 input
#define ADC3 36         // AD-converter 3 input
#define DAC 26          // DA-converter output

#define ShiftRegStrb 13 // shift register STROBE
#define ShiftRegData 4  // shift register DATA
#define ShiftRegClk 23  // shift register CLOCK

#define OnBoardLed 2    // White LED & blue on-board-LED

const byte DDo[4] = { 16, 17, 18, 19 };
const byte DDi[4] = { 32, 33, 25, 27 };
bool Serout[24];
bool DDin[4];
bool MXi[16];
float ADC1Val, ADC2Val, ADC3Val;

#define Beep 12         // Beeper

void setup()
{
    // *****************************************************************
    // ************************ SETUP **********************************
    // *****************************************************************
    E4SBoard.Begin();
    lcd.begin();
    lcd.backlight();
    lcd.print("ON4IPR - E4SBOARD - SAMPLE 2");


    // *********************************************
    // init serial port
    // *********************************************
    Serial.begin(9600);

    // *********************************************
    // init on board LED
    // *********************************************
    pinMode(OnBoardLed, OUTPUT);
    digitalWrite(OnBoardLed, LOW);

    // *********************************************
    // switch direct digital outputs OFF
    // *********************************************
    for (int i = 0; i <= 3; i++) // set outputs
    {
        pinMode(DDo[i], OUTPUT);
        digitalWrite(DDo[i], LOW);
    }
    // *********************************************
    // initialise direct digital inputs
    for (int i = 0; i <= 3; i++) // set outputs
    {
        pinMode(DDi[i], INPUT);
        DDin[i] = 0;
    }
    // *********************************************
    // init MUX inputs
    // *********************************************
    pinMode(MuxA0, OUTPUT);
    pinMode(MuxA1, OUTPUT);
    pinMode(MuxA2, OUTPUT);
    pinMode(MuxA3, OUTPUT);
    pinMode(MuxDin, INPUT);

    // *********************************************
    // init shift reg outputs
    // *********************************************
    pinMode(ShiftRegData, OUTPUT);
    pinMode(ShiftRegClk, OUTPUT);
    pinMode(ShiftRegStrb, OUTPUT);
    for (int k = 0; k < 24; k++) // ALL OFF
    {
        digitalWrite(Serout[k], LOW);
    }
    UpdateSerialOutputs();

    // *********************************************
    // init I²C
    // *********************************************
    //pinMode(21,OUTPUT);
    //pinMode(22,OUTPUT);

    // *********************************************
    // init beeper
    // *********************************************
    pinMode(Beep, OUTPUT);
    digitalWrite(Beep, LOW);

    // *********************************************
    // init ADC
    // *********************************************
    //pinMode(ADC1, INPUT);
    //pinMode(ADC2, INPUT);
    //pinMode(ADC3, INPUT);

    // *********************************************
    // init DAC
    // *********************************************
    //pinMode(DAC, OUTPUT);
    dacWrite(DAC, 0);

    // *********************************************
    // init LCD
    // *********************************************
    lcd.begin();
    lcd.backlight();
}

void loop()
{
    // *****************************************************************
    // ******************** MAIN LOOP **********************************
    // *****************************************************************

    // ****************************************
    // LCD test
    // ****************************************
    lcd.clear();
    lcd.setCursor(0, 0); // (col,row)
    lcd.print("ESP32");
    lcd.setCursor(0, 1);
    lcd.print("trainer V1.0");
    lcd.setCursor(0, 2);
    lcd.print("ON4IPR");
    lcd.setCursor(0, 3);
    lcd.print("www.on4ipr.be");
    delay(2000);

    // ****************************************
    // blink On-board-LED 10 times
    // ****************************************
    lcd.clear();
    for (int i = 0; i <= 5; i++) // set outputs
    {
        lcd.setCursor(0, 0);
        lcd.print("White LED test ");
        lcd.print(i);
        digitalWrite(OnBoardLed, HIGH);
        delay(200);
        digitalWrite(OnBoardLed, LOW);
        delay(200);
    }
    // ******************************************
    // running light on direct digital out
    // ****************************************
    lcd.clear();
    for (int j = 0; j < 5; j++) // repeat
    {
        lcd.setCursor(0, 0);
        lcd.print("direct output test ");
        lcd.print(j);
        lcd.setCursor(0, 1);
        lcd.print("transistor outputs");
        for (int i = 0; i < 4; i++) // OUTPUT ON
        {
            digitalWrite(DDo[i], HIGH);
            delay(100);
            for (int i = 0; i < 4; i++) // ALL OFF
            {
                digitalWrite(DDo[i], LOW);
            }
        }
        delay(500);
    }
    // ****************************************
    // copy direct digital in to direct digital out
    // ****************************************
    lcd.clear();
    lcd.print("direct input test");
    lcd.setCursor(0, 1);
    lcd.print("inputs to outputs");
    lcd.setCursor(0, 2);
    lcd.print("push side-buttons");
    for (int j = 200; j > 0; j--) // repeat
    {
        lcd.setCursor(0, 3);
        lcd.print("    ");
        lcd.setCursor(0, 3);
        lcd.print(j);
        for (int i = 0; i < 4; i++) // copy 4 inputs to outputs
        {
            digitalWrite(DDo[i], digitalRead(DDi[i]));
            delay(10);
        }
    }
    // ****************************************
    // beeper test
    // ****************************************
    lcd.clear();
    for (int i = 0; i < 3; i++) // repeat
    {
        lcd.setCursor(0, 0);
        lcd.print("beeper test ");
        lcd.print(i);
        for (int j = 0; j < 4; j++) // repeat
        {
            for (int k = 0; k <= 150; k++) //tone
            {
                digitalWrite(Beep, HIGH);
                delayMicroseconds(100);
                digitalWrite(Beep, LOW);
                delayMicroseconds(100);
            }
            delay(100);
        }
        delay(1000);
    }

    // ****************************************
    // relays running
    // ****************************************
    lcd.clear();
    for (int i = 0; i < 5; i++) // repeat
    {
        lcd.setCursor(0, 0);
        lcd.print("relay output test ");
        lcd.print(i);
        for (int j = 23; j > 15; j--) //
        {
            Serout[j] = HIGH;
            UpdateSerialOutputs();
            delay(100);
            Serout[j] = LOW;
            UpdateSerialOutputs();
            delay(100);
        }
    }

    // ****************************************
    // Serial test
    // ****************************************
    lcd.clear();
    for (int i = 0; i < 5; i++) // repeat
    {
        lcd.setCursor(0, 0);
        lcd.print("serial port test ");
        lcd.print(i);
        lcd.setCursor(0, 1);
        lcd.print("open serial monitor");
        lcd.setCursor(0, 2);
        lcd.print("@9600Bd");
        Serial.println("ESP32-BASEBOARD");
        Serial.println("serial port test");
        delay(700);
    }

    // ****************************************
    // temperature test
    // ****************************************
    lcd.clear();
    for (int i = 0; i < 10; i++) // repeat
    {
        lcd.setCursor(0, 0);
        lcd.print("temperature test ");
        lcd.print(i, DEC);
        lcd.setCursor(0, 1);
        lcd.print("Temperature = ");
        lcd.print(temperature.readTemperatureC());
        delay(1000);
    }

    // ****************************************
    // RGD LED test
    // ****************************************
    lcd.clear();
    for (int i = 0; i < 3; i++) // repeat
    {
        lcd.setCursor(0, 0);
        lcd.print("RGB LED test ");
        lcd.print(i, DEC);

        lcd.setCursor(0, 1);
        lcd.print("BLUE    ");
        Serout[0] = 1;
        UpdateSerialOutputs();
        delay(500);
        Serout[0] = 0;
        UpdateSerialOutputs();

        lcd.setCursor(0, 1);
        lcd.print("GREEN   ");
        Serout[1] = 1;
        UpdateSerialOutputs();
        delay(500);
        Serout[1] = 0;
        UpdateSerialOutputs();

        lcd.setCursor(0, 1);
        lcd.print("RED   ");
        Serout[2] = 1;
        UpdateSerialOutputs();
        delay(500);
        Serout[2] = 0;
        UpdateSerialOutputs();
    }

    // ****************************************
    // LED Display test
    // ****************************************
    lcd.clear();
    for (int i = 0; i < 3; i++) // repeat
    {
        lcd.setCursor(0, 0);
        lcd.print("LED display test ");
        lcd.print(i, DEC);
        for (int k = 4; k < 8; k++) // repeat
        {
            Serout[k] = 1;
            UpdateSerialOutputs();
            for (int j = 8; j < 16; j++) // repeat
            {
                Serout[j] = 1;
                UpdateSerialOutputs();
                delay(50 + i * 150);
                Serout[j] = 0;
                UpdateSerialOutputs();
                //delay(50+i*150);
            }
            Serout[k] = 0;
            UpdateSerialOutputs();
        }
        Serout[3] = 1;
        UpdateSerialOutputs();
        delay(200);
        Serout[3] = 0;
        UpdateSerialOutputs();
    }

    // ****************************************
    // MUX inputs test
    // ****************************************
    lcd.clear();
    lcd.print("MUX input test ");
    lcd.setCursor(0, 1);
    lcd.print("push DDi0 to exit");
    lcd.setCursor(0, 2);
    lcd.print("0123456789012345");
    while (!digitalRead(DDi[0]))
    {
        ReadMuxInputs();
        for (int i = 0; i < 16; i++) // repeat
        {
            lcd.setCursor(i, 3);
            lcd.print(char(48 + MXi[i]));
        }
        delay(100);
    }

    // ****************************************
    // ADC test
    // ****************************************
    lcd.clear();
    lcd.print("ADC test, DDi0=exit");
    delay(1000);
    while (!digitalRead(DDi[0]))
    {
        lcd.setCursor(0, 1);
        lcd.print("ADC1 value: ");
        ADC1Val = analogRead(ADC1);
        lcd.print(ADC1Val * 3.3 / 4095);

        lcd.setCursor(0, 2);
        lcd.print("ADC2 value: ");
        ADC2Val = analogRead(ADC2);
        lcd.print(ADC2Val * 3.3 / 4095);

        lcd.setCursor(0, 3);
        lcd.print("ADC3 value: ");
        ADC3Val = analogRead(ADC3);
        lcd.print(ADC3Val * 3.3 / 4095);
        delay(100);
    }

    // ****************************************
    // DAC test
    // ****************************************
    lcd.clear();
    lcd.print("DAC test, DDi0=exit");
    delay(1000);
    for (int j = 0; j < 5; j++) // repeat
    {
        for (int i = 0; i < 17; i++) // repeat
        {
            lcd.setCursor(0, 1);
            lcd.print("DAC value:     ");
            lcd.setCursor(11, 1);
            lcd.print(i * i - 1);
            dacWrite(DAC, i * i - 1);
            delay(50);
        }
    }
    while (!digitalRead(DDi[0]))
    {
        lcd.setCursor(0, 1);
        ADC1Val = analogRead(ADC1) / 16;
        lcd.print("DAC value:        ");
        lcd.setCursor(11, 1);
        lcd.print(ADC1Val, 0);
        dacWrite(DAC, ADC1Val);
        delay(20);
    }

}
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$ zet alle outputs van shift registers volgens array
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void UpdateSerialOutputs()
{
    digitalWrite(ShiftRegStrb, LOW);
    digitalWrite(ShiftRegClk, LOW);
    for (int i = 0; i < 24; i++) // repeat
    {
        digitalWrite(ShiftRegData, Serout[i]);
        delayMicroseconds(10);
        digitalWrite(ShiftRegClk, HIGH);
        delayMicroseconds(10);
        digitalWrite(ShiftRegClk, LOW);
        delayMicroseconds(10);
    }
    digitalWrite(ShiftRegStrb, HIGH);
}

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$ Lees alle inputs van de MUX naar array
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void ReadMuxInputs()
{
    for (int i = 0; i < 16; i++) // repeat
    {
        switch (i)
        {
        case 0:
            digitalWrite(MuxA0, 0);
            digitalWrite(MuxA1, 0);
            digitalWrite(MuxA2, 0);
            digitalWrite(MuxA3, 0);
            break;
        case 1:
            digitalWrite(MuxA0, 1);
            digitalWrite(MuxA1, 0);
            digitalWrite(MuxA2, 0);
            digitalWrite(MuxA3, 0);
            break;
        case 2:
            digitalWrite(MuxA0, 0);
            digitalWrite(MuxA1, 1);
            digitalWrite(MuxA2, 0);
            digitalWrite(MuxA3, 0);
            break;
        case 3:
            digitalWrite(MuxA0, 1);
            digitalWrite(MuxA1, 1);
            digitalWrite(MuxA2, 0);
            digitalWrite(MuxA3, 0);
            break;
        case 4:
            digitalWrite(MuxA0, 0);
            digitalWrite(MuxA1, 0);
            digitalWrite(MuxA2, 1);
            digitalWrite(MuxA3, 0);
            break;
        case 5:
            digitalWrite(MuxA0, 1);
            digitalWrite(MuxA1, 0);
            digitalWrite(MuxA2, 1);
            digitalWrite(MuxA3, 0);
            break;
        case 6:
            digitalWrite(MuxA0, 0);
            digitalWrite(MuxA1, 1);
            digitalWrite(MuxA2, 1);
            digitalWrite(MuxA3, 0);
            break;
        case 7:
            digitalWrite(MuxA0, 1);
            digitalWrite(MuxA1, 1);
            digitalWrite(MuxA2, 1);
            digitalWrite(MuxA3, 0);
            break;
        case 8:
            digitalWrite(MuxA0, 0);
            digitalWrite(MuxA1, 0);
            digitalWrite(MuxA2, 0);
            digitalWrite(MuxA3, 1);
            break;
        case 9:
            digitalWrite(MuxA0, 1);
            digitalWrite(MuxA1, 0);
            digitalWrite(MuxA2, 0);
            digitalWrite(MuxA3, 1);
            break;
        case 10:
            digitalWrite(MuxA0, 0);
            digitalWrite(MuxA1, 1);
            digitalWrite(MuxA2, 0);
            digitalWrite(MuxA3, 1);
            break;
        case 11:
            digitalWrite(MuxA0, 1);
            digitalWrite(MuxA1, 1);
            digitalWrite(MuxA2, 0);
            digitalWrite(MuxA3, 1);
            break;
        case 12:
            digitalWrite(MuxA0, 0);
            digitalWrite(MuxA1, 0);
            digitalWrite(MuxA2, 1);
            digitalWrite(MuxA3, 1);
            break;
        case 13:
            digitalWrite(MuxA0, 1);
            digitalWrite(MuxA1, 0);
            digitalWrite(MuxA2, 1);
            digitalWrite(MuxA3, 1);
            break;
        case 14:
            digitalWrite(MuxA0, 0);
            digitalWrite(MuxA1, 1);
            digitalWrite(MuxA2, 1);
            digitalWrite(MuxA3, 1);
            break;
        case 15:
            digitalWrite(MuxA0, 1);
            digitalWrite(MuxA1, 1);
            digitalWrite(MuxA2, 1);
            digitalWrite(MuxA3, 1);
            break;
        }
        delayMicroseconds(10); // wait for stable selectlines
        MXi[i] = digitalRead(MuxDin); // read mux input
    }
}