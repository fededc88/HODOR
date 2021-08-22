#include <SD.h>
#include <EEPROM.h>

const int chipSelect = 4;
bool debug =true;

int i=0;
int nCards=0;
int eeAddress=0;


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

  if  (SD.exists("Cardslog.txt")){

   File dataFile = SD.open("Cardslog.txt");

  Serial.println("Cargando datos Numero Tarjetas...");

  if (dataFile) {

    char dataString[9] = "";
    nCards=dataFile.size()/9;
    Serial.print("Numero de tarjetas registradas: ");
    Serial.println(nCards,DEC);
    
    unsigned long Cards=0,CardsCheck=0; 
    
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
        Cards=atol(dataString);
        EEPROM.get(eeAddress, CardsCheck);

        if(Cards != CardsCheck){ //compara q el numero en EEprom sea distinto
        
        EEPROM.put(eeAddress, Cards);
        Serial.println("Dato actualizado");
        }
               
        Serial.print("Cards: ");
        Serial.println(Cards,DEC);

        EEPROM.get(eeAddress, CardsCheck);       

        Serial.print("EEPROM direcction: ");
        Serial.print(eeAddress);
        Serial.print(" :");       
        Serial.println(CardsCheck,DEC);

        eeAddress += sizeof(long);
        i=0;

      }   else{
              i++;
      }

          
    }
          
    dataFile.close();
    
    Serial.println("done uploading Cards IDs.");

    if (SD.remove("Cardslog.txt")){
      Serial.println("Cardslog.txt deleted...");
    }
      else{
       Serial.println("ERROR!! Cardslog.txt Wasn't deleted...");
      }

      } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Cardslog.txt file");
    }
  } else {

    Serial.println("Cardslog.txt isnt presente. EEPROM memory used");
    
  }


}

void loop() {
  // put your main code here, to run repeatedly:

}
