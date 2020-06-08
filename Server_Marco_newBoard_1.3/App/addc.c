#include "addc.h"

#include "peltier.h"
//#include "proto.h"

// un-comment just one of the following two defines
//#define	USE_DAC_AD5640	1	// 14 bit DAC
#define	USE_DAC_AD5660	1	// 16 bit DAC

static int32_t volt[2][8];
static int32_t voltRaw[2][8];
static char buffnum = 0;
static char reading = 0;

static float dac_ch1_value = 0;
static float dac_ch2_value = 0;
static float dac_ch1_value_cur = 0;
static float dac_ch2_value_cur = 0;

#define SPI_MSB 1

#define READ_FREQUENCY 15

static const unsigned char BitReverseTable256[] =
{
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

void spi_write(char* data,int len) {
#if SPI_MSB == 0
	char tmp[256];
	if (len >= sizeof tmp)
	{
		printf("SPI: cannot send too much data >256");
		return;
	}
	char cmdraw = data[0];
	//send data
	for (int i = 0; i < len; i++) {
		tmp[i] = BitReverseTable256[data[i]];
		//tmp[i] = data[i];
	}
	char cmd = tmp[0];

	wiringPiSPIDataRW(SPI_CH, tmp, len);

	char rx = tmp[0];

	//response
	for (int i = 0; i < len; i++) {
		data[i] = BitReverseTable256[tmp[i]];
	}

	printf("spi cmd [%d][%d] read raw=[%d] lsb=[%d] msb=[%d]\n", cmdraw, cmd,rx,tmp[0], data[0]);
#else
	wiringPiSPIDataRW(SPI_CH, data, len);
#endif
}

void  bsp_DelayUS(uint64_t micros)
{
	struct timespec t1;
	if (micros > 250)
	{
		t1.tv_sec = 0;
		t1.tv_nsec = 1000 * (long)(micros - 200);
		nanosleep(&t1, NULL);
	}
	else {
		t1.tv_sec = 0;
		t1.tv_nsec = micros;
		nanosleep(&t1, NULL);
	}
}

/*
*********************************************************************************************************
*	name: bsp_InitADS1256
*	function: Configuration of the STM32 GPIO and SPI interface��The connection ADS1256
*	parameter: NULL
*	The return value: NULL
*********************************************************************************************************
*/


void bsp_InitADS1256(void)
{
#ifdef SOFT_SPI
	CS_1();
	SCK_0();
	DI_0();
#endif

	//ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_1000SPS);	/* ����ADC������ ����1:1, ����������� 1KHz */
}




/*
*********************************************************************************************************
*	name: ADS1256_InitScan
*	function: Configuration DRDY PIN for external interrupt is triggered
*	parameter: _ucDiffMode : 0  Single-ended input  8 channel�� 1 Differential input  4 channe
*	The return value: NULL
*********************************************************************************************************
*/
void ADS1256_InitScan(uint8_t _ucScanMode)
{
	g_tADS1256.ScanMode = _ucScanMode;
	/* ��ʼɨ��ǰ, ������������ */
	{
		uint8_t i;

		g_tADS1256.Channel = 0;

		for (i = 0; i < 8; i++)
		{
			g_tADS1256.AdcNow[i] = 0;
		}
	}

}

/*
*********************************************************************************************************
*	name: ADS1256_Send8Bit
*	function: SPI bus to send 8 bit data
*	parameter: _data:  data
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_Send8Bit(uint8_t _data)
{
	bsp_DelayUS(2);
	spi_write(&_data,1);
}

/*
*********************************************************************************************************
*	name: ADS1256_CfgADC
*	function: The configuration parameters of ADC, gain and data rate
*	parameter: _gain:gain 1-64
*                      _drate:  data  rate
*	The return value: NULL
*********************************************************************************************************
*/
void ADS1256_CfgADC(ADS1256_GAIN_E _gain, ADS1256_DRATE_E _drate)
{
	g_tADS1256.Gain = _gain;
	g_tADS1256.DataRate = _drate;

	ADS1256_WaitDRDY();

	{
		uint8_t buf[4];		/* Storage ads1256 register configuration parameters */

							/*Status register define
							Bits 7-4 ID3, ID2, ID1, ID0  Factory Programmed Identification Bits (Read Only)

							Bit 3 ORDER: Data Output Bit Order
							0 = Most Significant Bit First (default)
							1 = Least Significant Bit First
							Input data  is always shifted in most significant byte and bit first. Output data is always shifted out most significant
							byte first. The ORDER bit only controls the bit order of the output data within the byte.

							Bit 2 ACAL : Auto-Calibration
							0 = Auto-Calibration Disabled (default)
							1 = Auto-Calibration Enabled
							When Auto-Calibration is enabled, self-calibration begins at the completion of the WREG command that changes
							the PGA (bits 0-2 of ADCON register), DR (bits 7-0 in the DRATE register) or BUFEN (bit 1 in the STATUS register)
							values.

							Bit 1 BUFEN: Analog Input Buffer Enable
							0 = Buffer Disabled (default)
							1 = Buffer Enabled

							Bit 0 DRDY :  Data Ready (Read Only)
							This bit duplicates the state of the DRDY pin.

							ACAL=1  enable  calibration
							*/
							//buf[0] = (0 << 3) | (1 << 2) | (1 << 1);//enable the internal buffer
		buf[0] = (0 << 3) | (1 << 2) | (0 << 1);  // The internal buffer is prohibited

												  //ADS1256_WriteReg(REG_STATUS, (0 << 3) | (1 << 2) | (1 << 1));

		buf[1] = 0x08;

		/*	ADCON: A/D Control Register (Address 02h)
		Bit 7 Reserved, always 0 (Read Only)
		Bits 6-5 CLK1, CLK0 : D0/CLKOUT Clock Out Rate Setting
		00 = Clock Out OFF
		01 = Clock Out Frequency = fCLKIN (default)
		10 = Clock Out Frequency = fCLKIN/2
		11 = Clock Out Frequency = fCLKIN/4
		When not using CLKOUT, it is recommended that it be turned off. These bits can only be reset using the RESET pin.

		Bits 4-3 SDCS1, SCDS0: Sensor Detect Current Sources
		00 = Sensor Detect OFF (default)
		01 = Sensor Detect Current = 0.5 �� A
		10 = Sensor Detect Current = 2 �� A
		11 = Sensor Detect Current = 10�� A
		The Sensor Detect Current Sources can be activated to verify  the integrity of an external sensor supplying a signal to the
		ADS1255/6. A shorted sensor produces a very small signal while an open-circuit sensor produces a very large signal.

		Bits 2-0 PGA2, PGA1, PGA0: Programmable Gain Amplifier Setting
		000 = 1 (default)
		001 = 2
		010 = 4
		011 = 8
		100 = 16
		101 = 32
		110 = 64
		111 = 64
		*/
		buf[2] = (0 << 5) | (0 << 3) | (_gain << 0);
		//ADS1256_WriteReg(REG_ADCON, (0 << 5) | (0 << 2) | (GAIN_1 << 1));	/*choose 1: gain 1 ;input 5V/
		buf[3] = s_tabDataRate[_drate];	// DRATE_10SPS;	

		CS_0();	/* SPIƬѡ = 0 */
		ADS1256_Send8Bit(CMD_WREG | 0);	/* Write command register, send the register address */
		ADS1256_Send8Bit(0x03);			/* Register number 4,Initialize the number  -1*/

		ADS1256_Send8Bit(buf[0]);	/* Set the status register */
		ADS1256_Send8Bit(buf[1]);	/* Set the input channel parameters */
		ADS1256_Send8Bit(buf[2]);	/* Set the ADCON control register,gain */
		ADS1256_Send8Bit(buf[3]);	/* Set the output rate */

		CS_1();	/* SPI  cs = 1 */
	}

	bsp_DelayUS(50);
}


/*
*********************************************************************************************************
*	name: ADS1256_DelayDATA
*	function: delay
*	parameter: NULL
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_DelayDATA(void)
{
	/*
	Delay from last SCLK edge for DIN to first SCLK rising edge for DOUT: RDATA, RDATAC,RREG Commands
	min  50   CLK = 50 * 0.13uS = 6.5uS
	*/
	bsp_DelayUS(10);	/* The minimum time delay 6.5us */

	//@TODO hack remove
	//delay(1);
}




