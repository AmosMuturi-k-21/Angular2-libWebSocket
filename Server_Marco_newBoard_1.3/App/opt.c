#include "opt.h"

static FILE *fp;
static FILE *temp;
static const file_name = "/home/pi/opt.bin";
struct Options opt;
static struct Options oldopt;

char OPT_SAVE_MUTEX = 0;

static void reset_opt() {
	memset(&opt, 0, sizeof(struct Options));
	memset(&oldopt, 0, sizeof(struct Options));

	opt.SLOPE_TEMP_MIRROR = 1;
	opt.SLOPE_TEMP_SURF = 1;
	//opt.SLOPE_CALIBRATION = 1;

	opt.BIP_ON_BUTTON = 1;

	opt.FAN_SPEED = 100;

	opt.BUZZ_FREQ = 3000;
	opt.LCD_REFRESH = 500;

	opt.LCD_ID = 0x37;

	opt.SLOPE_AW = 1;
	opt.DELTA_AW = 0;

	opt.SAMPLE_TEMP = 25; //default 25 laboratory
}

void default_opt() {
	reset_opt();
	sync_opt();
}

void initialize_opt() {
	if (fp > 0) return; //already started

	fp = fopen(file_name, "rb+");

	if (fp <= 0) {
		temp = fopen(file_name, "wb+");
		if (temp != 0) {
			log_info("init opt.bin to default: no file found");
			default_opt();
			fclose(temp);
			temp = 0;
		}
		fp = fopen(file_name, "rb+");
		if (fp<=0)
		log_error("file open opt.bin");
		return;
	}

	fseek(fp, 0L, SEEK_END);
	if (ftell(fp) != sizeof(opt)) {
		printf("%d != %d\n", ftell(fp), sizeof(opt));
		log_info("init opt.bin to default: wrong file size");
		fclose(fp);
		temp = fopen(file_name, "wb+");
		if (temp != 0) {
			log_info("writing opt.bin to default");
			default_opt();
			fclose(temp);
			temp = 0;
		}
		fp = fopen(file_name, "rb+");
		if (fp <= 0)
			log_error("file open opt.bin");
		return;
	}

	fseek(fp, 0L, SEEK_SET);

	//load options
	log_info("loading options");
	fread(&opt, 1, sizeof(opt), fp);

	fseek(fp, 0L, SEEK_SET);
}

void sync_opt() {
	if (memcmp(&oldopt, &opt, sizeof(oldopt)) != 0) {
		if (fp > 0) {
			fseek(fp, 0L, SEEK_SET);
			fwrite(&opt, 1, sizeof(opt), fp);
			fflush(fp);
			fseek(fp, 0L, SEEK_SET);
			memcpy(&oldopt, &opt, sizeof(oldopt));
			log_info("options flushed fp");
		}else
			if (temp > 0) {
				fseek(temp, 0L, SEEK_SET);
				fwrite(&opt, 1, sizeof(opt), temp);
				fflush(temp);
				fseek(temp, 0L, SEEK_SET);
				memcpy(&oldopt, &opt, sizeof(oldopt));
				log_info("options flushed temp");
			}
	}
}

