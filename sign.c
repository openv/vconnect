/*
 *  sign.c
 *  vConnect
 *
 *  Created by Frank Nobis on 15.01.12.
 *  Copyright 2012 Radio-DO.de. All rights reserved.
 *
 */


int main()
{
	unsigned short int usiX1;	// this is 16 bit
	signed short int siX2;		// this is 15 bit and a sign bit
	usiX1 = -1;
	usiX1 ^= 0xffff;
	usiX1++;
	siX2 = -usiX1;
	
	
	int x = 0;
	
	char buffer[] = { 1, 0, 0, 1 };
	
	for (int i=3; i>0; i--) {
		x += buffer[i];
		x <<= 8;
	}
	x += buffer[0];
	
	return 0;
}