/*
*********************************************************************************************************
*	name: ADS1256_Recive8Bit
*	function: SPI bus receive function
*	parameter: NULL
*	The return value: NULL
*********************************************************************************************************
*/
static uint8_t ADS1256_Recive8Bit(void)
{
	uint8_t read = 0xff;
	//read = bcm2835_spi_transfer(0xff);
	spi_write(&read,1);
	//printf("spi rx = [%d]",read);
	return read;
}

/*
*********************************************************************************************************
*	name: ADS1256_WriteReg
*	function: Write the corresponding register
*	parameter: _RegID: register  ID
*			 _RegValue: register Value
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_WriteReg(uint8_t _RegID, uint8_t _RegValue)
{
	CS_0();	/* SPI  cs  = 0 */
	ADS1256_Send8Bit(CMD_WREG | _RegID);	/*Write command register */
	ADS1256_Send8Bit(0x00);		/*Write the register number */

	ADS1256_Send8Bit(_RegValue);	/*send register value */
	CS_1();	/* SPI   cs = 1 */
}

/*
*********************************************************************************************************
*	name: ADS1256_ReadReg
*	function: Read  the corresponding register
*	parameter: _RegID: register  ID
*	The return value: read register value
*********************************************************************************************************
*/
static uint8_t ADS1256_ReadReg(uint8_t _RegID)
{
	uint8_t read;

	CS_0();	/* SPI  cs  = 0 */
	ADS1256_Send8Bit(CMD_RREG | _RegID);	/* Write command register */
	ADS1256_Send8Bit(0x00);	/* Write the register number */

	ADS1256_DelayDATA();	/*delay time */

	read = ADS1256_Recive8Bit();	/* Read the register values */
	CS_1();	/* SPI   cs  = 1 */

	return read;
}

