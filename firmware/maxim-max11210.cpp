#include "maxim-max11210.h"

// Constructor
Maxim::Max11210::Max11210() {

}

void Maxim::Max11210::begin() {
  // The firmware (spark_wiring.cpp) does not allow monitoring of MISO using digitalRead().
  // The MISO if therefore also connected to a digital input of the Spark Core.
  pinMode(D0, INPUT);
    
  // The Spark Core system clock is 72 MHz, while the MAX11210 SPI runs at 5 MHz maximum.
  // Setting the SPI clock divider to 16x results in a SCLK of 4.5 MHz.
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE0);

  // Set default configuration of the ADC in register CTRL1
  //setLineFreq(MAX11210_50HZ);
  //setInputRange(MAX11210_UNIPOLAR);
  //setClockSource(MAX11210_INTCLK);
  //setEnableRefBuf(false);
  //setEnableSigBuf(false);
  //setFormat(MAX11210_2SCOMPL)
  //setConvMode(MAX11210_CONT);
  _writeReg8(CTRL1, 0b11000000);
  _sCycle = false;

  // Set default configuration of the ADC in register CTRL2
  //pinModeGpio(0, OUTPUT);
  //pinModeGpio(1, OUTPUT);
  //pinModeGpio(2, OUTPUT);
  //pinModeGpio(3, OUTPUT);
  //digitalWriteGpio(0, HIGH);
  //digitalWriteGpio(1, LOW);
  //digitalWriteGpio(2, HIGH);
  //digitalWriteGpio(3, LOW);
  _writeReg8(CTRL2, 0b11110101);
  
  // Set default configuration of the ADC in register CTRL3
  //setGain(MAX11210_GAIN1);
  //setDisableSysGain(true);
  //setDisableSysOffset(true);
  //setDisableSelfCalGain(true);
  //setDisableSelfCalOffset(true);
  _writeReg8(CTRL3, 0b00011110);
  selfCal();

  // Initiate conversions
  _rate = MAX11210_RATE10;
  setRate(_rate);
}

void Maxim::Max11210::end() {
  _sendCmd(IMPD);
  SPI.end();
}

unsigned char Maxim::Max11210::_readReg8(unsigned char addr) {
  unsigned char mesg = ((addr & 0x0F) << 1) | START | MODE | READ;
  digitalWrite(SS, LOW);
  SPI.transfer(mesg);
  unsigned char resp = SPI.transfer(0x00);
  digitalWrite(SS, HIGH);
  return resp;    
}

long Maxim::Max11210::_readReg24(unsigned char addr) {
  unsigned char mesg = ((addr & 0x0F) << 1) | START | MODE | READ;
  digitalWrite(SS, LOW);
  SPI.transfer(mesg);
  long resp = SPI.transfer(0x00);
  resp = resp << 8;
  resp |= SPI.transfer(0x00);
  resp = resp << 8;
  resp |= SPI.transfer(0x00);
  digitalWrite(SS, HIGH);
  return resp;
}

void Maxim::Max11210::_writeReg8(unsigned char addr, unsigned char data) {
  unsigned char mesg = ((addr & 0x0F) << 1) | START | MODE;
  digitalWrite(SS, LOW);
  SPI.transfer(mesg);
  SPI.transfer(data);
  digitalWrite(SS, HIGH);
}

void Maxim::Max11210::_writeReg24(unsigned char addr, long data) {
  unsigned char mesg = ((addr & 0x0F) << 1) | START | MODE;
  digitalWrite(SS, LOW);
  SPI.transfer(mesg);
  SPI.transfer((data >> 16) & 0xFF);
  SPI.transfer((data >> 8) & 0xFF);
  SPI.transfer(data & 0xFF);
  digitalWrite(SS, HIGH);
}

void Maxim::Max11210::_sendCmd(unsigned char data) {
  unsigned char mesg = (data & 0x3F) | START;
  digitalWrite(SS, LOW);
  SPI.transfer(mesg);
  digitalWrite(SS, HIGH);
}

long Maxim::Max11210::read() {
  digitalWrite(SS, LOW);
  do {
    delayMicroseconds(1);  
  } while (digitalRead(D0) == LOW);
  unsigned char addr = DATA;
  long resp = _readReg24(addr);
  return resp;
}

void Maxim::Max11210::selfCal() {
  _sendCmd(CAL0);
  delay(220); // A self-calibration requires 200 ms to complete
}

void Maxim::Max11210::sysOffsetCal() {
  _sendCmd(CAL1);
  delay(120); // A zero-scale calibration requires 100 ms to complete
}

void Maxim::Max11210::sysGainCal() {
  _sendCmd(CAL0 | CAL1);
  delay(120); // A full-scale calibration requires 100 ms to complete
}

void Maxim::Max11210::setRate(unsigned char rate) {
  _rate = rate;
  _sendCmd(rate & 0x07);
}

bool Maxim::Max11210::getSysGainOverRange() {
  unsigned char addr = STAT1;
  unsigned char stat = _readReg8(addr);
  return ((stat & SYSOR) > 0x00);
}

