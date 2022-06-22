void display1Thread(){
  switch(thread1Counter)
  {
  case 1 : writeDisplay(1,"Frikandel?");break;
  case 2 : clearDisplay(1);break;
  case 3 : writeDisplay(1,"Jummie!");break;
  case 4 : clearDisplay(1);break;
  }
  thread1Counter++;
  if (thread1Counter == 5){thread1Counter=1;} ;
}

void display2Thread(){
}

void display3Thread(){
}

void display4Thread(){
}

void display5Thread(){
}

void display6Thread(){
}

void display7Thread(){
}

void display8Thread(){
}
