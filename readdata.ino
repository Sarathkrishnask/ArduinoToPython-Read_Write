
void read_data()
{
  display.clearDisplay();

  if (Serial.available() >= 5) { // Ensure that at least 5 bytes are available for processing
    byte header1 = Serial.read();
    byte header2 = Serial.read();
    byte payloadSize = Serial.read()-1;
    byte payload[payloadSize];
    
    // Read the payload bytes
    for (int i = 0; i < payloadSize; i++) {
      payload[i] = Serial.read();
    }

    byte checksum = Serial.read();

    // Verify checksum
    byte calculatedChecksum = 0;
    calculatedChecksum += header1;
    calculatedChecksum += header2;
    calculatedChecksum += payloadSize+1;
    for (int i = 0; i < payloadSize; i++) {
      calculatedChecksum += payload[i];
    }
    calculatedChecksum %= 256;


    if (calculatedChecksum == checksum) {
      // Convert payload bytes to float
      float received_float;
      memcpy(&received_float, payload, sizeof(float));

      // Process the received float value
      Serial.print("Received float value: ");
      Serial.println(received_float);
       display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10,20);
      display.println(received_float);
      display.display();
      
      // Checksum is valid, process the payload
      // Here you can do whatever you need with the received payload
      
    } else {
      // Checksum error
      Serial.println("Checksum error");
    }
  }
  }
