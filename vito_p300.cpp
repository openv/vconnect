/*
 *  vito_p300.cpp
 *  vConnect
 *
 *  Created by Frank Nobis on 23.11.11.
 *  Copyright 2011 Radio-DO.de. All rights reserved.
 *
 */

#include <iostream>
#include <cerrno>
#include <sys/ioctl.h>
#include "vito_p300.h"
#include "unit.h"


void command::calcCRC(int bufferEnd)
{
	m_ucCRC=0;
	
	for (int i=1; i< bufferEnd-1; i++) {
		m_ucCRC += m_ucBuffer[i];
	}
	
	m_ucCRC &= 0xff;
	
	DPRINT(fprintf(stderr, "m_ucCRC = %x\n", m_ucCRC);)
}

bool command::checkCRC()
{
	calcCRC(m_totalBufferLen);
	return m_ucCRC == m_ucBuffer[m_totalBufferLen-1]; 
}

int bcd2int(unsigned char b)
{
	int i = 0;
	
	i = (b >> 4)  & 0x0f;
	i *= 10;
	i += b & 0x0f;
	
	return i;
}

void command::sendCommand()
{
	int w = 0;
	unsigned char byte = 0;
	
	clear_Buffer();
	m_ucBuffer[0] = 0x41;			// 0x41 is the start of frame
	m_ucBuffer[1] = 1+1+2+1;		// 1 command, 1 subcommand, 2 addr, 1 read
	m_ucBuffer[2] = 0x00;			// 0x00 to read, 0x01 to write
	m_ucBuffer[3] = 0x01;
	m_ucBuffer[4] = m_uiAddr >> 8;	// high byte
	m_ucBuffer[5] = m_uiAddr & 0xff;// low byte of commands address
	m_ucBuffer[6] = m_ucLen;
	m_totalBufferLen = 8;
	calcCRC(m_totalBufferLen);
	m_ucBuffer[7] = m_ucCRC;

	DPRINT(std::cerr << "sendCommand: buffer" << std::endl;)
	DPRINT(dumpBuffer(m_ucBuffer, 8);)
	w = write(m_fd, m_ucBuffer, m_ucBuffer[1]+3);	// payload + 3 overhead, framestart, length and crc
	if (w == -1) {
		perror("write failed\n");
	}
	if (w > 0) {
		DPRINT(fprintf(stderr, "wrote %d bytes\n", w);)
	}
	clear_Buffer();					// clear the buffer and start reading the answer
	byte=readOneByte();
	DPRINT(dumpBuffer(&byte, 1, stderr);)
	if (byte != 0x06) {
		DPRINT(printf("stderr, got no ACK %x\n", byte);)
		m_bCommandFailed = true;
		return;						// in this case no sense to read further
	}
	byte = readOneByte();
	DPRINT(dumpBuffer(&byte, 1, stderr);)
	DPRINT(fprintf(stderr, "got ACK\n");)
	if (byte != 0x41) {
		DPRINT(fprintf(stderr, "expected 0x41 got %x\n", byte);)
		m_bCommandFailed = true;
		return;						// in this case no sense to read further
	}
	intoBuffer(0x41);
	DPRINT(fprintf(stderr, "got framestart\n");)
	
	byte = readOneByte();
	if (byte == 0) {
		m_bCommandFailed = true;
		return;						// in this case no sense to read further
	}
	intoBuffer(byte);					// This is the length of message
	DPRINT(fprintf(stderr, "got %d bytes to read further\n", byte);)

	for (int i=0; i<byte+1; i++) {				// typical one off bug here crc is NOT counted remember!
		// Fill the buffer with the rest of the message
		intoBuffer(readOneByte());
	}
	
	// time to do some checks
	// if crc is good then check further otherwise the buffer and the return value
	// is invalid. In that case set the failed flag.
	// until here a complete buffer has been received.
	// Frame is valid in terms of framestart byte 0x41
	// now check crc and the other bytes
	
	DPRINT(dumpBuffer(m_ucBuffer, m_totalBufferLen, stderr);)
	DPRINT(fprintf(stderr, "checksumm is %s\n", checkCRC() ? "good" : "bad");)

	if (checkCRC()) {
		// Wenn Anfrage, dann erwarten wir b2 und b3 gleich 1
		if ((m_ucBuffer[1] != 1) and (m_ucBuffer[2] != 1)) {
			DPRINT(std::cerr << "(b1 and b2) != 1" << std::endl;)
			m_bCommandFailed = true;
			return;
		}
		if (m_cMyUnit != NULL) {	// Anhand der Länge und des Types innerhalb der Unit dekodieren
			m_cMyUnit->decodeBuffer(m_ucBuffer+7, m_ucLen);
			// Ausgabe
		}
	}
#if 0

	if (checkCRC()) {
		// Wenn Anfrage, dann erwarten wir b2 und b3 gleich 1
		if ((m_ucBuffer[1] != 1) and (m_ucBuffer[2] != 1)) {
			DPRINT(std::cerr << "(b1 and b2) != 1" << std::endl;)
			m_bCommandFailed = true;
			return;
		}
		// Alles was hier folgt, wird noch auf unit basierte Umrechnung angepasst
		// Beispiel ist unten zu sehen.
		unsigned int ival = 0;
		m_ucLen = m_ucBuffer[6];
		if (m_ucLen == 1) {
			m_val.iVal =  m_ucBuffer[7];
		}
		if ((m_ucLen == 2) && (m_cMyUnit == NULL)) {
			ival  = (m_ucBuffer[8] & 0xff) << 8;
			ival += m_ucBuffer[7];
			if (ival >= 1<<15) {
				ival ^= 0xffff;
				ival++;
				m_val.iVal = -ival;
			} else {
				m_val.iVal = ival;
			}
		} else {
			m_cMyUnit->decodeBuffer(m_ucBuffer+7,2);
		}
		if (m_ucLen == 4) {
			ival  = (m_ucBuffer[10] & 0xff) << 24;
			ival |= (m_ucBuffer[9] & 0xff) << 16;
			ival |= (m_ucBuffer[8] & 0xff) << 8;
			ival |=  m_ucBuffer[7];
			m_val.iVal = ival;
		}
		if (m_ucLen == 8 && m_cMyUnit == NULL) {	// z.Z. ist dies die Systemzeit TODO: auf unabhängige Units umstellen
			struct tm vtime;
			bzero(&vtime, sizeof(vtime));
			// Jetzt wird die Unixzeit zusammengebaut.
			// Viessmann liefert acht Bytes mit einer BCD kodierten Zeit
			vtime.tm_year = (bcd2int(m_ucBuffer[7])-19)*100 + bcd2int(m_ucBuffer[8]);
			vtime.tm_mon  = bcd2int(m_ucBuffer[ 9])-1;
			vtime.tm_mday = bcd2int(m_ucBuffer[10]);
			// in m_ucBuffer[11] ist der Wochentag kodiert. Das ist aber anhand der Unixzeit irrelevant.
			vtime.tm_hour = bcd2int(m_ucBuffer[12]);
			vtime.tm_min  = bcd2int(m_ucBuffer[13]);
			vtime.tm_sec  = bcd2int(m_ucBuffer[14]);
			
			time_t t = mktime(&vtime);
			std::cerr << "time is " << t << std::endl;
			if (t == -1) {
				DPRINT(std::cerr << "mktime returned -1" << std::endl;)
				m_val.iVal = 0;
				m_bCommandFailed = true;
			} else {
				m_val.iVal = t;
			}
		}
//		if (m_ucLen == 8 && m_cMyUnit != NULL) {
//			m_cMyUnit->decodeAsCycleTime(m_ucBuffer+7,8);
//		}
	} else {
		m_val.iVal = 0;
		m_bCommandFailed = true;
	}
//	if ((m_val.iVal > 130) or m_bCommandFailed) {
//		// just to check why there are values around 6k
//		FILE *dumpFile = NULL;
//		dumpFile = fopen("/tmp/vconn.dump", "a");
//		if (dumpFile == NULL) {
//			m_val.iVal = 0;
//			m_bCommandFailed = true;
//			return;
//		}
//		time_t t = time(NULL);
//		struct tm *ts = localtime(&t);
//		
//		fprintf(dumpFile, "+++\nvConnect large value at time: %s\n", asctime(ts));
//		fprintf(dumpFile, "val              = %d\n", m_val.iVal);
//		fprintf(dumpFile, "m_bCommandFailed = %s\n", m_bCommandFailed ? "true" : "false");
//		fprintf(dumpFile, "buffer:\n");
//		dumpBuffer(m_ucBuffer, m_totalBufferLen, dumpFile);
//		fprintf(dumpFile, "+++\n\n");
//		fclose(dumpFile);
//	}
#endif
	
}

 char command::readOneByte() {
	int n;
	 char c;
	 
	n = read(m_fd, &c, 1);
	if (n ==1) {
		return c;
	}
	if (n == 0) {
		 fprintf(stderr, "read error in: readOneByte: read returned wirh 0\n");
	}
	if (n == -1) {
		fprintf(stderr, "read error in: readOneByte\n");
		perror("x");
	}
	return 0;
}