unsigned char Maxim::Max11210::getRate() {
  unsigned char addr = STAT1;
  unsigned char stat = _readReg8(addr);
  unsigned char rate = ((stat & 0x70) >> 4);
  return rate;
}

bool Maxim::Max11210::getOverRange() {
  unsigned char addr = STAT1;
  unsigned char stat = _readReg8(addr);
  return ((stat & OR) > 0x00);
}

bool Maxim::Max11210::getUnderRange() {
  unsigned char addr = STAT1;
  unsigned char stat = _readReg8(addr);
  return ((stat & UR) > 0x00);
}

bool Maxim::Max11210::getMeasStat() {
  unsigned char addr = STAT1;
  unsigned char stat = _readReg8(addr);
  return ((stat & MSTAT) > 0x00);
}

bool Maxim::Max11210::getReady() {
  unsigned char addr = STAT1;
  unsigned char stat = _readReg8(addr);
  return ((stat & RDY) > 0x00);
}

void Maxim::Max11210::setLineFreq(unsigned char value) {
  unsigned char addr = CTRL1;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (value == MAX11210_50HZ) {
    data = (stat | LINEF);
    _writeReg8(addr, data);
  }  
  else if (value == MAX11210_60HZ) {
    data = (stat & ~LINEF);
    _writeReg8(addr, data);
  }
}

void Maxim::Max11210::setInputRange(unsigned char value) {
  unsigned char addr = CTRL1;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (value == MAX11210_UNIPOLAR) {
    data = (stat | U);
    _writeReg8(addr, data);
  }  
  else if (value == MAX11210_BIPOLAR) {
    data = (stat & ~U);
    _writeReg8(addr, data);
  }
}

void Maxim::Max11210::setClockSource(unsigned char value) {
  unsigned char addr = CTRL1;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (value == MAX11210_EXTCLK) {
    data = (stat | EXTCLK);
    _writeReg8(addr, data);
  }  
  else if (value == MAX11210_INTCLK) {
    data = (stat & ~EXTCLK);
    _writeReg8(addr, data);
  }  
}

void Maxim::Max11210::setEnableRefBuf(bool state) {
  unsigned char addr = CTRL1;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (state)
    data = (stat | REFBUF);
  else
    data = (stat & ~REFBUF);
  _writeReg8(addr, data);
}

void Maxim::Max11210::setEnableSigBuf(bool state) {
  unsigned char addr = CTRL1;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (state)
    data = (stat | SIGBUF);
  else
    data = (stat & ~SIGBUF);
  _writeReg8(addr, data);
}

void Maxim::Max11210::setFormat(unsigned char value) {
  unsigned char addr = CTRL1;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (value == MAX11210_OFFSET) {
    data = (stat | FORMAT);
    _writeReg8(addr, data);
  }  
  else if (value == MAX11210_2SCOMPL) {
    data = (stat & ~FORMAT);
    _writeReg8(addr, data);
  }
}

void Maxim::Max11210::setConvMode(unsigned char value) {
  unsigned char addr = CTRL1;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (value == MAX11210_SINGLE) {
    _sCycle = true;
    data = (stat | SCYCLE);
    _writeReg8(addr, data);
  }  
  else if (value == MAX11210_CONT) {
    _sCycle = false;
    data = (stat & ~SCYCLE);
    _writeReg8(addr, data);
  }
}

unsigned char Maxim::Max11210::getLineFreq() {
  unsigned char addr = CTRL1;
  unsigned char stat = _readReg8(addr);
  if ((stat & LINEF) > 0x00)
    return MAX11210_60HZ;
  else
    return MAX11210_50HZ;
}

unsigned char Maxim::Max11210::getInputRange() {
  unsigned char addr = CTRL1;
  unsigned char stat = _readReg8(addr);
  if ((stat & U) > 0x00)
    return MAX11210_UNIPOLAR;
  else
    return MAX11210_BIPOLAR;
}

unsigned char Maxim::Max11210::getClockSource() {
  unsigned char addr = CTRL1;
  unsigned char stat = _readReg8(addr);
  if ((stat & EXTCLK) > 0x00)
    return MAX11210_EXTCLK;
  else
    return MAX11210_INTCLK;
}

bool Maxim::Max11210::getEnableRefBuf() {
  unsigned char addr = CTRL1;
  unsigned char stat = _readReg8(addr);
  return ((stat & REFBUF) > 0x00);
}

bool Maxim::Max11210::getEnableSigBuf() {
  unsigned char addr = CTRL1;
  unsigned char stat = _readReg8(addr);
  return ((stat & SIGBUF) > 0x00);
}

unsigned char Maxim::Max11210::getFormat() {
  unsigned char addr = CTRL1;
  unsigned char stat = _readReg8(addr);
  if ((stat & FORMAT) > 0x00)
    return MAX11210_OFFSET;
  else
    return MAX11210_2SCOMPL;
}