/*
*********************************************************************************************************
*	name: ADS1256_WriteCmd
*	function: Sending a single byte order
*	parameter: _cmd : command
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_WriteCmd(uint8_t _cmd)
{
	CS_0();	/* SPI   cs = 0 */
	ADS1256_Send8Bit(_cmd);
	CS_1();	/* SPI  cs  = 1 */
}

/*
*********************************************************************************************************
*	name: ADS1256_ReadChipID
*	function: Read the chip ID
*	parameter: _cmd : NULL
*	The return value: four high status register
*********************************************************************************************************
*/
uint8_t ADS1256_ReadChipID(void)
{
	uint8_t id;

	ADS1256_WaitDRDY();
	id = ADS1256_ReadReg(REG_STATUS);
	return (id >> 4);
}

/*
*********************************************************************************************************
*	name: ADS1256_SetSingEndChannel
*	function: Configuration channel number
*	parameter:  _ch:  channel number  0--7
*	The return value: NULL
*********************************************************************************************************
*/
static void ADS1256_SetSingEndChannel(uint8_t _ch)
{
	/*
	Bits 7-4 PSEL3, PSEL2, PSEL1, PSEL0: Positive Input Channel (AINP) Select
	0000 = AIN0 (default)
	0001 = AIN1
	0010 = AIN2 (ADS1256 only)
	0011 = AIN3 (ADS1256 only)
	0100 = AIN4 (ADS1256 only)
	0101 = AIN5 (ADS1256 only)
	0110 = AIN6 (ADS1256 only)
	0111 = AIN7 (ADS1256 only)
	1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are ��don��t care��)

	NOTE: When using an ADS1255 make sure to only select the available inputs.

	Bits 3-0 NSEL3, NSEL2, NSEL1, NSEL0: Negative Input Channel (AINN)Select
	0000 = AIN0
	0001 = AIN1 (default)
	0010 = AIN2 (ADS1256 only)
	0011 = AIN3 (ADS1256 only)
	0100 = AIN4 (ADS1256 only)
	0101 = AIN5 (ADS1256 only)
	0110 = AIN6 (ADS1256 only)
	0111 = AIN7 (ADS1256 only)
	1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are ��don��t care��)
	*/
	if (_ch > 7)
	{
		return;
	}
	ADS1256_WriteReg(REG_MUX, (_ch << 4) | (1 << 3));	/* Bit3 = 1, AINN connection AINCOM */
}

