#include "boot.h"

/*
const fboot = "/etc/rc.local";

const filupdate = "/boot/rc.local";
const filui = "/boot/xinitrc";

const fenable = "/home/pi/rc.local_yes";
const fenableui = "/home/pi/xinitrc";

const fenableapp = "rc.local";
*/


int boot_at_startup(char boolean) {
	FILE *temp;
	switch (boolean)
	{
	case 0: //disabled
		system("sudo rm /boot/rc.local");
		system("sudo rm /home/pi/.config/lxsession/LXDE-pi/autostart");
		break;

	case 1: //no ui
		system("sudo rm /home/pi/.config/lxsession/LXDE-pi/autostart");
		system("sudo rm /boot/rc.local");
		system("sudo rm /etc/rc.local");

		system("sudo cp /home/pi/rc.local /etc/rc.local");
		system("sudo cp /home/pi/rc.local_yes /boot/rc.local");

		system("sudo chmod +x /boot/rc.local");
		system("sudo chmod +x /etc/rc.local");
		break;

	case 2: //ui
		system("sudo rm /home/pi/.config/lxsession/LXDE-pi/autostart");
		system("sudo rm /boot/rc.local");
		system("sudo rm /etc/rc.local");

		system("sudo cp /home/pi/rc.local /etc/rc.local");
		system("sudo cp /home/pi/rc.local_yes /boot/rc.local");
		system("sudo cp /home/pi/xinitrc /home/pi/.config/lxsession/LXDE-pi/autostart");

		system("sudo chmod +x /boot/rc.local");
		system("sudo chmod +x /etc/rc.local");
		system("sudo chmod +x /home/pi/.config/lxsession/LXDE-pi/autostart");
		system("sudo chmod +x /home/pi/start_touch");
		break;

	default:
		log_error("unknow option");
		break;
	}
}