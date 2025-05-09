#ifndef FILEMGMT_H
#define FILEMGMT_H
#include <SD.h>

extern File g_file;
char fn[] =     "trker000.txt";
char img_fn[] = "im000000.bmp";

void startSD(){
  static uint64_t coolDown = 0;

  if (millis() - coolDown < 3000)
    return;
    
  coolDown = millis();

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  uint8_t err = 0;
  //pinMode(MISO, INPUT_PULLUP);
  if (!(err = SD.begin(A1))) {
    Serial.println("Card failed, or not present");
  }else{
  Serial.println("card initialized.");
  }
}

void initGlobalFilename(void){
    while (SD.exists(fn)){    
        if(fn[7]++=='9'){
          if(fn[6]++=='9'){
            fn[5]++;
            fn[6] = '0';
          }
          fn[7] = '0';
        }
    }
}

void setGlobalFile(void){
  bool goodFile = false;
  //do{
    if(!g_file)
      g_file = SD.open(fn, FILE_WRITE);
    if(g_file){
      if (g_file.size() > 2147483645){
        g_file.close();
        Serial.println("Start new file.");
        if(fn[7]++=='9'){
          if(fn[6]++=='9'){
            fn[5]++;
            fn[6] = '0';
          }
          fn[7] = '0';
        }  
      }
      else{
        goodFile = true;
      }
    }
    else{
      startSD();
    }
  //}while (!goodFile);
}

#endif
