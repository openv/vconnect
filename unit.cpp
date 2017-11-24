/*
 *  unit.cpp
 *  vConnect
 *
 *  Created by Frank Nobis on 11.12.11.
 *  Copyright 2011 Radio-DO.de. All rights reserved.
 *
 */

#include "unit.h"

#pragma mark public functions

void vito_unit::decodeBuffer(unsigned char *buffer, int bufferLen)
{
	switch (m_eUnitType) {
		case CycleTime:
			//
			decodeAsCycleTime(buffer, bufferLen);
			break;
		case Temperature:
			//
			decodeAsTemperature(buffer);
			break;
		case Raw:
			//
			decodeAsRaw(buffer, bufferLen);
			break;
		case Integer1:
			//
			decodeAsInteger(buffer, 1);
			break;
		case Integer2:
			//
			decodeAsInteger(buffer, 2);
			break;
		case Integer4:
			//
			decodeAsInteger(buffer, 4);
			break;
		default:
			// D.h. Unknown Type
			break;
	}
}

#pragma mark private functions

void vito_unit::decodeAsCycleTime(unsigned char *buffer, int bufferLen)
{
	// the buffer contains up to 8 bytes in pais of on/off CycleTime Units
	
	std::stringstream *decodedClearText;
	int h,m;
	
	decodedClearText = new std::stringstream;
	
	for (int i=0; i<bufferLen; i+=2) {
		if (buffer[i] == 0xff) {
			*decodedClearText << i/2+1 << ":An:--     :Aus:--" << std::endl;
		} else {
			h = (buffer[i] & 0xF8) >> 3;
			m = (buffer[i] & 0x07) * 10;
			*decodedClearText
			<< i/2+1 
			<< ":An:"
			<< std::setw(2) << std::setfill('0')
			<< h << ":" 
			<< std::setw(2) << std::setfill('0') 
			<< m;

			h = (buffer[i+1] & 0xF8) >> 3;
			m = (buffer[i+1] & 0x07) * 10;
			*decodedClearText 
			<< "  :Aus:" 
			<< std::setw(2) << std::setfill('0')
			<< h << ":" 
			<< std::setw(2) << std::setfill('0') 
			<< m
			<< std::endl;
		}
	}
	setValue(decodedClearText->str());
}

void vito_unit::decodeAsTemperature(unsigned char *buffer)
{
	std::stringstream *decodedClearText;
	float f;
	
	decodedClearText = new std::stringstream;
	
	f = decode2BytesWithSign(buffer[1], buffer[0]) *  m_fReadMultiplier;
	*decodedClearText << std::setprecision(1) << std::fixed
		<< f;
	
	setValue(decodedClearText->str());
}

int vito_unit::decode2BytesWithSign(unsigned char hByte, unsigned char lByte)
{
	short int ival = 0;
	
	ival  = (hByte & 0xff) << 8;
	ival += lByte;
	if (ival >= 1<<15) {
		ival ^= 0xffff;
		ival++;
		ival = -ival;
	}
	return ival;
}

void vito_unit::decodeAsRaw(unsigned char *buffer, int bufferLen)
{
	std::stringstream *decodedClearText;
	decodedClearText = new std::stringstream;

	*decodedClearText << "0x" << std::hex;// << std::setw(8);

//	for (int i=0; i<bufferLen; i++) {
//		*decodedClearText << buffer[i];
//	}
	*decodedClearText << buffer;
	setValue(decodedClearText->str());
}

void vito_unit::decodeAsInteger(unsigned char *buffer, int bufferLen)
{
	std::stringstream *decodedClearText;
	decodedClearText = new std::stringstream;
	int ival=0;
	
	switch (bufferLen) {
		case 1:
			ival = buffer[0];
			break;
		case 2:
		case 4:
		default:
			for (int i=bufferLen-1; i>0; i--) {
				ival += buffer[i];
				ival <<= 8;
			}
			ival += buffer[0];
	}
	
	*decodedClearText << ival;
	setValue(decodedClearText->str());
}