unsigned char Maxim::Max11210::getConvMode() {
  unsigned char addr = CTRL1;
  unsigned char stat = _readReg8(addr);
  if ((stat & SCYCLE) > 0x00)
    return MAX11210_SINGLE;
  else
    return MAX11210_CONT;
}

void Maxim::Max11210::pinModeGpio(int pin, unsigned char mode) {
  if (pin < 0 || pin > 3)
    return;
  unsigned char addr = CTRL2;
  unsigned char stat = _readReg8(addr);
  unsigned char data = 0x00;
  if (mode == OUTPUT)
    data = stat | (0x10 << pin);      
  else if (mode == INPUT)
    data = stat ^ (~(0x10 << pin));
  data &= 0xF0;
  _writeReg8(addr, data);
}

void Maxim::Max11210::digitalWriteGpio(int pin, bool value) {
  if (pin < 0 || pin > 3)
    return;
  unsigned char addr = CTRL2;
  unsigned char stat = _readReg8(addr);
  unsigned char data = 0x00;
  if (value)
    data = stat | (0x01 << pin);  
  else
    data = stat ^ (~(0x01 << pin));
  data &= 0x0F;  
  _writeReg8(addr, data);
}

bool Maxim::Max11210::digitalReadGpio(int pin) {
  if (pin < 0 || pin > 3)
    return false;    
  unsigned char addr = CTRL2;
  unsigned char stat = _readReg8(addr) & (0x01 << pin);
  return (stat > 0x00);
}

void Maxim::Max11210::setGain(unsigned char gain) {
  if (gain > 0x04)
    gain = 0x04;
  unsigned char addr = CTRL3;
  unsigned char data = (gain & 0x07) << 5;
  unsigned char stat = _readReg8(addr);
  data |= (stat & 0x1F);
  _writeReg8(addr, data);
}

void Maxim::Max11210::setDisableSysGain(bool state) {
  unsigned char addr = CTRL3;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (state)
    data = (stat | NOSYSG);
  else
    data = (stat & ~NOSYSG);
  _writeReg8(addr, data);
}

void Maxim::Max11210::setDisableSysOffset(bool state) {
  unsigned char addr = CTRL3;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (state)
    data = (stat | NOSYSO);
  else
    data = (stat & ~NOSYSO);
  _writeReg8(addr, data);    
}

void Maxim::Max11210::setDisableSelfCalGain(bool state) {
  unsigned char addr = CTRL3;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (state)
    data = (stat | NOSCG);
  else
    data = (stat & ~NOSCG);
  _writeReg8(addr, data);    
}

void Maxim::Max11210::setDisableSelfCalOffset(bool state) {
  unsigned char addr = CTRL3;
  unsigned char data = 0x00;
  unsigned char stat = _readReg8(addr);
  if (state)
    data = (stat | NOSCO);
  else
    data = (stat & ~NOSCO);
  _writeReg8(addr, data);
}

unsigned char Maxim::Max11210::getGain() {
  unsigned char addr = CTRL3;
  unsigned char stat = _readReg8(addr);
  return ((stat & 0xE0) >> 5);
}
  
bool Maxim::Max11210::getDisableSysGain() {
  unsigned char addr = CTRL3;
  unsigned char stat = _readReg8(addr);
  return ((stat & NOSYSG) > 0x00);
}
  
bool Maxim::Max11210::getDisableSysOffset() {
  unsigned char addr = CTRL3;
  unsigned char stat = _readReg8(addr);
  return ((stat & NOSYSO) > 0x00);
}
  
bool Maxim::Max11210::getDisableSelfCalGain() {
  unsigned char addr = CTRL3;
  unsigned char stat = _readReg8(addr);
  return ((stat & NOSCG) > 0x00);   
}
  
bool Maxim::Max11210::getDisableSelfCalOffset() {
  unsigned char addr = CTRL3;
  unsigned char stat = _readReg8(addr);
  return ((stat & NOSCO) > 0x00);
}

void Maxim::Max11210::setSysGainCal(long value) {
  unsigned char addr = SGC;
  _writeReg24(addr, value);
}    

long Maxim::Max11210::getSysGainCal() {
  unsigned char addr = SGC;
  return _readReg24(addr);
}

void Maxim::Max11210::setSysOffsetCal(long value) {
  unsigned char addr = SOC;
  _writeReg24(addr, value);    
}

long Maxim::Max11210::getSysOffsetCal() {
  unsigned char addr = SOC;
  return _readReg24(addr);
}
  
void Maxim::Max11210::setSelfCalGain(long value) {
  unsigned char addr = SCGC;
  _writeReg24(addr, value);    
}

long Maxim::Max11210::getSelfCalGain() {
  unsigned char addr = SCGC;
  return _readReg24(addr);
}
  
void Maxim::Max11210::setSelfCalOffset(long value) {
  unsigned char addr = SCOC;
  _writeReg24(addr, value);    
}

long Maxim::Max11210::getSelfCalOffset() {
  unsigned char addr = SCOC;
  return _readReg24(addr);
}
