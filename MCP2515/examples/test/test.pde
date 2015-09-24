#include <SPI.h>
#include <MCP2515.h>

#define SERIAL_DEBUG

void setup()
{
#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  Serial.println("ECU Reader");
#endif

  // Enable Interrupts
  sei();

  // Initialize the MCP2515
  if (!MCP2515.init(SPEED_500_KBPS))
  {
#ifdef SERIAL_DEBUG
    Serial.println("Failure:MCP2515 Initialization");
#endif    
  }
  else
  {
#ifdef SERIAL_DEBUG
    Serial.println("MCP2515 is Active");
#endif    
  }

#ifdef SERIAL_DEBUG
    Serial.println("Create message");
#endif    

  tCANMsg message;
	
  // Test message
  message.id = 0x123;
  message.header.rtr = 0;
  message.header.length = 2;
  message.data[0] = 0xab;
  message.data[1] = 0xcd;
	
  MCP2515.print_can_message(&message);
	
#ifdef SERIAL_DEBUG
  Serial.println("Switch to the loopback mode");
#endif    
  MCP2515.bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), (1<<REQOP1));
	
  // Send a message
  if (MCP2515.send_message(&message))
  {
#ifdef SERIAL_DEBUG
    Serial.println("Message was written in the buffer");
#endif
  }
  else
  {
#ifdef SERIAL_DEBUG
    Serial.println("Error: could not send the message");
#endif
  }
	
  // Wait a while
  delay(10);
	
  if (MCP2515.check_message())
  {
#ifdef SERIAL_DEBUG
    Serial.println("Message received");
#endif
		
    // read the message from the buffers
    if (MCP2515.get_message(&message))
    {
      MCP2515.print_can_message(&message);
    }
    else
    {
#ifdef SERIAL_DEBUG
      Serial.println("Error: could not read the message");
#endif
    }
  }
  else
  {
#ifdef SERIAL_DEBUG
    Serial.println("Error: No message received");
#endif
  }

#ifdef SERIAL_DEBUG
  Serial.println("back to normal mode");
#endif
  MCP2515.bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  
#ifdef SERIAL_DEBUG
  Serial.println("// From here we are again connected to the CAN bus");
#endif
	
  // Attempts to send the message again, this time via CAN
  if (MCP2515.send_message(&message))
  {
#ifdef SERIAL_DEBUG
    Serial.println("Message was written in the buffer");
#endif
  }
  else 
  {
#ifdef SERIAL_DEBUG
    Serial.println("Error: could not send the message");
#endif
  }
}

void loop()
{
#ifdef SERIAL_DEBUG
  Serial.println("Wait for the reception of messages");
#endif
  // wait until we receive a message
  if (MCP2515.check_message())
  {
#ifdef SERIAL_DEBUG
    Serial.println("Message received!");
#endif
			
    tCANMsg message;
    
    // Reading the message from the buffers of the MCP2515
    if (MCP2515.get_message(&message))
    {
      MCP2515.print_can_message(&message);
    }
    else
    {
#ifdef SERIAL_DEBUG
      Serial.println("Cannot read the message");
#endif
    }
  }
  
  delay(1000);
}
