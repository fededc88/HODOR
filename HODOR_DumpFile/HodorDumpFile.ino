#include <SD.h>

const int chipSelect = 4;
bool debug =true;

int i=0;
int n=0;


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
  Serial.println("Cargando datos Numero Tarjetas...");

   File dataFile = SD.open("Cardslog.txt");

  if (dataFile) {

    char dataString[9] = "";
    n=dataFile.size()/9;
    Serial.print("numero de tarjetas registradas: ");
    Serial.println(n,DEC);
    
    unsigned long Cards[n];
    n=0;
    
    dataFile.rewindDirectory();

    while (dataFile.available()){

      dataString[i] = dataFile.read();


      Serial.print("Char dataString[");
      Serial.print(i);
      Serial.print("] :");
      Serial.print(dataString[i]);
      Serial.print(" DEC:");
      Serial.println(dataString[i],DEC);      
      

      
      if(dataString[i] == '\n'){
        Cards[n]=atol(dataString);
        Serial.print("Cards[");
        Serial.print(n);
        Serial.print("] :");       
        Serial.println(Cards[n],DEC);
        i=0;
        n++;
      } else{
        i++;
      }
    }
          
    dataFile.close();
    
    Serial.println("done uploading Cards IDs.");

      } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Cardslog.txt file");
  }


}

void loop() {
  // put your main code here, to run repeatedly:

}