/*
*********************************************************************************************************
*	name: ADS1256_SetDiffChannel
*	function: The configuration difference channel
*	parameter:  _ch:  channel number  0--3
*	The return value:  four high status register
*********************************************************************************************************
*/
static void ADS1256_SetDiffChannel(uint8_t _ch)
{
	/*
	Bits 7-4 PSEL3, PSEL2, PSEL1, PSEL0: Positive Input Channel (AINP) Select
	0000 = AIN0 (default)
	0001 = AIN1
	0010 = AIN2 (ADS1256 only)
	0011 = AIN3 (ADS1256 only)
	0100 = AIN4 (ADS1256 only)
	0101 = AIN5 (ADS1256 only)
	0110 = AIN6 (ADS1256 only)
	0111 = AIN7 (ADS1256 only)
	1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are ��don��t care��)

	NOTE: When using an ADS1255 make sure to only select the available inputs.

	Bits 3-0 NSEL3, NSEL2, NSEL1, NSEL0: Negative Input Channel (AINN)Select
	0000 = AIN0
	0001 = AIN1 (default)
	0010 = AIN2 (ADS1256 only)
	0011 = AIN3 (ADS1256 only)
	0100 = AIN4 (ADS1256 only)
	0101 = AIN5 (ADS1256 only)
	0110 = AIN6 (ADS1256 only)
	0111 = AIN7 (ADS1256 only)
	1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are ��don��t care��)
	*/
	if (_ch == 0)
	{
		ADS1256_WriteReg(REG_MUX, (0 << 4) | 1);	/* DiffChannal  AIN0�� AIN1 */
	}
	else if (_ch == 1)
	{
		ADS1256_WriteReg(REG_MUX, (2 << 4) | 3);	/*DiffChannal   AIN2�� AIN3 */
	}
	else if (_ch == 2)
	{
		ADS1256_WriteReg(REG_MUX, (4 << 4) | 5);	/*DiffChannal    AIN4�� AIN5 */
	}
	else if (_ch == 3)
	{
		ADS1256_WriteReg(REG_MUX, (5 << 4) | 6);	/*DiffChannal   AIN5 & AIN6 */
	}
}

/*
*********************************************************************************************************
*	name: ADS1256_WaitDRDY
*	function: delay time  wait for automatic calibration
*	parameter:  NULL
*	The return value:  NULL
*********************************************************************************************************
*/
static void ADS1256_WaitDRDY(void)
{
	uint32_t i;

	for (i = 0; i < 400000; i++)
	{
		if (DRDY_IS_LOW())
		{
			break;
		}
	}
	if (i >= 400000)
	{
		printf("ADS1256_WaitDRDY() Time Out ...\r\n");
	}
}

/*
*********************************************************************************************************
*	name: ADS1256_ReadData
*	function: read ADC value
*	parameter: NULL
*	The return value:  NULL
*********************************************************************************************************
*/
static int32_t ADS1256_ReadData(void)
{
	uint32_t read = 0;
	static uint8_t buf[3];

	CS_0();	/* SPI   cs = 0 */

	ADS1256_Send8Bit(CMD_RDATA);	/* read ADC command  */

	ADS1256_DelayDATA();	/*delay time  */

							/*Read the sample results 24bit*/
	buf[0] = ADS1256_Recive8Bit();
	buf[1] = ADS1256_Recive8Bit();
	buf[2] = ADS1256_Recive8Bit();

	read = ((uint32_t)buf[0] << 16) & 0x00FF0000;
	read |= ((uint32_t)buf[1] << 8);  /* Pay attention to It is wrong   read |= (buf[1] << 8) */
	read |= buf[2];

	CS_1();	/* SPIƬѡ = 1 */

			/* Extend a signed number*/
	if (read & 0x800000)
	{
		read |= 0xFF000000;
	}

	return (int32_t)read;
}


/*
*********************************************************************************************************
*	name: ADS1256_GetAdc
*	function: read ADC value
*	parameter:  channel number 0--7
*	The return value:  ADC vaule (signed number)
*********************************************************************************************************
*/
int32_t ADS1256_GetAdc(uint8_t _ch)
{
	int32_t iTemp;

	if (_ch > 7)
	{
		return 0;
	}

	iTemp = g_tADS1256.AdcNow[_ch];

	return iTemp;
}

