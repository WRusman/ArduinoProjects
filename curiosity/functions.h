void eye1Move(){
  if (!focus){
    Serial.println("Eye1 moving ");
    eye1LeftRight.write(random(60,120));
    eye1UpDown.write(random(40,90));
    eye1Thread->setInterval(random(3000,7000));
    }
    else
    {
    //Serial.println("Eye1 Focus");
    eye1LeftRight.write(faceX);
    eye1UpDown.write(faceY);
    eye1Thread->setInterval(50);
  }
}

void eye2Move(){
  if (!focus){
    Serial.println("Eye2 moving ");
    eye2LeftRight.write(random(60,120));
    eye2UpDown.write(random(40,110));
    eye2Thread->setInterval(random(3000,7000));
  }
  else
  {
    //Serial.println("Eye2 Focus");
    eye2LeftRight.write(faceX);
    eye2UpDown.write(faceY);
    eye2Thread->setInterval(50);
  }
}

void eye3Move(){
  if (!focus){
    Serial.println("Eye3 moving ");
    eye3LeftRight.write(random(60,120));
    eye3UpDown.write(random(40,110));
    eye3Thread->setInterval(random(3000,7000));
  }
  else
  {
    //Serial.println("Eye3 Focus");
    eye3LeftRight.write(faceX);
    eye3UpDown.write(faceY);
   eye3Thread->setInterval(50);
  }
}

void eye4Move(){
  if (!focus){
    Serial.println("Eye4 moving ");
    eye4LeftRight.write(random(60,120));
    eye4UpDown.write(random(40,110));
    eye4Thread->setInterval(random(3000,7000));
  }
  else
  {
    //Serial.println("Eye4 Focus");
    eye4LeftRight.write(faceX);
    eye4UpDown.write(faceY);
   eye4Thread->setInterval(50);
  }
}

void eye5Move(){
  if (!focus){
    Serial.println("Eye5 moving ");
    eye5LeftRight.write(random(60,120));
    eye5UpDown.write(random(40,110));
    eye5Thread->setInterval(random(3000,7000));
  }
  else
  {
    //Serial.println("Eye5 Focus");
    eye5LeftRight.write(faceX);
    eye5UpDown.write(faceY);
    eye5Thread->setInterval(50);
  }
}

void eye6Move(){
  if (!focus){
    Serial.println("Eye6 moving ");
    eye6LeftRight.write(random(60,120));
    eye6UpDown.write(random(40,110));
    eye6Thread->setInterval(random(3000,7000));
  }
  else
  {
    //Serial.println("Eye6 Focus");
    eye6LeftRight.write(faceX);
    eye6UpDown.write(faceY);
    eye6Thread->setInterval(50);
  }
}

void eye7Move(){
  if (focus==false){
    Serial.println("Eye7 moving ");
    eye7LeftRight.write(random(60,120));
    eye7UpDown.write(random(40,110));
    eye7Thread->setInterval(random(3000,7000));
  }
  else
  {
    //Serial.println("Eye7 Focus");
    eye7LeftRight.write(faceX);
    eye7UpDown.write(faceY);
    eye7Thread->setInterval(50);
  }
}

void eye1Blink(){
  if (focus==false){
    Serial.println("Eye1 blink ");
    eye1LeftLowLid.write(130);
    eye1RightLowLid.write(80);
    eye1LeftUpLid.write(80);
    eye1RightUpLid.write(130);
    delay(200);
    eye1LeftLowLid.write(60);
    eye1RightLowLid.write(120);
    eye1LeftUpLid.write(120);
    eye1RightUpLid.write(60);
    eye1BlinkThread->setInterval(random(3000,8000));
  }
  else
  {
    eye1LeftLowLid.write(60);
    eye1RightLowLid.write(120);
    eye1LeftUpLid.write(120);
    eye1RightUpLid.write(60);
  }
}

void eye2Blink(){
  if (focus==false){
    Serial.println("eye2 blink ");
    eye2LeftLowLid.write(130);
    eye2RightLowLid.write(80);
    eye2LeftUpLid.write(80);
    eye2RightUpLid.write(130);
    delay(200);
    eye2LeftLowLid.write(60);
    eye2RightLowLid.write(120);
    eye2LeftUpLid.write(120);
    eye2RightUpLid.write(60);
    eye2BlinkThread->setInterval(random(3000,8000));
  }
  else
  {
    eye2LeftLowLid.write(60);
    eye2RightLowLid.write(120);
    eye2LeftUpLid.write(120);
    eye2RightUpLid.write(60);
  }
}

