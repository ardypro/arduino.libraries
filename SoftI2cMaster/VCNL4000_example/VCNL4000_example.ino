#include <SoftI2cMaster.h>

word ch1 = 0x00;
word ch2 = 0x00;
word ch3 = 0x00;
word ch4 = 0x00;
uint8_t data2 = 0x00;
uint8_t data3 = 0x00;
uint8_t addr = 0;

SoftI2cMaster i2c;

#define SCL_PIN1 8        /*Different SCL pins can be assigned to each I2C bus*/
#define SCL_PIN2 8
#define SCL_PIN3 8
#define SCL_PIN4 8
#define SDA_PIN1 9
#define SDA_PIN2 10
#define SDA_PIN3 11
#define SDA_PIN4 12

void i2c_transaction(uint8_t n, uint8_t reg, uint8_t dat){
    switch (n){
        case 1: 
        
        TwoWireBase *bus1;
        bus1 = &i2c;
        i2c.init(SCL_PIN1, SDA_PIN1);
        
        bus1->start(0x26);
        bus1->write(reg);
        bus1->write(dat);
        bus1->stop();
        
        bus1->start(0x26);
        bus1->write(0x87);/////////////////////Read Proximity
        bus1->restart(0x27);
        data2 = bus1->read(0);
        data3 = bus1->read(1);
        bus1->stop();

        ch1 = word(data2,data3);
        break;
        
        case 2:
        
        TwoWireBase *bus2;
        bus2 = &i2c;
        i2c.init(SCL_PIN2, SDA_PIN2);
        
        bus2->start(0x26);
        bus2->write(reg);
        bus2->write(dat);
        bus2->stop();
        
        bus2->start(0x26);
        bus2->write(0x87);/////////////////////Read Proximity
        bus2->restart(0x27);
       
        data2 = bus2->read(0);
        data3 = bus2->read(1);
        bus2->stop();
        
        ch2 = word(data2,data3);
        break;
        
        case 3:
        
        TwoWireBase *bus3;
        bus3 = &i2c;
        i2c.init(SCL_PIN3, SDA_PIN3);
        
        bus3->start(0x26);
        bus3->write(reg);
        bus3->write(dat);
        bus3->stop();
        
        bus3->start(0x26);
        bus3->write(0x87);/////////////////////Read Proximity
        bus3->restart(0x27);
        data2 = bus3->read(0);
        data3 = bus3->read(1);
        bus3->stop();
        
        ch3 = word(data2,data3);
        break;
        
        case 4:
        
        TwoWireBase *bus4;
        bus4 = &i2c;
        i2c.init(SCL_PIN4, SDA_PIN4);
        
        bus4->start(0x26);
        bus4->write(reg);
        bus4->write(dat);
        bus4->stop();
        
        bus4->start(0x26);
        bus4->write(0x87);/////////////////////Read Proximity
        bus4->restart(0x27);
       
        data2 = bus4->read(0);
        data3 = bus4->read(1);
        bus4->stop();
        
        ch4 = word(data2,data3);
        break;
    }
}

void setup(void)
{
    Serial.begin(9600);
    
    i2c_transaction(1,0x83,20);
    i2c_transaction(1,0x89,3);
    
    i2c_transaction(2,0x83,20);
    i2c_transaction(2,0x89,3);
    
    i2c_transaction(3,0x83,20);
    i2c_transaction(3,0x89,3);
    
    i2c_transaction(4,0x83,20);
    i2c_transaction(4,0x89,3);
}

void loop(void)
{
    i2c_transaction(1,0x80,8);
    i2c_transaction(2,0x80,8);
    i2c_transaction(3,0x80,8);
    i2c_transaction(4,0x80,8);
    
    Serial.print(" (1). ");
    Serial.print(ch1);
    Serial.print(" (2). ");
    Serial.print(ch2);
    Serial.print(" (3). ");
    Serial.print(ch3);
    Serial.print(" (4). ");
    Serial.println(ch4);
}
