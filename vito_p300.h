/*
 *  vito_p300.h
 *  vConnect
 *
 *  Created by Frank Nobis on 23.11.11.
 *  Copyright 2011 Radio-DO.de. All rights reserved.
 *
 */

#ifndef __vito_p300_h
#define __vito_p300_h

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "unit.h"

#define COMMAND_BUFFER_SIZE 20

#ifdef __DEBUG__
#define DPRINT(x)	x
#else
#define DPRINT(x)
#endif

union command_value_t {
	int iVal;
	float fVal;
};

class command {

private:
	std::string			m_myName;
	unsigned int	m_uiAddr;
	unsigned char	m_ucLen;
	int				m_totalBufferLen;
	float			m_fReadMultiplier;
	float			m_fWriteMultiplier;
	vito_unit		*m_cMyUnit;
	bool			m_bCommandFailed;
	command_value_t m_val;
	unsigned char	m_ucCRC;
	unsigned char	m_ucBuffer[COMMAND_BUFFER_SIZE];
	int				m_fd;
	FILE			*m_file;
	
		// Helper functions for internal use
	void calcCRC(int bufferEnd);
	bool checkCRC();
	 char readOneByte();
	void clear_Buffer() {
		bzero(m_ucBuffer, sizeof(m_ucBuffer));
		m_totalBufferLen = 0;
	};
	void intoBuffer(char c) {
		m_ucBuffer[m_totalBufferLen++] = c;
	}
	void common_init(std::string myName, unsigned int uiAddr=0, unsigned char ucLen=0,
					 float fReadMultiplier=1.0,
					 float fWriteMultiplier=1.0)
	{
		m_myName = myName;
		m_uiAddr = uiAddr;
		m_ucLen = ucLen;
		m_fReadMultiplier = fReadMultiplier;
		m_fWriteMultiplier = fWriteMultiplier;
		m_cMyUnit = NULL;
		m_bCommandFailed = false;
		m_val.iVal = 0;
	}
	
public:
//	command( std::string myName, unsigned int uiAddr=0, unsigned char ucLen=0,
//			float fReadMultiplier=1.0,
//			float fWriteMultiplier=1.0)
//	{
//		common_init(myName, uiAddr, ucLen, fReadMultiplier, fWriteMultiplier);
//		clear_Buffer();
//	}
	command( std::string myName, int uiAddr=0,  char ucLen=0,
			float fReadMultiplier=1.0,
			float fWriteMultiplier=1.0,
			vito_unit *cUnit=NULL)
	{
		common_init(myName, uiAddr, ucLen, fReadMultiplier, fWriteMultiplier);
		m_cMyUnit = cUnit;
		clear_Buffer();
	}
	
	void setValue(int iVal) {
		m_val.iVal = iVal;
	};
	void setValue(float fVal) {
		m_val.fVal = fVal;
	};
	int get_iValue() {
		return m_val.iVal;
	};
	float get_fValue() {
		return ((float) m_val.iVal) * m_fReadMultiplier;
	}
	std::string get_myName() {
		return m_myName;
	}
	std::string get_UnitValue() {
		if (m_cMyUnit) {
			return m_cMyUnit->getValue();
		} else {
			std::stringstream ss;
			ss << std::setw(12) << get_fValue();
			return ss.str();
		}
	}
	void sendCommand();
	int readCommand(void *buffer, size_t len);
	void setFile(int fd, FILE *f) {
		m_fd = fd;
		m_file = f;
	}
	bool commandSuccess() {
		return !m_bCommandFailed;
	}
	void dumpBuffer(unsigned char *buffer, int len, FILE *dumpTo=stderr);
	friend std::ostream& operator<< (std::ostream &out, command cCommand);
};

class p300 {
public:
	p300(const char *hostName, int port) {
		open_socket(hostName, port);
		init_vito();
	};
	p300(const char *deviceName, int baud, int stopbits, int parity) {
		open_device(deviceName, baud, stopbits, parity);
		init_vito();
	};
	~p300();
	void init_vito();
	int getFD() {
		return m_fd;
	}
	FILE* getFile() {
		return m_file;
	}
private:
	struct termios m_savedTermios;
	bool m_bInitSuccess;
	 char p300_read();
	void p300_write( char *c, int count=1);
	void open_socket(const char *hostName, int port) throw(char *);
	void open_device(const char *deviceName, int baud, int stopbits, int parity);
	void flush_file();
protected:
	int m_fd;
	FILE *m_file;
};

#endif