/*
*********************************************************************************************************
*	name: ADS1256_ISR
*	function: Collection procedures
*	parameter: NULL
*	The return value:  NULL
*********************************************************************************************************
*/
void ADS1256_ISR(void)
{
	if (g_tADS1256.ScanMode == SINGLE_ENDED_MODE)	/*  0  Single-ended input  8 channels, 1 Differential input  4 channels */
	{

		ADS1256_SetSingEndChannel(g_tADS1256.Channel);	/*Switch channel mode */
		bsp_DelayUS(5);

		ADS1256_WriteCmd(CMD_SYNC);
		bsp_DelayUS(5);

		ADS1256_WriteCmd(CMD_WAKEUP);
		bsp_DelayUS(25);

		if (g_tADS1256.Channel == 0)
		{
			g_tADS1256.AdcNow[7] = ADS1256_ReadData();
		}
		else
		{
			g_tADS1256.AdcNow[g_tADS1256.Channel - 1] = ADS1256_ReadData();
		}

		if (++g_tADS1256.Channel >= 8)
		{
			g_tADS1256.Channel = 0;
		}
	}
	else	/*DiffChannal*/
	{		/*  0  Single-ended input  8 channels, 1 Differential input  4 channels */

		ADS1256_SetDiffChannel(g_tADS1256.Channel);	/* change DiffChannal */
		bsp_DelayUS(5);

		ADS1256_WriteCmd(CMD_SYNC);
		bsp_DelayUS(5);

		ADS1256_WriteCmd(CMD_WAKEUP);
		bsp_DelayUS(25);

		if (g_tADS1256.Channel == 0)
		{
			g_tADS1256.AdcNow[3] = ADS1256_ReadData();
		}
		else
		{
			g_tADS1256.AdcNow[g_tADS1256.Channel - 1] = ADS1256_ReadData();
		}

		if (++g_tADS1256.Channel >= 4)
		{
			g_tADS1256.Channel = 0;
		}
	}
}

/*
*********************************************************************************************************
*	name: ADS1256_acq_se_diff
*	function: Data collection procedure for mixed single-ended and differential inputs
*	parameters: none
*	return value:  none
*********************************************************************************************************
*/
void ADS1256_acq_se_diff(void)
{
	// Board channels are:
	//		0	VAL_MIR		(AIN0-AIN1, differential)
	//		1	VAL_DET		(AIN2-AIN3, differential)
	//		2	VAL_SURF	(AIN4-AIN5, differential)
	//		6	VAL_REF		(AIN6, single-ended)
	//		7	THERMISTOR	(AIN7, single-ended)
	
	if (g_tADS1256.Channel < 3)		// channels 0,1,2 are differential
	{
		
		ADS1256_SetDiffChannel(g_tADS1256.Channel);	
		bsp_DelayUS(5);

		ADS1256_WriteCmd(CMD_SYNC);
		bsp_DelayUS(5);

		ADS1256_WriteCmd(CMD_WAKEUP);
		bsp_DelayUS(25);

		if (g_tADS1256.Channel == 0)	// when requesting channel X, data received belongs to channel X-1
		{								// see ADS1256 Datasheet, Page 21, Fig.19
			g_tADS1256.AdcNow[7] = ADS1256_ReadData();	
		}
		else
		{
			g_tADS1256.AdcNow[g_tADS1256.Channel-1] = ADS1256_ReadData();	
		}

		if (++g_tADS1256.Channel >= 3)	// jump to the first single-ended input
		{
			g_tADS1256.Channel = 6;
		}
	}
	else							// channels 6,7 are single-ended
	{
		ADS1256_SetSingEndChannel(g_tADS1256.Channel);	
		bsp_DelayUS(5);

		ADS1256_WriteCmd(CMD_SYNC);
		bsp_DelayUS(5);

		ADS1256_WriteCmd(CMD_WAKEUP);
		bsp_DelayUS(25);

		if(g_tADS1256.Channel == 6)
			g_tADS1256.AdcNow[2] = ADS1256_ReadData();
		else
			g_tADS1256.AdcNow[g_tADS1256.Channel-1] = ADS1256_ReadData();	

		if (++g_tADS1256.Channel >= 8)
		{
			g_tADS1256.Channel = 0;
		}
	}
}


/*
*********************************************************************************************************
*	name: ADS1256_Scan
*	function:
*	parameter:NULL
*	The return value:  1
*********************************************************************************************************
*/
uint8_t ADS1256_Scan(void)
{
	if (DRDY_IS_LOW())
	{
		//ADS1256_ISR();		// this function call works with ALL channels either single-ended or differential
		ADS1256_acq_se_diff();	// this function call addresses mixed single-ended and differential channels
		return 1;
	}

	return 0;
}
/*
*********************************************************************************************************
*	name: Write_DAC8552
*	function:  DAC send data
*	parameter: channel : output channel number
*			   data : output DAC value
*	The return value:  NULL
*********************************************************************************************************
*/
void Write_DAC8552(uint8_t channel, uint16_t Data)
{
	uint8_t i;

	CS1_1();
	CS1_0();
	spi_write(&channel,1);
	uint8_t tmp;
	tmp = (Data >> 8);
	spi_write(&tmp,1);
	tmp = (Data & 0xff);
	spi_write(&tmp, 1);
	CS1_1();
}

