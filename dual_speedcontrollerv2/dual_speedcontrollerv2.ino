unsigned int throttle=1500 , steering=1500, rightThrottle1=0, rightThrottle2=0, leftThrottle1=0, leftThrottle2=0; // only positive values
int m1Pin1=9, m1Pin2=6, m2Pin1=10, m2Pin2=11, thInPin=18, stInPin=19;

void setup() {
  pinMode(thInPin,INPUT);
  pinMode(stInPin,INPUT);
  pinMode(m1Pin1,OUTPUT);
  pinMode(m1Pin2,OUTPUT);
  pinMode(m2Pin1,OUTPUT);
  pinMode(m2Pin2,OUTPUT);
  Serial.begin(115200);
}

void loop() {
 throttle=pulseIn(thInPin, HIGH); 
 steering=pulseIn(stInPin, HIGH); 

 if (throttle > 1860){throttle = 1860;}
 if (throttle < 1060){throttle = 1060;}
 if (steering > 1860){steering = 1860;}
 if (steering < 1060){steering = 1060;}
 
switch(throttle){
   case 1600 ... 1860: // speed forward
   if (steering > 1600){ //right turn
    rightThrottle1=map(throttle,1600,1860,75,150);
    rightThrottle2=0;
    leftThrottle1=(map(throttle,1600,1860,75,150)- map(steering,1600,1860,0,50));
    leftThrottle2=0;  
    Serial.println ("forward right");
    }
  else if (steering < 1400){ //left turn 
    rightThrottle1=(map(throttle,1600,1860,75,150) - map(steering,1400,1060,0,50));
    rightThrottle2=0;
    leftThrottle1=map(throttle,1600,1860,75,150);
    leftThrottle2=0;  
    Serial.println ("forward left");
    }
  else //just forward
    {
    rightThrottle1=map(throttle,1600,1860,75,150);
    rightThrottle2=0;
    leftThrottle1=map(throttle,1600,1860,75,150);
    leftThrottle2=0;  
    Serial.println ("forward");
    }
  break;

  case 1401 ... 1599:  // speed neutral
  if (steering > 1600){ //right turn
    rightThrottle1=map(steering,1600,1860,75,150);
    rightThrottle2=0;
    leftThrottle1=0;
    leftThrottle2=0; 
    Serial.println ("turn right");
    }
  else if (steering < 1400){ //left turn
    rightThrottle1=0;
    rightThrottle2=0;
    leftThrottle1=map(steering,1400,1060,75,150);
    leftThrottle2=0;  
    Serial.println ("turn left");
    }
  else {
    rightThrottle1=0;
    rightThrottle2=0;
    leftThrottle1=0;
    leftThrottle2=0;
    Serial.println("brake");  
    }
  break;

  case 1060 ... 1400: // speed reverse
  if (steering > 1600){ //right turn
    rightThrottle1=0;
    rightThrottle2=map(throttle,1400,1100,70,100);
    leftThrottle1=0;
    leftThrottle2=(map(throttle,1400,1100,70,100) - map(steering,1600,1860,0,40));
    Serial.println ("reverse right");
    }
  else if (steering < 1400){ //left turn 
    rightThrottle1=0;
    rightThrottle2=(map(throttle,1400,1100,70,100) - map(steering,1400,1060,0,40));
    leftThrottle1=0;
    leftThrottle2=map(throttle,1400,1100,70,100);
    Serial.println ("reverse left");
    }
  else
    {
    rightThrottle1=0; //just reverse
    rightThrottle2=map(throttle,1400,1100,70,100);
    leftThrottle1=0;
    leftThrottle2=map(throttle,1400,1100,70,100);  
    Serial.println ("reverse");
    }    
  break;
  }
   // Serial.print ("l1:");
   // Serial.print (leftThrottle1);
   // Serial.print (",l2:");
  //  Serial.print (leftThrottle2);
   // Serial.print (",r1:");
   // Serial.print (rightThrottle1);
  //  Serial.print (",r2:");
  //  Serial.println (rightThrottle2);
  analogWrite(m1Pin1,rightThrottle1);
  analogWrite(m1Pin2,rightThrottle2);
  analogWrite(m2Pin1,leftThrottle1);
  analogWrite(m2Pin2,leftThrottle2);
  }