void command::dumpBuffer(unsigned char *buffer, int len, FILE *dumpTo) {
	int i;
	
	for (i=0; i<len; i++) {
		fprintf(dumpTo, "buffer[0x%02d] = 0x%02x\n", i, (unsigned char) buffer[i]);
	}
}

std::ostream& operator<< (std::ostream &out, command cCommand)
{
	if (cCommand.m_bCommandFailed) {
		out << cCommand.get_myName() << ".value U";
	} else {
		out << cCommand.get_myName() << ".value " << cCommand.get_UnitValue();
	}

	return out;
}


void p300::open_socket(const char *hostName, int port) throw(char *)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0) {
		std::string e1 = "opening socket: ";
		std::string e2 = strerror(errno);
		throw(e1 + e2);
	}
    server = gethostbyname(hostName);
    if (server == NULL) {
		throw("no such host");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
		  (char *)&serv_addr.sin_addr.s_addr,
		  server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(m_fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");
	
	m_file = fdopen(m_fd, "r+");
	
	DPRINT(fprintf (stderr, "Connected:\n");)
	
}

void p300::open_device(const char *deviceName, int baud, int stopbits, int parity)
{
	int status=0;
	struct termios currentTermios;
	
	m_bInitSuccess = false;	// at this stage nothing is initialised
	
	// open the device
	if ((m_fd = open(deviceName, O_RDWR) < 0)) {
		throw ("open failed");
	}
	// get the linediscipline and save it
	status = tcgetattr(m_fd, &m_savedTermios);
	if (status < 0) {
		throw ("tcgetattr failed");
	}
	currentTermios = m_savedTermios;
	// set the viessmann values 4800 8NE2
	cfmakeraw(&currentTermios);
	currentTermios.c_cflag = (CLOCAL | B4800 | CS8 | CREAD| PARENB | CSTOPB);
	status = tcsetattr(m_fd, TCSAFLUSH, &currentTermios);
	if (status < 0) {
		throw ("tcsetattr failed");
	}
	/* DTR High fuer Spannungsversorgung */
	int modemctl = 0;
	ioctl(m_fd, TIOCMGET, &modemctl);
	modemctl |= TIOCM_DTR;
	status = ioctl(m_fd,TIOCMSET,&modemctl);
	if (status < 0) {
		throw ("ioctl failed");
	}
	// reopen the file as a stream
	m_file = fdopen(m_fd, "r+");
	
	// now we good
	m_bInitSuccess = true;
}

void p300::init_vito()
{
	 char val		= 0;
	 char initVal	= 0x04;
	char initSeq[]	= { 0x16, 0, 0 };
	m_bInitSuccess = false;

	DPRINT(fprintf(stderr, "init: start:\n");)
	flush_file();
	
init_again:
	
	DPRINT(fprintf(stderr, "init: write initVal:\n");)
	p300_write(&initVal);

	val = p300_read();
	if (val == 0x6) {	// in this state a 0x06 should not happen, but it
						// did, sometimes when there is no cleanup at the end
						// so write the init sequence again
		DPRINT(fprintf(stderr, "init; state 1 got 6: init sequence done??? Let's try again...\n");)
		//m_bInitSuccess = true;
		//flush_file();
		//return;
		goto init_again;
	}
	do {
		DPRINT(fprintf(stderr, "init: state 2 write initSeq:\n");)
		p300_write(initSeq, sizeof(initSeq));
		val = p300_read();
		DPRINT(fprintf(stderr, "init: state 2 val=%d\n", val);)
	} while (val == 0x05);
	
	// val = p300_read();
	if (val == 0x6) {
		DPRINT(fprintf(stderr, "init; state 3 got 6: init sequence done\n");)
		m_bInitSuccess = true;
	}
	flush_file();
	DPRINT(fprintf(stderr, "init: done:\n");)
}

p300::~p300() {
	// last thing to do is write a 4 to reset vito to default sending 5's
	char c=0x04;
	p300_write(&c, 1);
}

ssize_t                                         /* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
	size_t  nleft;
	ssize_t nread;
	char    *ptr;
	
	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;              /* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;                          /* EOF */
		
		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);              /* return >= 0 */
}

void p300::flush_file() {
	DPRINT(fprintf(stderr, "flush_file:\n");)

	char string[100];
	
	tcflush(m_fd, TCIOFLUSH);		// ok, this IS low level, but hey
	fseek(m_file, 0, SEEK_END);
	fcntl(m_fd,F_SETFL,O_NONBLOCK);
	while(readn(m_fd,string,sizeof(string))>0);
	fcntl(m_fd,F_SETFL,!O_NONBLOCK);
	
}

 char p300::p300_read()
{
	int		n=0;
	 char	c=0;
	
	n = read(m_fd, &c, 1);
	if (n < 0) {
		perror("p300_read error");
	}
	return c;
}

void p300::p300_write( char *c, int count)
{
	int n=0;
	
	n = write(m_fd, c, count);
	if (n < 0) {
		perror("p300_write error\n");
		exit(EXIT_FAILURE);
	}
	DPRINT(fprintf(stderr, "p300_write: n=%d, count=%d\n", n, count);)
	assert(n == count);
	DPRINT(fprintf(stderr, "p300_write: %d\n", n);)
	tcdrain(m_fd);
}
