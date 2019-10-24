#define m1Pin1 3
#define m1Pin2 5
#define m2Pin1 6
#define m2Pin2 9

#define thInPin 20
#define stInPin 21

unsigned int throttle=1500 , steering=1500, rightThrottle=0, leftThrottle=0; // only positive values

void setup() {
  pinMode(thInPin,INPUT);
  pinMode(stInPin,INPUT);
  digitalWrite(m1Pin1, 0);
  digitalWrite(m1Pin2, 0);
  digitalWrite(m2Pin1, 0);
  digitalWrite(m2Pin2, 0);
  Serial.begin(9600);
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
    rightThrottle=constrain((map(throttle,1600,1860,155,255) - map(steering,1600,1860,0,100)),155,255);
    leftThrottle=map(throttle,1600,1860,155,255);
    }
  else if (steering < 1400){ //left turn 
    rightThrottle=map(throttle,1600,1860,155,255);
    leftThrottle=constrain((map(throttle,1600,1860,155,255) - map(steering,1400,1060,0,100)),155,255);
    }
  else
    {
    rightThrottle=map(throttle,1600,1860,155,255);
    leftThrottle=map(throttle,1600,1860,155,255);  
    }
  break;

  case 1401 ... 1599:  // speed neutral
  if (steering > 1600){ //right turn
    rightThrottle=map(steering,1600,1860,155,255);
    leftThrottle=0;
    }
  else if (steering < 1400){ //left turn
    rightThrottle=0;
    leftThrottle=map(steering,1400,1060,155,255);
    }
  else {
    leftThrottle = 0;
    rightThrottle = 0;
    }
  break;

  case 1060 ... 1400: // speed reverse
  if (steering > 1600){ //right turn
    rightThrottle=constrain((map(throttle,1400,1060,155,255) - map(steering,1600,1860,0,100)),155,255);
    leftThrottle=map(throttle,1400,1060,155,255);
    }
  else if (steering < 1400){ //left turn 
    rightThrottle=map(throttle,1400,1060,155,255);
    leftThrottle=constrain((map(throttle,1400,1060,155,255) - map(steering,1400,1060,0,100)),155,255);
    }
  else
    {
    rightThrottle=map(throttle,1400,1060,155,255);
    leftThrottle=map(throttle,1400,1060,155,255);  
    }    
  break;
  }
  
  Serial.print("Throttle:"); 
  Serial.println(throttle);
  Serial.print("Steering:"); 
  Serial.println(steering);
  Serial.print("Left motor:"); 
  Serial.println(leftThrottle);
  Serial.print("Right motor:"); 
  Serial.println(rightThrottle);
  delay(1000); 
}
