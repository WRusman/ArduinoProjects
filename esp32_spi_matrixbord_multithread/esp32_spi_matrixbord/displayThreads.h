void outputThread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  digitalWrite (clkPin, 0);
  SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE0));
  for (int y=numRow; y >0 ; y--){      // for every row in the character, 0..6 rows of leds
    for (int x=(numRegister*numDisplay); x >= 0 ; x--){    // for byte on the line buffer, 0..12 shift registers of 8 bit
      SPI.transfer(rowBuffer[x][y]);
    }
    digitalWrite (rowPins[y], 0);
    digitalWrite (oePin, 0);
    delayMicroseconds(300);
    digitalWrite (oePin, 1);
    digitalWrite (rowPins[y], 1);
  }
SPI.endTransaction();

}

void display1Thread(void *pvParameters)  // This is a task.
{
  byte* localRowBuffer = (byte*)pvParameters;
  switch(thread1Counter)
  {
  case 1 : writeDisplay(1,"Display 1");break;
  case 2 : clearDisplay(1);break;
  case 3 : writeDisplay(1,"111111111111");break;
  case 4 : clearDisplay(1);break;
  }
  thread1Counter++;
  vTaskDelay(1000);
  if (thread1Counter == 5){thread1Counter=1;} ;
}

void display2Thread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  switch(thread2Counter)
  {
  case 1 : writeDisplay(2,"Display 2");break;
  case 2 : clearDisplay(2);break;
  case 3 : writeDisplay(2,"222222222222");break;
  case 4 : clearDisplay(2);break;
  }
  thread2Counter++;
  vTaskDelay(2000);
  if (thread2Counter == 5){thread2Counter=1;} ;
}

void display3Thread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  switch(thread3Counter)
  {
  case 1 : writeDisplay(3,"Display 3");break;
  case 2 : clearDisplay(3);break;
  case 3 : writeDisplay(3,"333333333333");break;
  case 4 : clearDisplay(3);break;
  }
  thread3Counter++;
  vTaskDelay(3000);
  if (thread3Counter == 5){thread3Counter=1;} ;
}

void display4Thread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  switch(thread4Counter)
  {
  case 1 : writeDisplay(4,"Display 4");break;
  case 2 : clearDisplay(4);break;
  case 3 : writeDisplay(4,"444444444444");break;
  case 4 : clearDisplay(4);break;
  }
  thread4Counter++;
  vTaskDelay(4000);
  if (thread4Counter == 5){thread4Counter=1;} ;
}

void display5Thread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  switch(thread5Counter)
  {
  case 1 : writeDisplay(5,"Display 5");break;
  case 2 : clearDisplay(5);break;
  case 3 : writeDisplay(5,"555555555555");break;
  case 4 : clearDisplay(5);break;
  }
  thread5Counter++;
  vTaskDelay(5000);
  if (thread5Counter == 5){thread5Counter=1;} ;
}

void display6Thread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  switch(thread6Counter)
  {
  case 1 : writeDisplay(6,"Display 6");break;
  case 2 : clearDisplay(6);break;
  case 3 : writeDisplay(6,"666666666666");break;
  case 4 : clearDisplay(6);break;
  }
  thread6Counter++;
  vTaskDelay(6000);
  if (thread6Counter == 5){thread6Counter=1;} ;
}

void display7Thread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  switch(thread7Counter)
  {
  case 1 : writeDisplay(7,"Display 7");break;
  case 2 : clearDisplay(7);break;
  case 3 : writeDisplay(7,"777777777777");break;
  case 4 : clearDisplay(7);break;
  }
  thread7Counter++;
  vTaskDelay(7000);
  if (thread7Counter == 5){thread7Counter=1;} ;
}

void display8Thread(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  switch(thread7Counter)
  {
  case 1 : writeDisplay(8,"Display 8");break;
  case 2 : clearDisplay(8);break;
  case 3 : writeDisplay(8,"888888888888");break;
  case 4 : clearDisplay(8);break;
  }
  thread8Counter++;
  vTaskDelay(8000);
  if (thread8Counter == 5){thread8Counter=1;} ;
}
