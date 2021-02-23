#ifndef AD770X_H
#define AD770X_H

#include <Arduino.h>
#if defined(ESP32)
	#include <SPI.h>
#endif
/*
 * AD7705/AD7706 Library
 * Kerry D. Wong
 * http://www.kerrywong.com
 * Initial version 1.0 3/2011
 * Updated 2.0 02/2021 (BY BALLESTEROS ULYSSE)
 */

class AD770X {
public:
    //register selection 
    //RS2 RS1 RS0
    static const byte REG_CMM = 0x0; //communication register 8 bit
    static const byte REG_SETUP = 0x1; //setup register 8 bit
    static const byte REG_CLOCK = 0x2; //clock register 8 bit
    static const byte REG_DATA = 0x3; //data register 16 bit, contains conversion result
    static const byte REG_TEST = 0x4; //test register 8 bit, POR 0x0
    static const byte REG_NOP = 0x5; //no operation
    static const byte REG_OFFSET = 0x6; //offset register 24 bit
    static const byte REG_GAIN = 0x7; // gain register 24 bit

    //channel selection for AD7706 (for AD7705 use the first two channel definitions)
    //CH1 CH0
    static const byte CHN_AIN1 = 0x0; //AIN1; calibration register pair 0
    static const byte CHN_AIN2 = 0x1; //AIN2; calibration register pair 1
    static const byte CHN_COMM = 0x2; //common; calibration register pair 0
    static const byte CHN_AIN3 = 0x3; //AIN3; calibration register pair 2

    //output update rate
    //CLK FS1 FS0
    static const byte UPDATE_RATE_20 = 0x0; // 20 Hz
    static const byte UPDATE_RATE_25 = 0x1; // 25 Hz
    static const byte UPDATE_RATE_100 = 0x2; // 100 Hz
    static const byte UPDATE_RATE_200 = 0x3; // 200 Hz
    static const byte UPDATE_RATE_50 = 0x4; // 50 Hz
    static const byte UPDATE_RATE_60 = 0x5; // 60 Hz
    static const byte UPDATE_RATE_250 = 0x6; // 250 Hz
    static const byte UPDATE_RATE_500 = 0x7; // 500 Hz

    //operating mode options
    //MD1 MD0
    static const byte MODE_NORMAL = 0x0; //normal mode
    static const byte MODE_SELF_CAL = 0x1; //self-calibration
    static const byte MODE_ZERO_SCALE_CAL = 0x2; //zero-scale system calibration, POR 0x1F4000, set FSYNC high before calibration, FSYNC low after calibration
    static const byte MODE_FULL_SCALE_CAL = 0x3; //full-scale system calibration, POR 0x5761AB, set FSYNC high before calibration, FSYNC low after calibration

    //gain setting
    static const byte GAIN_1 = 0x0;
    static const byte GAIN_2 = 0x1;
    static const byte GAIN_4 = 0x2;
    static const byte GAIN_8 = 0x3;
    static const byte GAIN_16 = 0x4;
    static const byte GAIN_32 = 0x5;
    static const byte GAIN_64 = 0x6;
    static const byte GAIN_128 = 0x7;

    static const byte UNIPOLAR = 0x0;
    static const byte BIPOLAR = 0x1;

    static const byte CLK_DIV_1 = 0x1;
    static const byte CLK_DIV_2 = 0x2;

    byte spiTransfer(volatile byte data) {
		#if defined(ESP32)
			vspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
			byte result = vspi->transfer(data);
			vspi->endTransaction();
			
			return result;
		#else
			SPDR = data;

			while (!(SPSR & _BV(SPIF)));

			return SPDR;
		#endif
    };
	
	#if defined(ESP32)
		AD770X(double vref, SPIClass * _vspi, int Clk, int CS);
	#else
		AD770X(double vref, int MOSI, int MISO, int SPIClock, int CS);
	#endif
    void setNextOperation(byte reg, byte channel, byte readWrite);
    void writeClockRegister(byte CLKDIS, byte CLKDIV, byte outputUpdateRate);
    void writeSetupRegister(byte operationMode, byte gain, byte unipolar, byte buffered, byte fsync);
    unsigned int readADResultRaw(byte channel);
    double readADResult(byte channel, float refOffset = 0.0);
    void reset();
    bool dataReady(byte channel);
    void init(byte channel);
    void init(byte channel, byte clkDivider, byte polarity, byte gain, byte updRate);
	#if defined(ESP32)
	void updateSPI(SPIClass * _vspi){
		vspi = _vspi;
	}
	#endif
private:
	#if defined(ESP32)
		SPIClass * vspi;
		int spiClk;
	#else
		int pinMOSI = 11; //MOSI
		int pinMISO = 12; //MISO
		int pinSPIClock = 13; //SCK
	#endif
    int pinCS = 10; //CS
    double VRef;
    unsigned int readADResult();
};
#endif
