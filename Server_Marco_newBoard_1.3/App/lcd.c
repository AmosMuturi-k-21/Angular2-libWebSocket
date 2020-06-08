#include "lcd.h"
#include "opt.h"

//#define LCD_DEBUG

static int lcddev = 0;
static int cx = 0;
static int cy = 0;
static const int rowOff[4] = { 128, 192, 148, 212 };

static int lcd_fifo;

//#define LCD_DELAY 400
//#define LCD_DEBUG
//#define LCD_DEBUG_PIPE

#define LCD_SIZE (LCD_ROW * LCD_COL)
static char lcd_screen[LCD_SIZE+1] = {0};
static char lcd_screen_w[LCD_SIZE+1] = {0};
static int lcd_screen_i_w = 0;
static char lcd_reprint = 0;
static char lcd_message_buff[10][LCD_SIZE] = { 0 };
static int lcd_message_i = 0;

static void i2cWriteByte(char data) {
	wiringPiI2CWrite(lcddev,data);
}
static char i2cReadByte() {
	return wiringPiI2CRead(lcddev);
}

static void lcd_strobe(unsigned char data)
{
	i2cWriteByte(data | En | LCD_BACKLIGHT);
	delayMicroseconds(1000); //500
	i2cWriteByte(((data & ~En) | LCD_BACKLIGHT));
	delayMicroseconds(1000); //100
}

static void write4bits(unsigned char data)
{
	i2cWriteByte(data | LCD_BACKLIGHT);
	lcd_strobe(data);
}

void writeCommand(unsigned char cmd)
{
	write4bits(cmd & 0xF0);
	write4bits((cmd << 4) & 0xF0);
}

static writeChar(char cmd)
{
	write4bits(Rs | (cmd & 0xF0));
	write4bits(Rs | ((cmd << 4) & 0xF0));
}

static void _lcd_newline() {
	if (++cy == LCD_ROW)
		cy = 0;
	writeCommand(rowOff[cy]);

#ifdef LCD_DEBUG
	printf("\n");
#endif
#ifdef LCD_DEBUG_PIPE
	char data = '\n';
	write(lcd_fifo, &data, 1);
#endif
}

static void _lcd_putchar(unsigned char data)
{
	if (data == '\r' || data == 0) return; //skip
	if (data == '\n') {
		//_lcd_newline();
		return;
	}

	writeChar(data);
#ifdef LCD_DEBUG
	printf("%c",data);
#endif
#ifdef LCD_DEBUG_PIPE
	write(lcd_fifo,&data,1);
#endif
	if (++cx == LCD_COL)
	{
		cx = 0;
		_lcd_newline();
	}
}

static void _lcd_putstr(const char *string)
{
	while (*string)
		_lcd_putchar(*string++);
}

static void _lcd_clear() {
	//writeCommand(LCD_CLEAR);
	writeCommand(rowOff[0]);
	cy = 0;
	cx = 0;
}

//memcmp not supported by valgrind. BUG XXXX

int _memcmp(char * a,char * b,int siz) {
	while ((siz--) && (a++[0] == b++[0])) { };
	if (siz == -1) return 0;
	//i don't care -1/1
	return 1;
}

static PI_THREAD(th_lcd_manager)
{
	while (1) {
		if (lcd_reprint && (_memcmp(lcd_screen_w, lcd_screen, LCD_SIZE) != 0)) {
			//int siz = lcd_screen_i_w;
			/*if (siz >= LCD_SIZE) */
			int siz = LCD_SIZE;
			_lcd_clear();
			memcpy(lcd_screen, lcd_screen_w, LCD_SIZE);
			lcd_reprint = 0;
			for (int i = 0; i < siz; i++) {
				_lcd_putchar(lcd_screen[i]);
			}

			//debug
			//fflush(stdout);
#ifdef LCD_DEBUG
			printf("\n");
#endif
#ifdef LCD_DEBUG_PIPE
			char data = '\n';
			write(lcd_fifo, &data, 1);
#endif
			delay(opt.LCD_REFRESH);
			//while ((c = lcd_read(0)) & (1 << LCD_BUSY)) {}
		}
		else lcd_reprint = 0;
#ifndef LCD_SYNC
		delay(50);
#else
		delay(1);
#endif
	}
}

int lcd_working() {
	return lcddev>=0;
}

void initialize_lcd() {

#ifdef LCD_DEBUG_PIPE
	char * myfifo = "/tmp/app.lcd";
	mkfifo(myfifo, 7777);
	lcd_fifo = open(myfifo, O_WRONLY);
#endif

	lcddev = wiringPiI2CSetup(opt.LCD_ID);
	if (lcddev < 0) {
		log_error("cannot initialize lcd");
		return;
	}

	printf("LCD: init with device id [%d] refresh [%d]\n", opt.LCD_ID,opt.LCD_REFRESH);

	// setup 4-bit mode
	write4bits(LCD_FUNC | LCD_8BITMODE);
	delayMicroseconds(4100); // 4.1ms
	write4bits(LCD_FUNC | LCD_8BITMODE);
	delayMicroseconds(100); // 100us
	write4bits(LCD_FUNC | LCD_8BITMODE);
	write4bits(LCD_FUNC | LCD_4BITMODE);


	//writeCommand(LCD_FUNC | LCD_4BITMODE | LCD_5x8DOTS | (LCD_ROW > 1 ? LCD_2LINE : LCD_1LINE));
	writeCommand(LCD_FUNC | LCD_4BITMODE | LCD_5x8DOTS | LCD_2LINE);
	
	// Rest of the initialisation sequence
	writeCommand(LCD_CTRL);
	writeCommand(LCD_CLEAR);
	writeCommand(LCD_ENTRY | LCD_ENTRYLEFT);
	writeCommand(LCD_CDSHIFT | LCD_CDSHIFT_RL);

	// Display ON
	writeCommand(LCD_CTRL | LCD_DISPLAY_CTRL);

	delayMicroseconds(100000); // 100ms

	piThreadCreate(th_lcd_manager);
}


void lcd_clear() {
	lcd_screen_i_w = 0;
	memset(lcd_screen_w, ' ', LCD_SIZE);
	lcd_reprint = 1;
}

void lcd_putchar(unsigned char data)
{
	if (lcd_reprint)
		lcd_reprint = 0; //disable reprint
	if (data == '\n') {
		lcd_reprint = 1;
		return;
	}
	if (lcd_screen_i_w < LCD_SIZE)
	lcd_screen_w[lcd_screen_i_w++] = data;
}

void lcd_putstr(const char *string)
{
	while (*string)
		lcd_putchar(*string++);
}

void lcd_fflush() {
	lcd_reprint = 1;
#ifdef LCD_SYNC
	while (lcd_reprint && (memcmp(lcd_screen_w, lcd_screen, LCD_SIZE) != 0)) { delay(1); };
#endif
}

void lcd_setline(unsigned char data) {
	if (data >= LCD_ROW) data = LCD_ROW - 1;
	lcd_screen_i_w = LCD_COL * data;
}

void lcd_setcursor(unsigned char data) {
	if (data >= LCD_SIZE) data = LCD_SIZE - 1;
	lcd_screen_i_w = data;
}
