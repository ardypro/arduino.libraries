/*
 * Copyright (c) 2011 by Mark Winney
 * MP2515 library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "../SPI/SPI.h"
#include "Arduino.h"
#include "MCP2515.h"

MCP2515Class MCP2515;

/**********************************************************************/
uint8_t MCP2515Class::init(uint8_t speed, uint8_t cs_pin, uint8_t int_pin)
{
#ifdef SERIAL_DEBUG  
  char out[132];
  sprintf(out, "MCP2515Class::init:Enter, speed=0x%X", speed);
  Serial.println(out);
  sprintf(out, "MCP2515Class::init:cs_pin=0x%X", cs_pin);
  Serial.println(out);
  sprintf(out, "MCP2515Class::init:int_pin=0x%X", int_pin);
  Serial.println(out);
#endif
  // Store the Chip select pin and Interrupt pin.
  _chipSelectPin = cs_pin;
  _interruptPin = int_pin;

  // start the SPI library:
  SPI.begin();

  SET_OUTPUT(_chipSelectPin);
  SET(_chipSelectPin);
	
  SET_INPUT(_interruptPin);
  SET(_interruptPin);
	
  // reset MCP2515 by software reset.
  // After this it is in configuration mode.
  RESET(_chipSelectPin);
  SPI.transfer(SPI_RESET);
  SET(_chipSelectPin);
	
  // wait a little bit until the MCP2515 has restarted
  delay(10);
	
  if (speed == SPEED_125_KBPS)
  {
    write_register(CNF3, (1<<PHSEG21));

    // 125 kbps at 20 Mhz
    write_register(CNF2, (1<<BTLMODE)|(1<<PHSEG12));
  }
  else if(speed == SPEED_250_KBPS)
  {
    write_register(CNF3, (1<<PHSEG21));

    // 250 kbps at 20 Mhz
    write_register(CNF2, (1<<BTLMODE)|(1<<PHSEG12));
  }
  else if(speed == SPEED_500_KBPS)
  {
    write_register(CNF3, (1<<PHSEG21));

    // 500 kbps at 20 Mhz
    write_register(CNF2, (1<<BTLMODE)|(1<<PHSEG12));
  }

  write_register(CNF1, speed);

  // activate interrupts
  write_register(CANINTE, (1<<RX1IE)|(1<<RX0IE));

  // test if we could read back the value => is the chip accessible?
  if (read_register(CNF1) != speed)
  {
#ifdef SERIAL_DEBUG
    Serial.println("MCP2515Class::init:Exit false");
#endif
    return false;
  }
	
  // deactivate the RXnBF Pins (High Impedance State)
  write_register(BFPCTRL, 0);
	
  // set TXnRTS as inputs
  write_register(TXRTSCTRL, 0);

  // reset device to normal mode
  // Not sure why but when I set mode back to normal then
  // back to config, the filters do not work.  No idea why yet.
//  setMode(NORMAL_MODE);

  // Clear interrupt bits ???????
  bit_modify(CANINTF, (1<<RX0IF)|(1<<RX1IF), 0);


#ifdef SERIAL_DEBUG
  Serial.println("init:Exit true");
#endif
  return true;
}

/**********************************************************************/
void MCP2515Class::setMode(uint8_t mode)
{
  // Set device to Config mode
  write_register(CANCTRL, mode);

  uint8_t status = 0;

  // Now wait till into mode
  do
  {
    status = read_register(CANSTAT);
#ifdef SERIAL_DEBUG
    char out[132];
    sprintf(out, "setConfigMode:status=0x%X", status);
    Serial.println(out);
#endif
  }while(!((status & mode) == mode));
}

