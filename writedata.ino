
typedef union {
  float val_;
  unsigned long value;
  byte bytes[4];
} LongBytes;


long oldPosition  = -999;
LongBytes force, emg_chnl_1,emg_chnl_2, encoder_,timeVal;


void blink(){
  offset = scale.read_average();
}
void count(){
  state = !state;

} 

void write_data() {
  count();
  digitalWrite(ledPin, state);
//  Serial.println(state);

  byte header[] = {0xFF, 0xFF, 0x00};
  byte chkSum = 0xFE;
  byte _temp;
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
  }
  encoder_.value = 50;
  load_cell();
  emg_chnl_1.val_ = analogRead(emg1_);
  emg_chnl_2.val_ = analogRead(emg2_);
  timeVal.value = millis();
  // Send header

  header[2] = 20 + 1;
  chkSum += header[2];

  Serial.write(header[0]);
  Serial.write(header[1]);
  Serial.write(header[2]);

  // print out the value you read:
  //  Serial.write(sensorValue);
  
  for (int i = 0; i < 4; i++) {
    Serial.write(force.bytes[i]);
    chkSum += force.bytes[i];
  }
  for (int i = 0; i < 4; i++) {
    Serial.write(emg_chnl_1.bytes[i]);
    chkSum += emg_chnl_1.bytes[i];
  }
  for (int i = 0; i < 4; i++) {
    Serial.write(emg_chnl_2.bytes[i]);
    chkSum += emg_chnl_2.bytes[i];
  }
    for (int i = 0; i < 4; i++) {
  Serial.write(encoder_.bytes[i]);
  chkSum += encoder_.bytes[i];
  }
    
  for (int i = 0; i < 4; i++) {
    Serial.write(timeVal.bytes[i]);
    chkSum += timeVal.bytes[i];
  }
  Serial.write(chkSum);
    delayMicroseconds(960);     // delay in between reads for stability

}

void load_cell(){
  if(scale.is_ready()){
  reading = (scale.get_units() - offset);
  weight = (reading / 44.473);
//  Serial.println(weight / 1000);

//  force.val_ = (weight / 1000);
force.val_ = 25;
  }
  else{
//    force.val_=(weight / 1000);
  force.val_ = 35;
  }
  
}
