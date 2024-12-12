void showBuffer(){
  for (int displayRow=numRow; displayRow >=0 ; displayRow--){       // for every row on the display, 7..1
    for (int displayReg=numReg; displayReg >= 0 ; displayReg--){    // for every register in the buffer, 36..1
      shiftOut(matrixDataPin, matrixClkPin, LSBFIRST, displayBuffer[displayReg][displayRow]);
    }
  digitalWrite (rowPins[displayRow], LOW);
  digitalWrite (matrixOePin, LOW);
  delayMicroseconds(666);
  digitalWrite (rowPins[displayRow], HIGH);
  digitalWrite (matrixOePin, HIGH);
  }
}

void clearBuffer(){
  for (int x=0; x < numReg ; x++){
    for (int y=0; y < numRow ; y++){
      displayBuffer[x][y] = B00000000;
    }
  }

  for (int x=0; x < numChar ; x++){
    for (int y=0; y < numRow ; y++){
      textBuffer[x][y]= B00000000;
    }
  }
}

void message2Buffer(){
  clearBuffer();
  for (int messageChar=0; messageChar <= (message.length()) ; messageChar++){                     // for every character "messageChar" in message
    for (int displayRow=0; displayRow < numRow ; displayRow++){                                   // for every row in the display (0..6)
      if ((isAscii(message[messageChar])) and (!(message[messageChar] == 0))){                    // if character is ascci and not null
        textBuffer[messageChar][displayRow]= charset[(int(message[messageChar])-32)][displayRow]; // store the 8 bits for the character in array textBuffer[messageChar] for line [displayRow]
        //displayBuffer[messageChar][displayRow]=textBuffer[messageChar][displayRow];
      }
    }
  }

  for (int displayRow=0; displayRow < numRow ; displayRow++){                                     // for every row in the display (0..6)
    int displayBufferByte=0;                                                                      // start at the beginning of the displayBuffer
    uint8_t currentByte = 0;                                                                      // start at the beginning of the textBuffer
    int bitPos=0;                                                                                 // start at bit 0 of the first displayBuffer byte
    for (int textBufferChar=0; textBufferChar < numChar; textBufferChar++){                       // for every byte "bufferChar" in textBuffer

      for (int charBit = 3; charBit < 8; charBit++) {                                             // for every bit 3..8 from the Byte
        uint8_t currentBit = (textBuffer[textBufferChar][displayRow] >> (7 - charBit)) & 0x01;    // shift to the correct position and mask                           
        currentByte |= (currentBit << (7 - bitPos));                                              // add the bit to the current working Byte    
        bitPos++;                                                                                 // go to the next bit of the textBuffer Byte
        if (((textBufferChar==18) or(textBufferChar==37)) and (bitPos==7)){                       // if this is the last textBufferByte of the display and it is the last bit of the textbuffer
          bitPos++;                                                                               // skip one bit in the displaybuffer because that isn't used
        }
        if (bitPos == 8) {                                                                        // if we are over bit 8 of the working byte  
          displayBuffer[displayBufferByte++][displayRow] = currentByte;                           // switch to the next display buffer byte
          currentByte = 0;                                                                        // clear the content of the working byte
          bitPos = 0;                                                                             // and start at the beginning of the working byte
        }
        if (bitPos > 0) {                                                                         // when finished and the current bit counter is more than zero
          displayBuffer[displayBufferByte][displayRow] = currentByte;                             // write the remaining bits to the displayBuffer
        }
      }
      
    }  
  }
}

int checkDST(time_t dt){
  int DST = 0;
  if (month(dt)<3 || month(dt)>10) DST=0; 
  if (month(dt)>3 && month(dt)<10) DST=1; 
  if (month(dt)==3 && day(dt)<25) DST=0; 
  if (month(dt)==10 && day(dt)<25) DST=1; 
  if ((month(dt)==3 && hour(dt) + 24 * day(dt))>=(1 + 24*(31 - (5 * year(dt) /4 + 4) % 7)) or (month(dt)==10 && (hour(dt) + 24 * day(dt))<(1 + 24*(31 - (5 * year(dt) /4 + 1) % 7)))) DST=1;
  return DST;
}