/**********************************************************************/
uint8_t MCP2515Class::setMaskAndFilter( uint16_t mask, uint16_t filter )
{
#ifdef SERIAL_DEBUG  
  char out[132];
  sprintf(out, "MCP2515Class::setMaskAndFilter:Enter mask=0x%X, filter=0x%X", mask, filter);
  Serial.println(out);
#endif

  // reset device to config mode
  setMode(CONFIG_MODE);

  // Check if filter and mask set and if not then receive any messages. 	
  if (filter == 0 && mask == 0)
  {
    // turn off filters => receive any message
    write_register(RXB0CTRL, (1<<RXM1)|(1<<RXM0));
    write_register(RXB1CTRL, (1<<RXM1)|(1<<RXM0));
  }
  else
  {
    // Only allow standard messages that match filters
    // basic-can  filter and mask for buffer 0
    write_register(RXM0SIDH, (uint8_t)(mask >> 3));
    write_register(RXM0SIDL, (uint8_t)(mask << 5));
    write_register(RXF0SIDH, (uint8_t)(filter >> 3)); 
    write_register(RXF0SIDL, (uint8_t)(filter << 5));
    // receive only matching standart-can frames hitting filter 0
    // and allow overflow to RXB1
    write_register(RXB0CTRL, (0<<RXM1)|(1<<RXM0)|(1<<BUKT));

    // Make sure mask was set correct
    uint16_t msidh = read_register(RXM0SIDH) << 3;
    uint16_t msid = msidh | read_register(RXM0SIDL) >> 5;
    if (msid != mask)
    {
#ifdef SERIAL_DEBUG  
	Serial.println("MCP2515Class::setMaskAndFilter:exit false");
#endif
      return false; 
    }
    // Make sure filter was set correct
    uint16_t fsidh = read_register(RXF0SIDH) << 3;
    uint16_t fsid = fsidh | read_register(RXF0SIDL) >> 5;
    if (fsid != filter)
    {
#ifdef SERIAL_DEBUG  
	Serial.println("MCP2515Class::setMaskAndFilter:exit false");
#endif
      return false; 
    }

    // basic-can  filter and mask for buffer 1
    write_register(RXM1SIDH, (uint8_t)(mask >> 3));
    write_register(RXM1SIDL, (uint8_t)(mask << 5));
    write_register(RXF2SIDH, (uint8_t)(filter >> 3)); 
    write_register(RXF2SIDL, (uint8_t)(filter << 5));
    // receive only matching standart-can frames hitting filter 2
    write_register(RXB1CTRL, (0<<RXM1)|(1<<RXM0)|(1<<FILHIT1));
  }

    // Make sure mask was set correct
    uint16_t msidh = read_register(RXM1SIDH) << 3;
    uint16_t msid = msidh | read_register(RXM1SIDL) >> 5;
    if (msid != mask)
    {
#ifdef SERIAL_DEBUG  
	Serial.println("MCP2515Class::setMaskAndFilter:exit false");
#endif
      return false; 
    }
    // Make sure filter was set correct
    uint16_t fsidh = read_register(RXF2SIDH) << 3;
    uint16_t fsid = fsidh | read_register(RXF2SIDL) >> 5;
    if (fsid != filter)
    {
#ifdef SERIAL_DEBUG  
	Serial.println("MCP2515Class::setMaskAndFilter:exit false");
#endif
      return false; 
    }

  _filter = filter;
  _mask = mask;

  // reset device to normal mode
  setMode(NORMAL_MODE);

#ifdef SERIAL_DEBUG  
  Serial.println("MCP2515Class::setFilterAndMask:Exit");
#endif

  return true;
}

/**********************************************************************/
void MCP2515Class::write_register( uint8_t adress, uint8_t data )
{
#ifdef SERIAL_DEBUG  
//  char out[132];
//  sprintf(out, "write_register:Enter adress=0x%X, data=0x%X", adress, data);
//  Serial.println(out);
#endif

  RESET(_chipSelectPin);
  SPI.transfer(SPI_WRITE);
  SPI.transfer(adress);
  SPI.transfer(data);
  SET(_chipSelectPin);

#ifdef SERIAL_DEBUG  
//  Serial.println("write_register:Exit");
#endif
}

/**********************************************************************/
uint8_t MCP2515Class::read_register(uint8_t adress)
{
#ifdef SERIAL_DEBUG  
//  char out[132];
//  sprintf(out, "read_register:Enter, adress=0x%X", adress);
//  Serial.println(out);
//  Serial.println("read_register:Enter");
#endif

  uint8_t data;
	
  RESET(_chipSelectPin);
  SPI.transfer(SPI_READ);
  SPI.transfer(adress);
  data = SPI.transfer(0xff);	
  SET(_chipSelectPin);
	
#ifdef SERIAL_DEBUG
//  sprintf(out, "read_register:Exit, data=0x%X", data);
//  Serial.println(out);
//  Serial.println("read_register:Exit");
#endif

  return data;
}

/**********************************************************************/
void MCP2515Class::bit_modify(uint8_t adress, uint8_t mask, uint8_t data)
{
#ifdef SERIAL_DEBUG  
//  char out[132];
//  sprintf(out, "MCP2515Class::bit_modify:Enter, adress=0x%X,mask=0x%X,data=0x%X", adress, mask, data);
//  Serial.println(out);
#endif

  RESET(_chipSelectPin);
  SPI.transfer(SPI_BIT_MODIFY);
  SPI.transfer(adress);
  SPI.transfer(mask);
  SPI.transfer(data);
  SET(_chipSelectPin);

#ifdef SERIAL_DEBUG
//  Serial.println("MCP2515Class::bit_modify:Exit");
#endif
}

