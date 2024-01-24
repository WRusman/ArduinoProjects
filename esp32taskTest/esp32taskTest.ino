

void setup() {
  
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  
  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink1
    ,  "TaskBlink1"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskBlink2
    ,  "TaskBlink2"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskBlink3
    ,  "TaskBlink3"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

}

void loop()
{
  // Empty. Things are done in Tasks.
}

void TaskBlink1(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(13, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(21, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(100);  // one tick delay (15ms) in between reads for stability
    digitalWrite(21, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay(100);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskBlink2(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(23, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(23, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(900);  // one tick delay (15ms) in between reads for stability
    digitalWrite(23, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay(900);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskBlink3(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(22, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(22, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(400);  // one tick delay (15ms) in between reads for stability
    digitalWrite(22, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay(400);  // one tick delay (15ms) in between reads for stability
  }
}
