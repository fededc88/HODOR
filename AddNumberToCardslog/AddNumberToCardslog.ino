#include <Wiegand.h>
#include <SD.h>

const int chipSelect = 4;
bool debug =true;

WIEGAND wg;

void setup() {
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

  Serial.println("Initializing Wiegand Protocol...");
  wg.begin(2,0,3,1,debug); //Inicializa protocolo Wiegand. Ultimo argumento Debug
  Serial.println("Wiegand initialized.");

}

void loop() {
  if(wg.available()){
   
char dataString[8] = ""; // make a string for assembling the data to log:

if(debug){
  
    Serial.println(" ");
    Serial.print("Wiegand HEX = ");
    Serial.print(wg.getCode(),HEX);
    Serial.print(", DECIMAL = ");
    Serial.print(wg.getCode());
    Serial.print(", Type W");
    Serial.println(wg.getWiegandType());
    Serial.println(" ");
}

sprintf(dataString, "%07lu" ,wg.getCode()); //Set number to 7 digits. Add left zeros. 

    File dataFile = SD.open("Cardslog.txt", FILE_WRITE);

     // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }    
  }

}