/**********************************************************************/
uint8_t MCP2515Class::read_status(uint8_t type)
{
#ifdef SERIAL_DEBUG  
//  char out[132];
//  sprintf(out, "MCP2515Class::read_status:Enter, type=0x%X", type);
//  Serial.println(out);
//  Serial.println("MCP2515Class::read_status:Enter");
#endif

  uint8_t data;
	
  RESET(_chipSelectPin);
  SPI.transfer(type);
  data = SPI.transfer(0xff);
  SET(_chipSelectPin);
	
#ifdef SERIAL_DEBUG  
//  sprintf(out, "MCP2515Class::read_status:Exit, data=0x%X", data);
//  Serial.println(out);
//  Serial.println("MCP2515Class::read_status:Exit");
#endif

  return data;
}

/**********************************************************************/
uint8_t MCP2515Class::check_message(void)
{
#ifdef SERIAL_DEBUG  
//  Serial.println("MCP2515Class::check_message:Enter");
#endif

  uint8_t retVal = !IS_SET(_interruptPin);

#ifdef SERIAL_DEBUG  
//  Serial.println("MCP2515Class::check_message:Exit");
#endif

  return retVal;
}

/**********************************************************************/
uint8_t MCP2515Class::check_free_buffer(void)
{
#ifdef SERIAL_DEBUG  
//  Serial.println("MCP2515Class::check_free_buffer:Enter");
#endif
  uint8_t status = read_status(SPI_READ_STATUS);
	
  if ((status & 0x54) == 0x54)
  {
    // all buffers used
#ifdef SERIAL_DEBUG  
//  Serial.println("MCP2515Class::check_free_buffer:Exit false");
#endif
    return false;
  }
	
#ifdef SERIAL_DEBUG  
//  Serial.println("MCP2515Class::check_free_buffer:Exit true");
#endif
  return true;
}

/**********************************************************************/
uint8_t MCP2515Class::get_message(tCANMsg *message)
{
#ifdef SERIAL_DEBUG  
  Serial.println("MCP2515Class::get_message:Enter");
#endif

  static uint8_t last_buffer_read = 0;
  // read status
  uint8_t status = read_status(SPI_RX_STATUS);
  uint8_t addr;
  uint8_t t;
  
#ifdef SERIAL_DEBUG  
  char out[132];
  sprintf(out, "MCP2515Class::get_message:status=0x%X", status);
  Serial.println(out);
#endif

  // Check if buffer 0 has a value and buffer 1 has a value
  // and if the last read was from buffer 0, then read from 1 now.
  if (BIT_IS_SET(status,6) && BIT_IS_SET(status,7) && (last_buffer_read == 0))
  {
    // read message in buffer 1
    addr = SPI_READ_RX | 0x04;
    last_buffer_read = 1;
#ifdef SERIAL_DEBUG  
    Serial.println("MCP2515Class::get_message:message in buffer 1");
#endif
  }
  else if (BIT_IS_SET(status,6) && BIT_IS_SET(status,7) && (last_buffer_read == 1))
  {
    // Check if buffer 0 has a value and buffer 1 has a value
    // and if the last read was from buffer 1, then read from 0 now.
    // read message in buffer 0
    addr = SPI_READ_RX;
    last_buffer_read = 0;
#ifdef SERIAL_DEBUG  
    Serial.println("MCP2515Class::get_message:message in buffer 0");
#endif
  }
  else if (BIT_IS_SET(status,6))
  {
    // message in buffer 0
    addr = SPI_READ_RX;
    last_buffer_read = 0;
#ifdef SERIAL_DEBUG  
    Serial.println("MCP2515Class::get_message:message in buffer 0");
#endif
  }
  else if (BIT_IS_SET(status,7))
  {
    // message in buffer 1
    addr = SPI_READ_RX | 0x04;
    last_buffer_read = 1;
#ifdef SERIAL_DEBUG  
    Serial.println("MCP2515Class::get_message:message in buffer 1");
#endif
  }
  else
  {
    // Error: no message available
#ifdef SERIAL_DEBUG  
    Serial.println("MCP2515Class::get_message:Error: no message available");
#endif
    return 0;
  }

#ifdef SERIAL_DEBUG  
//  sprintf(out, "MCP2515Class::get_message:addr=0x%X", addr);
//  Serial.println(out);
#endif

  RESET(_chipSelectPin);
  SPI.transfer(addr);
	
  // read id
  uint8_t dataByte1 = SPI.transfer(0xff);
  message->id = (uint16_t)dataByte1 << 3;

  uint8_t dataByte2 = SPI.transfer(0xff);
  message->id |= dataByte2 >> 5;
	
  uint8_t dataByte3 = SPI.transfer(0xff);
  uint8_t dataByte4 = SPI.transfer(0xff);
	
  // read DLC
  uint8_t length = SPI.transfer(0xff) & 0x0f;
	
#ifdef SERIAL_DEBUG  
//  char out[132];
//  sprintf(out, "MCP2515Class::get_message:length=0x%X", length);
//  Serial.println(out);
#endif

#ifdef SERIAL_DEBUG  
//  sprintf(out, "MCP2515Class::get_message:bytes 1-5=0x%X,0x%X,0x%X,0x%X,0x%X", dataByte1, dataByte2, dataByte3, dataByte4, dataByte5);
//  Serial.println(out);
#endif

  message->header.length = length;
  message->header.rtr = (BIT_IS_SET(status, 3)) ? 1 : 0;
	
  // read data
  for (t = 0; t < length; t++)
  {
    message->data[t] = SPI.transfer(0xff);
  }
  SET(_chipSelectPin);
	
  // clear interrupt flag
  if (last_buffer_read == 0)
  {
    bit_modify(CANINTF, (1<<RX0IF), 0);
#ifdef SERIAL_DEBUG  
  Serial.println("MCP2515Class::get_message:Clear Buffer 0");
#endif
  }
  else
  {
    bit_modify(CANINTF, (1<<RX1IF), 0);
#ifdef SERIAL_DEBUG  
  Serial.println("MCP2515Class::get_message:Clear Buffer 1");
#endif
  }

#ifdef SERIAL_DEBUG  
//  sprintf(out, "MCP2515Class::get_message:Exit status=0x%X", ((status & 0x07) + 1));

//  print_can_message(message);

//  Serial.println(out);
#endif

  return (status & 0x07) + 1;
}