/*
*********************************************************************************************************
*	name: addc_UpdateDAC_AD5640
*	function:  update the AD5640 dac output 
*	parameters: data : output DAC word
*	return value:  none
*********************************************************************************************************
*/
void addc_UpdateDAC_AD5640(uint16_t Data)
{
	CS1_1();
	CS1_0();
	uint8_t tmp;
	tmp = (Data >> 8);
	spi_write(&tmp,1);
	tmp = (Data & 0xff);
	spi_write(&tmp, 1);
	CS1_1();
}

/*
*********************************************************************************************************
*	name: addc_UpdateDAC_AD5660
*	function:  update the AD5660 dac output
*	parameters: data : output DAC word
*	return value:  none
*********************************************************************************************************
*/
void addc_UpdateDAC_AD5660(uint16_t Data)
{
	CS1_1();
	CS1_0();
	uint8_t tmp;
	tmp = 0x00;
	spi_write(&tmp, 1);
	tmp = (Data >> 8);
	spi_write(&tmp, 1);
	tmp = (Data & 0xff);
	spi_write(&tmp, 1);
	CS1_1();
}


/*
*********************************************************************************************************
*	name: addc_UpdateDAC
*	function:  update either the AD5640 or the AD5660 dac output
*	parameters: data : output DAC word
*	return value:  none
*********************************************************************************************************
*/
void addc_UpdateDAC(uint16_t Data)
{
#ifdef USE_DAC_AD5640
	addc_UpdateDAC_AD5640(Data);
#endif

#ifdef USE_DAC_AD5660
	addc_UpdateDAC_AD5660(Data);
#endif
}

/*
*********************************************************************************************************
*	name: addc_Voltage_Convert_AD5660
*	function:  Voltage value conversion function, input is voltage, output is corresponding DAC word
*	parameters: voltage : desired output DAC voltage
*	return value:  the corresponding word
*********************************************************************************************************
*/
uint16_t addc_Voltage_Convert_AD5660(float voltage)
{
	float Vref_AD5660 = 2.5;		// fixed for this IC
	
	if (voltage >= Vref_AD5660) return 65535;	// 16 bit => 65535 = 2^16-1

	uint16_t _D_;
	_D_ = (uint16_t)(65535 * voltage / Vref_AD5660);
	//printf("\t0x%4x", _D_);

	return _D_;
}


/*
*********************************************************************************************************
*	name: addc_Voltage_Convert_AD5640
*	function:  Voltage value conversion function, input is voltage, output is corresponding DAC word
*	parameters: voltage : desired output DAC voltage
*	return value:  the corresponding word
*********************************************************************************************************
*/
uint16_t addc_Voltage_Convert_AD5640(float voltage)
{
	float Vref_AD5640 = 2.5;		// fixed for this IC

	if (voltage >= Vref_AD5640) return 16383;	// 14 bit => 16384 = 2^14

	uint16_t _D_;
	_D_ = (uint16_t)(16383 * voltage / Vref_AD5640);
	//printf("\t0x%4x", _D_);

	return _D_;
}




/*
*********************************************************************************************************
*	name: addc_Voltage_Convert
*	function:  DAC Voltage value conversion function
*	parameter: voltage : desired DAC output
*	return value:  word for DAC chip
*********************************************************************************************************
*/
uint16_t addc_Voltage_Convert(float voltage)
{
	uint16_t _D_;

#ifdef USE_DAC_AD5640
	_D_ = addc_Voltage_Convert_AD5640(voltage);
#endif
	
#ifdef USE_DAC_AD5660
	_D_ = addc_Voltage_Convert_AD5660(voltage);
#endif

	return _D_;
}

