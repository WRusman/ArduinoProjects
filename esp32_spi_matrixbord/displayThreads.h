void display1Thread(){
  switch(thread1Counter)
  {
  case 1 : writeDisplay(1,"Display 1");break;
  case 2 : clearDisplay(1);break;
  case 3 : writeDisplay(1,"111111111111");break;
  case 4 : clearDisplay(1);break;
  }
  thread1Counter++;
  if (thread1Counter == 5){thread1Counter=1;} ;
}

void display2Thread(){
  switch(thread2Counter)
  {
  case 1 : writeDisplay(2,"Display 2");break;
  case 2 : clearDisplay(2);break;
  case 3 : writeDisplay(2,"222222222222");break;
  case 4 : clearDisplay(2);break;
  }
  thread2Counter++;
  if (thread2Counter == 5){thread2Counter=1;} ;
}

void display3Thread(){
  switch(thread3Counter)
  {
  case 1 : writeDisplay(3,"Display 3");break;
  case 2 : clearDisplay(3);break;
  case 3 : writeDisplay(3,"333333333333");break;
  case 4 : clearDisplay(3);break;
  }
  thread3Counter++;
  if (thread3Counter == 5){thread3Counter=1;} ;
}

void display4Thread(){
  switch(thread4Counter)
  {
  case 1 : writeDisplay(4,"Display 4");break;
  case 2 : clearDisplay(4);break;
  case 3 : writeDisplay(4,"444444444444");break;
  case 4 : clearDisplay(4);break;
  }
  thread4Counter++;
  if (thread4Counter == 5){thread4Counter=1;} ;
}

void display5Thread(){
  switch(thread5Counter)
  {
  case 1 : writeDisplay(5,"Display 5");break;
  case 2 : clearDisplay(5);break;
  case 3 : writeDisplay(5,"555555555555");break;
  case 4 : clearDisplay(5);break;
  }
  thread5Counter++;
  if (thread5Counter == 5){thread5Counter=1;} ;
}

void display6Thread(){
  switch(thread6Counter)
  {
  case 1 : writeDisplay(6,"Display 6");break;
  case 2 : clearDisplay(6);break;
  case 3 : writeDisplay(6,"666666666666");break;
  case 4 : clearDisplay(6);break;
  }
  thread6Counter++;
  if (thread6Counter == 5){thread6Counter=1;} ;
}

void display7Thread(){
  switch(thread7Counter)
  {
  case 1 : writeDisplay(7,"Display 7");break;
  case 2 : clearDisplay(7);break;
  case 3 : writeDisplay(7,"777777777777");break;
  case 4 : clearDisplay(7);break;
  }
  thread7Counter++;
  if (thread7Counter == 5){thread7Counter=1;} ;
}

void display8Thread(){
}
