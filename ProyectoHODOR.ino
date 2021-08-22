#include <Wiegand.h>
#include <Wire.h>
#include <RTClib.h>
#include <SD.h>
#include <EEPROM.h>



const int chipSelect = 4;
bool debug =true;
bool AbrirPuerta =false;

int i=0;
int n=0;
int nCards=0;
int eeAddnCards = EEPROM.length()-2;
int eeAddress=0;
unsigned long Cards=0,CardsCheck=0;
//int ledPin = 13;
   

RTC_DS3231 rtc;

WIEGAND wg;

char dateBuffer[3];


void setup(){

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  
  Serial.begin(9600);
  while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
  }
  if (!SD.begin(chipSelect)) {
   Serial.println("Card failed, or not present");
  }
  
 if  (SD.exists("Cardslog.txt")){

   File dataFile = SD.open("Cardslog.txt");

  if (dataFile) {

    char dataString[9] = "";
    nCards=dataFile.size()/9;
    Serial.print("Numero de tarjetas registradas: ");
    Serial.println(nCards,DEC);
    
    dataFile.rewindDirectory();

    while (dataFile.available()){

      dataString[i] = dataFile.read();
     
      if(dataString[i] == '\n'){
        Cards=atol(dataString);
        EEPROM.get(eeAddress, CardsCheck);

        if(Cards != CardsCheck){ //compara q el numero en EEprom sea distinto
        
        EEPROM.put(eeAddress, Cards);
        Serial.println("Dato actualizado");
        }
               
        EEPROM.get(eeAddress, CardsCheck);       
        eeAddress += sizeof(long);
        i=0;

      }   else{
        i++;
      }

          
    }
          
    dataFile.close();
    
    Serial.println("done uploading Cards IDs.");

        for(eeAddress;eeAddress < (EEPROM.length()-2);eeAddress+=1){
         EEPROM.write(eeAddress, 0);
    }

     EEPROM.get(eeAddnCards, CardsCheck);

     if(nCards != CardsCheck){

        EEPROM.put(eeAddnCards, nCards);
      
     }

    if (!SD.remove("Cardslog.txt")){
       Serial.println("ERROR!! Cardslog.txt Wasn't deleted...");
    }

      } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Cardslog.txt file");
    }
  } else {

    Serial.println("Cardslog.txt isnt presente. EEPROM memory used");
    EEPROM.get(eeAddnCards, nCards);
    
  }

  //Actualico nCards para saber el numero de tarjetas presentes. 

     
     EEPROM.get(eeAddnCards, CardsCheck);

     if(nCards != CardsCheck){

        EEPROM.put(eeAddnCards, nCards);
      
     }

  wg.begin(2,0,3,1,debug); //Inicializa protocolo Wiegand. Ultimo argumento Debug
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  else if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}



void loop()
{

if(wg.available()){

  for(eeAddress=0;eeAddress<(nCards*4); eeAddress += sizeof(long)){
    
    EEPROM.get(eeAddress, CardsCheck);

    if(wg.getCode()== CardsCheck){
      
      
     Serial.println("EEEEE... PUERTA ABIERTA");
     AbrirPuerta = true;
      break;
    }
  }

    char dataString[8] = ""; // make a string for assembling the data to log:
    DateTime now = rtc.now();

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

  if(AbrirPuerta){
    
    AbrirPuerta=false;
    digitalWrite(7, LOW);
    delay(5000);
    digitalWrite(7, HIGH);
    
  }
}