extern float tmir;
extern char MESURE_EN_COURS;
static PI_THREAD(th_sensor_simulator)
{
#define V_DET 0
#define V_REF 1
#define V_MIR 2
#define V_SURF 6
#define V_INT 4
#define V(x) ((x) * 1000000.0)

	double TMIR = V(1.259 + (rand() % 100) / 1000.0); //1.26 is 4, 1.26 is 8
	double pel = 0;

	double DET = V(1.3 + (rand()%10)/10.0);
	double DET_RND = rand() % 10 / 10.0;
	double DET_NORMAL = DET;
	char det_changed = 0;
	signed char direction = 1;
	double gain = 100;
	double oldtimir = tmir;

	tmir = 10;

	while (1)
	{
		if (MESURE_EN_COURS == 0)
			det_changed = 0;

		volt[buffnum][V_DET] = DET;// reflextion 0 : V_REF
		/*change reflextion if temp is 0*/
		if (tmir < 0.00 && det_changed ==0) {
			det_changed = 1;
			DET += V(DET_RND);
			printf("SIMULATOR: flash point DET\n");
		}
		else
		if (tmir > 0.0 && det_changed){
			if (tmir < oldtimir) {
				if (direction != -1)
					gain /= 2.0; //
				direction = -1;
			}
			else 
				if (tmir > oldtimir) {
				if (direction != 1)
					gain /= 2.0; //
				direction = 1;
			}

			DET -= (tmir*3)*(direction*gain);
			if (DET < DET_NORMAL)
				DET = DET_NORMAL;

			//printf("DET=[%d]\ntmir=[%f]\ndirection=[%d]\ngain=[%f]", DET,tmir, direction, gain);
		}
		oldtimir = tmir;


		volt[buffnum][V_SURF] = V(0.19 +((rand()%10)/1000.0));// T surf 15

		volt[buffnum][V_MIR] = TMIR;// T mirror 4 ==> R = 1000

		pel = perltier_get_perc();
		if (pel > 0 || tmir > -10) //max -15
			TMIR += (pel * 10); //

		//printf("TMIR =[%f]\n",TMIR);
		//printf("peltier =[%f]\n", pel);

		volt[buffnum][V_REF] = V(2.5);// 2.5v
		volt[buffnum][V_INT] = V(2.5);// VREF / 2

		//set dac
		if (dac_ch1_value_cur != dac_ch1_value) {
			dac_ch1_value = dac_ch1_value_cur;
			uint16_t d;
			d = addc_Voltage_Convert(dac_ch1_value);
			addc_UpdateDAC(d);
			printf("dac A updated = %f\n", dac_ch1_value);
		}

		delay(1000 / READ_FREQUENCY + 1);
	}
}


static PI_THREAD(th_sensor)
{
	//int32_t adc[8];
	uint8_t i;
	uint8_t ch_num;
	int32_t iTemp;
	uint8_t buf[3];
	ch_num = 8;
	while (1)
	{
		reading = 1;
		while ((ADS1256_Scan() == 0)) delay(1);
		char nextb = buffnum+1;
		if (nextb >= 2) nextb = 0;
		for (i = 0; i < ch_num; i++)
		{
			voltRaw[nextb][i] = ADS1256_GetAdc(i);
			volt[nextb][i] = (voltRaw[nextb][i] * 100) / 167;
		}
		buffnum = nextb; //switch buffer
		reading = 0;

		//set dac
		if (dac_ch1_value_cur != dac_ch1_value) 
		{
			dac_ch1_value = dac_ch1_value_cur;
						
			uint16_t d;
			d = addc_Voltage_Convert(dac_ch1_value);
			addc_UpdateDAC(d);
			//printf("dac AD56xx updated = %.3f\n", dac_ch1_value);			
		}

		delay(1000 / READ_FREQUENCY +1);
	}
	//bcm2835_spi_end();
	//bcm2835_close();
}


void addc_dac_set(float val) 
{
	dac_ch1_value_cur = val;
}



#define SPI_CPHA                0x01
#define SPI_CPOL                0x02

#define SPI_MODE_0              (0|0)
#define SPI_MODE_1              (0|SPI_CPHA)
#define SPI_MODE_2              (SPI_CPOL|0)
#define SPI_MODE_3              (SPI_CPOL|SPI_CPHA)

#define SPI_CS_HIGH             0x04
#define SPI_LSB_FIRST           0x08
#define SPI_3WIRE               0x10

