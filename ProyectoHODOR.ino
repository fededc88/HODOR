#include <Wiegand.h>
#include <Wire.h>
#include <RTClib.h>
#include <SD.h>


//int ledPin = 13;              // LED connected to digital pin 13
const int chipSelect = 4;
bool debug =true;

int i=0;
int n=0;
   

RTC_DS3231 rtc;

WIEGAND wg;

char daysOfTheWeek[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char dateBuffer[3];



void setup(){

  
  Serial.begin(9600);
  while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Initializing SD card...");  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
   Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");  
  Serial.println("Cargando datos Numero Tarjetas...");

  
  File dataFile = SD.open("Cardslog.txt");

  if (dataFile) {

    char dataString[9] = "";
    n=dataFile.size()/9;
    Serial.print("numero de tarjetas");
    Serial.println(n,DEC);
    
    unsigned long Cards[n];
    n=0;
    
    dataFile.rewindDirectory();

    while (dataFile.available()){
      
      Serial.println(dataFile.read());
      
      dataString[i] = dataFile.read();
      i++;
      
      if(dataString[i] == 10){
        Cards[n]=atoi(dataString);
        Serial.print("Cards ");       
        Serial.println(Cards[n],DEC);
        i=0;
        n++;
      }
          
    }
      
      
    dataFile.close();
    
    Serial.println("done uploading Cards IDs.");
    
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Cardslog.txt file");
  }

  
   

  Serial.println("Initializing Wiegand Protocol...");
  wg.begin(2,0,3,1,debug); //Inicializa protocolo Wiegand. Ultimo argumento Debug
  Serial.println("Wiegand initialized.");
  
 // pinMode(ledPin, OUTPUT);      // sets the digital pin as output

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  //  while (1);
  }

  else if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}



void loop()
{

if(wg.available()){

    char dataString[8] = ""; // make a string for assembling the data to log:
    DateTime now = rtc.now();

    if(debug){
      
    Serial.println("");
    Serial.print("Wiegand HEX = ");
    Serial.print(wg.getCode(),HEX);
    Serial.print(", DECIMAL = ");
    Serial.print(wg.getCode());
    Serial.print(", Type W");
    Serial.println(wg.getWiegandType());
    Serial.println("");

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    }
    
    sprintf(dataString, "%07lu" ,wg.getCode()); //Set number to 7 digits. Add left zeros. 

    File dataFile = SD.open("Datalog.txt", FILE_WRITE);

     // if the file is available, write to it:
  if (dataFile) {    
    dataFile.print(dataString);
    dataFile.print(" ");
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    sprintf(dateBuffer,"%02u",now.month());
    dataFile.print(dateBuffer);
    dataFile.print('/');
    sprintf(dateBuffer,"%02u",now.day());
    dataFile.print(dateBuffer);
    dataFile.print(" ");
    sprintf(dateBuffer,"%02u",now.hour());
    dataFile.print(dateBuffer);
    dataFile.print(':');
    sprintf(dateBuffer,"%02u",now.minute());
    dataFile.print(dateBuffer);
    dataFile.print(':');
    sprintf(dateBuffer,"%02u",now.second());
    dataFile.print(dateBuffer);
    dataFile.print(" ");
    dataFile.println();
    dataFile.close();
    // print to the serial port too:
    Serial.print("dataString: ");
    Serial.println(dataString);
  }    
  }
}


