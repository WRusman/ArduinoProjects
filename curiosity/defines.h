bool focus=false;
int faceX, faceY;

ThreadController controll = ThreadController();

Thread* eye1Thread = new Thread();
Thread* eye2Thread = new Thread();
Thread* eye3Thread = new Thread();
Thread* eye4Thread = new Thread();
Thread* eye5Thread = new Thread();
Thread* eye6Thread = new Thread();
Thread* eye7Thread = new Thread();
Thread* eye1BlinkThread = new Thread();
Thread* eye2BlinkThread = new Thread();
Thread* eye3BlinkThread = new Thread();
Thread* eye4BlinkThread = new Thread();
Thread* eye5BlinkThread = new Thread();
Thread* eye6BlinkThread = new Thread();
Thread* eye7BlinkThread = new Thread();

Thread* personSensorThread = new Thread();


Servo eye1LeftLowLid;
Servo eye1RightLowLid;
Servo eye1LeftUpLid;
Servo eye1RightUpLid;
Servo eye1LeftRight;
Servo eye1UpDown;

Servo eye2LeftLowLid;
Servo eye2RightLowLid;
Servo eye2LeftUpLid;
Servo eye2RightUpLid;
Servo eye2LeftRight;
Servo eye2UpDown;

Servo eye3LeftLowLid;
Servo eye3RightLowLid;
Servo eye3LeftUpLid;
Servo eye3RightUpLid;
Servo eye3LeftRight;
Servo eye3UpDown;

Servo eye4LeftLowLid;
Servo eye4RightLowLid;
Servo eye4LeftUpLid;
Servo eye4RightUpLid;
Servo eye4LeftRight;
Servo eye4UpDown;

Servo eye5LeftLowLid;
Servo eye5RightLowLid;
Servo eye5LeftUpLid;
Servo eye5RightUpLid;
Servo eye5LeftRight;
Servo eye5UpDown;

Servo eye6LeftLowLid;
Servo eye6RightLowLid;
Servo eye6LeftUpLid;
Servo eye6RightUpLid;
Servo eye6LeftRight;
Servo eye6UpDown;

Servo eye7LeftLowLid;
Servo eye7RightLowLid;
Servo eye7LeftUpLid;
Servo eye7RightUpLid;
Servo eye7LeftRight;
Servo eye7UpDown;

void attachServos(){
  eye1LeftLowLid.attach(A0);
  eye1RightLowLid.attach(A1);
  eye1LeftUpLid.attach(A2);
  eye1RightUpLid.attach(A3);
  eye1LeftRight.attach(A4);
  eye1UpDown.attach(A5);
  
  eye2LeftLowLid.attach(A6);
  eye2RightLowLid.attach(A7);
  eye2LeftUpLid.attach(A8);
  eye2RightUpLid.attach(A9);
  eye2LeftRight.attach(A12);
  eye2UpDown.attach(A13);
  
  eye3LeftLowLid.attach(2);
  eye3RightLowLid.attach(3);
  eye3LeftUpLid.attach(4);
  eye3RightUpLid.attach(5);
  eye3LeftRight.attach(6);
  eye3UpDown.attach(7);
  
  eye4LeftLowLid.attach(8);
  eye4RightLowLid.attach(9);
  eye4LeftUpLid.attach(10);
  eye4RightUpLid.attach(11);
  eye4LeftRight.attach(12);
  eye4UpDown.attach(13);
  
  eye5LeftLowLid.attach(22);
  eye5RightLowLid.attach(24);
  eye5LeftUpLid.attach(26);
  eye5RightUpLid.attach(28);
  eye5LeftRight.attach(30);
  eye5UpDown.attach(32);
  
  eye6LeftLowLid.attach(34);
  eye6RightLowLid.attach(36);
  eye6LeftUpLid.attach(38);
  eye6RightUpLid.attach(40);
  eye6LeftRight.attach(42);
  eye6UpDown.attach(44);
  
  eye7LeftLowLid.attach(23);
  eye7RightLowLid.attach(25);
  eye7LeftUpLid.attach(27);
  eye7RightUpLid.attach(29);
  eye7LeftRight.attach(31);
  eye7UpDown.attach(33);
  }
