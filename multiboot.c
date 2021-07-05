#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <byteswap.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "multiboot.h"

//---------------------------------------------------------------------------
uint32_t Spi32(uint32_t val)
{
	union {
		uint32_t u32;
		u_char uc[4];
	} x;

	x.u32 = bswap_32(val);
	wiringPiSPIDataRW(0, x.uc, 4);

	return bswap_32(x.u32);
}
//---------------------------------------------------------------------------
void multiboot(const char *filename)
{

	wiringPiSPISetupMode(0, 1000000, 3);


	// -----------------------------------------------------
	// get filesize
	int fd = open(filename, O_RDONLY);

	if(fd == -1)
	{
		fprintf(stderr, "Error opening game ROM: fopen\n");
		exit(1);
	}

	struct stat stbuf;

	if(fstat(fd, &stbuf) == -1)
	{
		fprintf(stderr, "Error opening game ROM: fstat\n");
		exit(1);
	}

	uint32_t fsize = stbuf.st_size;

	if(fsize > 0x40000)
	{
		fprintf(stderr, "File size Error Max 256KB\n");
		exit(1);
	}

	close(fd);


	// -----------------------------------------------------
	// read file
	FILE* fp = fopen(filename, "rb");

	if(fp == NULL)
	{
		fprintf(stderr, "Error opening game ROM: Fopen Error\n");
		exit(1);
	}

	uint8_t* fdata = calloc(fsize + 0x10, sizeof(uint8_t));

	if(fdata == NULL)
	{
		fprintf(stderr, "Error opening game ROM: Calloc Error\n");
		exit(1);
	}

	fread(fdata, 1, fsize, fp);
	fclose(fp);


	// -----------------------------------------------------
	printf("Waiting for GBA. Please make sure the link cable is connected and turn on your GBA.\n");

	uint32_t recv;

	do
	{
		recv = Spi32(0x6202) >> 16;
		usleep(10000);

	} while(recv != 0x7202);

  printf("Connected, loading game...");

	// -----------------------------------------------------
	Spi32(0x6102);

	uint16_t* fdata16 = (uint16_t*)fdata;

	for(uint32_t i=0; i<0xC0; i+=2)
	{
		Spi32(fdata16[i / 2]);
	}

	Spi32(0x6200);


	// -----------------------------------------------------
	Spi32(0x6202);
	Spi32(0x63D1);

	uint32_t token = Spi32(0x63D1);

	if((token >> 24) != 0x73)
	{
		fprintf(stderr, "Failed handshake!\n");
		exit(1);
	}


	uint32_t crcA, crcB, crcC, seed;

	crcA = (token >> 16) & 0xFF;
	seed = 0xFFFF00D1 | (crcA << 8);
	crcA = (crcA + 0xF) & 0xFF;

	Spi32(0x6400 | crcA);

	fsize +=  0xF;
	fsize &= ~0xF;

	token = Spi32((fsize - 0x190) / 4);
	crcB  = (token >> 16) & 0xFF;
	crcC  = 0xC387;

	// -----------------------------------------------------
	uint32_t* fdata32 = (uint32_t*)fdata;

	for(uint32_t i=0xC0; i<fsize; i+=4)
	{
		uint32_t dat = fdata32[i / 4];

		// crc step
		uint32_t tmp = dat;

		for(uint32_t b=0; b<32; b++)
		{
			uint32_t bit = (crcC ^ tmp) & 1;

			crcC = (crcC >> 1) ^ (bit ? 0xc37b : 0);
			tmp >>= 1;
		}

		// encrypt step
		seed = seed * 0x6F646573 + 1;
		dat = seed ^ dat ^ (0xFE000000 - i) ^ 0x43202F2F;

		// send
		uint32_t chk = Spi32(dat) >> 16;

		if(chk != (i & 0xFFFF))
		{
			fprintf(stderr, "Transmission error at byte %zu: chk == %08x\n", i, chk);
			exit(1);
		}
	}

	// crc step final
	uint32_t tmp = 0xFFFF0000 | (crcB << 8) | crcA;

	for(uint32_t b=0; b<32; b++)
	{
		uint32_t bit = (crcC ^ tmp) & 1;

		crcC = (crcC >> 1) ^ (bit ? 0xc37b : 0);
		tmp >>= 1;
	}


	// -----------------------------------------------------
	Spi32(0x0065);

	do
	{
		recv = Spi32(0x0065) >> 16;
		usleep(10000);

	} while(recv != 0x0075);

	Spi32(0x0066);
	uint32_t crcGBA = Spi32(crcC & 0xFFFF) >> 16;

	printf("\n\nLoading complete! To stop playing turn off the GameBoy and press Ctrl+C here.\n");

  usleep(5000000);

  free(fdata);
}