void initialize_addc() {

/*
 Currently the ADS1256 is hardware-configured with the following channels:
	0	VAL_MIR		(AIN0-AIN1, differential)
	1	VAL_DET		(AIN2-AIN3, differential)
	2	VAL_SURF	(AIN4-AIN5, differential)
	6	VAL_REF		(AIN6, single-ended)
	7	THERMISTOR	(AIN7, single-ended)
 Before addressing a channel, it is set differential or single-ended
 inside the function ADS1256_acq_se_diff(), which either calls 
 ADS1256_SetSingEndChannel() or ADS1256_SetDiffChannel().
 
 The program flow to drive the ADS1256 is as follows:	

 main() >> initialize_board() >> initialize_addc() >> ADS1256_InitScan()
 (last function initialize the data array g_tADS1256.AdcNow[])

 Then the program flow starts with function PI_THREAD(th_sensor)
 where in the endless loop while(1) the function ADS1256_Scan() is called.
 It calls function ADS1256_acq_se_diff() which gets the next sample of
 ONE CHANNEL and prepare variables for the next channel.
 Therefore, before getting ALL the N input samples, the endless loop
 must cycle N times because it must call ADS1256_acq_se_diff() N times.
 Then function ADS1256_GetAdc() returns the last g_tADS1256.AdcNow[] samples.
 To resume:
 PI_THREAD() >> ADS1256_Scan() >> ADS1256_acq_se_diff() fills g_tADS1256.AdcNow[]
 ADS1256_GetAdc() returns g_tADS1256.AdcNow[]
 
*/

	uint8_t id;

	pinMode(DRDY, INPUT);

	pinMode(RST, OUTPUT);
	pinMode(SPICS, OUTPUT);
	pinMode(SPICS1, OUTPUT);
	pinMode(PDWN, OUTPUT);
	pullUpDnControl(DRDY, PUD_UP);

	digitalWrite(PDWN, 1);

	//reset
	digitalWrite(RST, 0);
	delay(500);
	//activate
	digitalWrite(RST, 1);
	delay(500); //wait calibration time


	CS_1();
	CS1_1();

	if (!wiringPiSPISetupMode(0, 500000, SPI_MODE_1 | SPI_LSB_FIRST)) {
		log_info("Cannot start SPI ch 1");
		return -1;
	}

	id = ADS1256_ReadChipID();
	printf("ADS1256 ID= %d\n",id);
	if (id != 3)
	{
		log_error("addc chip id error");
		/*start simulator*/
		log_info("!!!!SIMULATOR MODE!!!!");
		piThreadCreate(th_sensor_simulator);

		delay(500); //fake read values
	}
	else
	{
		log_info("addc chip ok");

		//ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_30SPS); //ADS1256_15SPS
#if READ_FREQUENCY == 15
		ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_15SPS); //ADS1256_GAIN_1 +-2.5v
		delay(500); //wait gain time
#endif
#if READ_FREQUENCY == 30
		ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_30SPS); //ADS1256_15SPS
#endif

#ifdef SINGLE_ENDED
		ADS1256_InitScan(SINGLE_ENDED);	// ADS1256 is set for single ended inputs
#else	// is def DIFFERENTIAL
		ADS1256_InitScan(DIFFERENTIAL);	// ADS1256 is set for differential inputs
#endif


		addc_UpdateDAC(0);
		
		piThreadCreate(th_sensor);
	}
}

int32_t addc_read_uv(int ch) {
	if (ch < 0)ch = 0;
	if (ch >= 8) ch = 7;
	return volt[buffnum][ch];
}

int32_t addc_read_raw(int ch) {
	if (ch < 0)ch = 0;
	if (ch >= 8) ch = 7;
	return voltRaw[buffnum][ch];
}

double addc_read_v(int ch) {
	if (ch < 0)ch = 0;
	if (ch >= 8) ch = 7;
	return volt[buffnum][ch] / 1000000.0;
}

void  addc_test()
{
	uint8_t i;
	uint8_t ch_num;
	double iTemp;
	char allzero = 1;
	printf("V:\n");
		for (i = 0; i < 8; i++)
		{
			iTemp = addc_read_v(i);	/* uV  */
			if (iTemp < 0)
			{
				iTemp = -iTemp;
				printf("AD%d (-%f V) \r\n",i,iTemp);
			}
			else
			{
				printf("AD%d (%f V) \r\n", i, iTemp);
			}

			//check zero
			if (iTemp != 0) allzero = 0;

		}
		printf("RAW:\n");
		for (i = 0; i < 8; i++)
		{
			iTemp = addc_read_raw(i);	/* uV  */
			if (iTemp < 0)
			{
				iTemp = -iTemp;
				printf("AD%d (-%f RAW) \r\n", i, iTemp);
			}
			else
			{
				printf("AD%d (%f RAW) \r\n", i, iTemp);
			}

		}
		printf("uV:\n");
		for (i = 0; i < 8; i++)
		{
			iTemp = addc_read_uv(i);	/* uV  */
			if (iTemp < 0)
			{
				iTemp = -iTemp;
				printf("AD%d (-%f uV) \r\n", i, iTemp);
			}
			else
			{
				printf("AD%d (%f uV) \r\n", i, iTemp);
			}

		}

		if (allzero) {
			log_error("values from sensors is 0 check hardware\n");
		}
}