/*
 *  unit.h
 *  vConnect
 *
 *  Created by Frank Nobis on 11.12.11.
 *  Copyright 2011 Radio-DO.de. All rights reserved.
 *
 */

#ifndef __unit_h
#define __unit_h

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

enum unitTypes {
	Unknown = 0,
	CycleTime = 1,
	Temperature = 2,
	Raw = 3,
	Integer1 = 4,
	Integer2 = 5,
	Integer4 = 6,
	Default = 8
	};

/*
 * Defintion der verschiedenen Einheiten
 *
 * Unit: CycleTime
 * Desc: 1 Byte Codierung der Schaltzeiten.
 *	Die schaltzeiten werden in 10 Minutenschritten aufgeteilt und codiert
 *	das spart Platz.
 *	Die oberen 5 Bit stellen die Stunden dar, die unteren 3 Bit die Anzahl
 *	von 10 Minuten Anteilen
 *
 *	 MSB                         LSB
 *  +---+---+---+---+---+---+---+---+
 *  | h4  h3  h2  h2  h0| m2-m0 * 10|
 *  +---+---+---+---+---+---+---+---+
 *
 *
 * Unit: Temperature
 * Desc: 2 Byte Codierung der Temperature 2-bit Komplement
 *
 * Unit: Raw
 * Desc: decode as hex value. Should be 2 or 4 bytes
 *
 */

class vito_unit {
private:
	int decode2BytesWithSign(unsigned char hByte, unsigned char lByte);
	void decodeAsCycleTime(unsigned char *buffer, int bufferLen);
	void decodeAsTemperature(unsigned char *buffer);
	void decodeAsRaw(unsigned char *buffer, int bufferLen);
	void decodeAsInteger(unsigned char *buffer, int bufferLen);

	unitTypes		m_eUnitType;
	std::string		m_sDecodedValue;
	float			m_fReadMultiplier;
	float			m_fWriteMultiplier;

public:
	vito_unit(unitTypes _UnitType = Unknown,
			  float fReadMultiplier=1.0,
			  float fWriteMultiplier=1.0)
	{
		m_eUnitType = _UnitType;
		m_sDecodedValue = "<Test>";
		m_fReadMultiplier = fReadMultiplier;
		m_fWriteMultiplier = fWriteMultiplier;

	};

	void decodeBuffer(unsigned char *buffer, int bufferLen);
	
	std::string getValue() {
		return m_sDecodedValue;
	}

	void setValue(std::string val) {
		m_sDecodedValue = val;
	}	
};

#endif