void createElements(const char *str){
  sscanf(str, "%d-%d-%d %d:%d:%d", &calcYear, &calcMonth, &calcDay, &calcHour, &calcMinute, &calcSecond);
  tm.Year = CalendarYrToTm(calcYear);
  tm.Month = calcMonth;
  tm.Day = calcDay;
  tm.Hour = calcHour;
  tm.Minute = calcMinute;
  tm.Second = calcSecond;
}

void updateBIGDisplay() {
  local_time = MyTZ.toLocal(timeClient.getEpochTime());
  createElements((targetTime).c_str());
  toTime = makeTime(tm);
  toDateSeconds = (toTime - local_time);

  if ((checkDST(toTime)==0) and (checkDST(local_time)==1)) toDateSeconds += 3600;
  if ((checkDST(toTime)==1) and (checkDST(local_time)==0)) toDateSeconds -= 3600;

  if (toDateSeconds < 0) toDateSeconds = 0;

  toDateMinutes = floor(toDateSeconds / 60);
  toDateHours = floor(toDateMinutes / 60);
  toDateDays = floor(toDateHours / 24);

  countDownDays = toDateDays;

  if (countDownDays >= 1) {
    countDownHours = toDateHours -(countDownDays * 24); 
    countDownMinutes = toDateMinutes - (countDownHours * 60) - ((countDownDays *24) * 60); 
  
    digitalWrite(bigStrobePin, LOW);
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[countDownMinutes % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[(countDownMinutes /10) % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[10]);
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[countDownHours % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[(countDownHours/10) % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[10]);
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[countDownDays % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[(countDownDays/10) % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[(countDownDays/100) % 10]);  
    delay(1);
    digitalWrite(bigStrobePin, HIGH);
  }
  else {
    countDownHours = toDateHours; 
    countDownMinutes = toDateMinutes - ((toDateHours -(countDownDays * 24)) * 60) - ((countDownDays *24) * 60); 
    countDownSeconds = toDateSeconds - (countDownMinutes * 60) - (((toDateHours -(countDownDays * 24)) * 60) *60) - (((countDownDays * 24) * 60) * 60); 
 
    digitalWrite(bigStrobePin, LOW);
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[countDownSeconds % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[(countDownSeconds /10) % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[10]);
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[countDownMinutes % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[(countDownMinutes /10) % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[10]);
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[countDownHours % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[(countDownHours/10) % 10]);  
    shiftOut(bigDataPin, bigClockPin, MSBFIRST, segChar[(countDownHours/100) % 10]);  
    delay(1);
    digitalWrite(bigStrobePin, HIGH);
  }
  delay(500);
}

void tick()
{
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void initFS() {
  if (!LittleFS.begin(false /* false: Do not format if mount failed */)) {
    Serial.println("Failed to mount LittleFS");
    if (!LittleFS.begin(true /* true: format */)) {
      Serial.println("Failed to format LittleFS");
    } else {
      Serial.println("LittleFS formatted successfully");
      filesystemOK = true;
    }
  } else { // Initial mount success
    filesystemOK = true;
  }
}

bool loadConfig() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }
  String fsTargetTime = doc["targetTime"];
  targetTime=fsTargetTime.c_str();
  String fsMessage = doc["message"];
  message=fsMessage.c_str();
  Serial.println("Config file read");
  message2Buffer();
  return true;
}

bool saveConfig() {
  StaticJsonDocument<200> doc;
  doc["targetTime"] = targetTime;
  doc["message"] = message;
  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  serializeJson(doc, configFile);
  Serial.println("Config file written");
  return true;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
