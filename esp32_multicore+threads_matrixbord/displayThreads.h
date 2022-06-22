void display1Thread(){
  int x = map(thread1Counter,1,80,1,4);
  switch(x)
  {
  case 1 : writeDisplay(1,"Mamma Mooie Mini");break;
  case 2 : clearDisplay(1);break;
  case 3 : writeDisplay(1,"Jummie!");break;
  case 4 : clearDisplay(1);break;
  }
  thread1Counter++;
  if (thread1Counter == 91){thread1Counter=1;} ;
}

void display2Thread(){
}

void display3Thread(){
}

void display4Thread(){
}

void display5Thread(){
}

void blinkLedThread(){
  if ((thread6Counter % 2) > 0) {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else{
    digitalWrite(LED_BUILTIN,LOW);
  }
  thread6Counter++;
  if (thread6Counter == 99){thread6Counter=1;} ;
}