void eye3Blink(){
  if (focus==false){
    Serial.println("eye3 blink ");
    eye3LeftLowLid.write(130);
    eye3RightLowLid.write(80);
    eye3LeftUpLid.write(80);
    eye3RightUpLid.write(130);
    delay(200);
    eye3LeftLowLid.write(60);
    eye3RightLowLid.write(120);
    eye3LeftUpLid.write(120);
    eye3RightUpLid.write(60);
    eye3BlinkThread->setInterval(random(3000,8000));
  }
  else
  {
    eye3LeftLowLid.write(60);
    eye3RightLowLid.write(120);
    eye3LeftUpLid.write(120);
    eye3RightUpLid.write(60);
  }
}

void eye4Blink(){
  if (focus==false){
    Serial.println("eye4 blink ");
    eye4LeftLowLid.write(130);
    eye4RightLowLid.write(80);
    eye4LeftUpLid.write(80);
    eye4RightUpLid.write(130);
    delay(200);
    eye4LeftLowLid.write(60);
    eye4RightLowLid.write(120);
    eye4LeftUpLid.write(120);
    eye4RightUpLid.write(60);
    eye4BlinkThread->setInterval(random(3000,8000));
  }
  else
  {
    eye4LeftLowLid.write(60);
    eye4RightLowLid.write(120);
    eye4LeftUpLid.write(120);
    eye4RightUpLid.write(60);
  }
}

void eye5Blink(){
  if (focus==false){
    Serial.println("eye5 blink ");
    eye5LeftLowLid.write(130);
    eye5RightLowLid.write(80);
    eye5LeftUpLid.write(80);
    eye5RightUpLid.write(130);
    delay(200);
    eye5LeftLowLid.write(60);
    eye5RightLowLid.write(120);
    eye5LeftUpLid.write(120);
    eye5RightUpLid.write(60);
    eye5BlinkThread->setInterval(random(3000,8000));
  }
  else
  {
    eye5LeftLowLid.write(60);
    eye5RightLowLid.write(120);
    eye5LeftUpLid.write(120);
    eye5RightUpLid.write(60);
  }
}

void eye6Blink(){
  if (focus==false){
    Serial.println("eye6 blink ");
    eye6LeftLowLid.write(130);
    eye6RightLowLid.write(80);
    eye6LeftUpLid.write(80);
    eye6RightUpLid.write(130);
    delay(200);
    eye6LeftLowLid.write(60);
    eye6RightLowLid.write(120);
    eye6LeftUpLid.write(120);
    eye6RightUpLid.write(60);
    eye6BlinkThread->setInterval(random(3000,8000));
  }
  else
  {
    eye6LeftLowLid.write(60);
    eye6RightLowLid.write(120);
    eye6LeftUpLid.write(120);
    eye6RightUpLid.write(60);
  }
}

void eye7Blink(){
  if (focus==false){
    Serial.println("eye7 blink ");
    eye7LeftLowLid.write(130);
    eye7RightLowLid.write(80);
    eye7LeftUpLid.write(80);
    eye7RightUpLid.write(130);
    delay(200);
    eye7LeftLowLid.write(60);
    eye7RightLowLid.write(120);
    eye7LeftUpLid.write(120);
    eye7RightUpLid.write(60);
    eye7BlinkThread->setInterval(random(3000,8000));
  }
  else
  {
    eye7LeftLowLid.write(60);
    eye7RightLowLid.write(120);
    eye7LeftUpLid.write(120);
    eye7RightUpLid.write(60);
  }
}

void personSensorCallback(){
  person_sensor_results_t results = {};
  if (!person_sensor_read(&results)) {
    Serial.println("No person sensor results found on the i2c bus");
    return;
  }

  if (results.num_faces > 0 ){
    const person_sensor_face_t* face = &results.faces[0];
    if (face->box_confidence > 80){
//      Serial.println("Face found");
      focus = true;
      faceX=map((face->box_right - 20), 50, 205, 120, 60);
      faceY=map((face->box_bottom - 40), 50, 205, 100, 40);
      Serial.print("X,Y : ");
      Serial.print(faceX);   
      Serial.print(",");   
      Serial.println(faceY);
    }
  }
  else
  {
//    Serial.println("No Face found");
    focus = false;
  }
}