/**********************************************************************/
uint8_t MCP2515Class::send_message(tCANMsg *message)
{
#ifdef SERIAL_DEBUG  
  Serial.println("MCP2515Class::send_message:Enter");
//  print_can_message(message);
#endif
  uint8_t status = read_status(SPI_READ_STATUS);
	
  /* Statusbyte:
   *
   * Bit	Function
   *  2	TXB0CNTRL.TXREQ
   *  4	TXB1CNTRL.TXREQ
   *  6	TXB2CNTRL.TXREQ
   */
  uint8_t address;
  uint8_t t;

  if (BIT_IS_CLEAR(status, 2))
  {
    address = 0x00;
  }
  else if (BIT_IS_CLEAR(status, 4))
  {
    address = 0x02;
  } 
  else if (BIT_IS_CLEAR(status, 6))
  {
    address = 0x04;
  }
  else
  {
    // all buffer used => could not send message
#ifdef SERIAL_DEBUG  
  Serial.println("MCP2515Class::send_message:all buffer used => could not send message");
  print_can_message(message);
#endif
    return 0;
  }
	
#ifdef SERIAL_DEBUG  
//  char out[132];
//  sprintf(out, "MCP2515Class::send_message:address=0x%X", address);
//  Serial.println(out);
#endif

  RESET(_chipSelectPin);
  SPI.transfer(SPI_WRITE_TX | address);
	
  SPI.transfer(message->id >> 3);
  SPI.transfer(message->id << 5);
	
  SPI.transfer(0);
  SPI.transfer(0);
	
  uint8_t length = message->header.length & 0x0f;
	
#ifdef SERIAL_DEBUG  
  char out[132];
  sprintf(out, "MCP2515Class::send_message:length=0x%X", length);
  Serial.println(out);
#endif

  if (message->header.rtr)
  {
    // a rtr-frame has a length, but contains no data
    SPI.transfer((1<<RTR) | length);
  }
  else
  {
    // set message length
    SPI.transfer(length);
		
    // data
    for (t = 0; t < length; t++)
    {
      SPI.transfer(message->data[t]);
    }
  }
  SET(_chipSelectPin);
	
  delay(10);
	
  // send message
  RESET(_chipSelectPin);
  address = (address == 0) ? 1 : address;
  SPI.transfer(SPI_RTS | address);
  SET(_chipSelectPin);

  // See if any TX errors, anything other than 0 is problem
  uint8_t txErr = MCP2515.read_register(TEC);

#ifdef SERIAL_DEBUG  
  sprintf(out, "MCP2515Class::send_message:txErr=0x%X,exit=0x%X", txErr, (txErr == 0));
  Serial.println(out);
#endif

  return (txErr == 0);
}

#ifdef SERIAL_DEBUG
/**********************************************************************/
void MCP2515Class::print_can_message(tCANMsg *message)
{
  uint8_t length = message->header.length;
	
  char out[132];
  sprintf(out, "print_can_message:Enter");
  Serial.println(out);
  sprintf(out, "id:     0x%3x", message->id);
  Serial.println(out);
  sprintf(out, "length: %d", length);
  Serial.println(out);
  sprintf(out, "rtr:    %d", message->header.rtr);
  Serial.println(out);
	
  if (!message->header.rtr)
  {
    Serial.println("data:  ");
		
    for (uint8_t i = 0; i < length; i++)
    {
      sprintf(out, "0x%02x ", message->data[i]);
      Serial.print(out);
    }
    Serial.println("");
  }
}
#endif

