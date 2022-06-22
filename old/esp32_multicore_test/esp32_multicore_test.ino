TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() {
  xTaskCreatePinnedToCore(
               Task1code, /* Task function. */
               "Task1",   /* name of task. */
               10000,     /* Stack size of task */
               NULL,      /* parameter of the task */
               1,         /* priority of the task */
               &Task1,    /* Task handle to keep track of created task */
               0);        /* pin task to core 0 */           
  delay(500);
  
  xTaskCreatePinnedToCore(
               Task2code,  /* Task function. */
               "Task2",    /* name of task. */
               20000,      /* Stack size of task */
               NULL,       /* parameter of the task */
               1,          /* priority of the task */
               &Task2,     /* Task handle to keep track of created task */
               1);         /* pin task to core 1 */         
  delay(500);
}

void Task1code( void * pvParameters ){
Serial.print("Task1 running on core ");
Serial.println(xPortGetCoreID());
for(;;) {//infinite loop
digitalWrite(led, HIGH);
delay(500);
digitalWrite(led, LOW);
delay(500);
}
}

void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  for(;;){
    float t = dht.readTemperature();
     Serial.print("Temperature: ");
     Serial.print(t);
   delay(2000);
  }
}

void loop() {
  }
