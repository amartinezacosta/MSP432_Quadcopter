#include "CLI.h"
#include "UARTDEBUG.h"

int help(char **argv, int argc)
{
    UARTDEBUG_printf("CLI MSP432 application to interact, test and configure sensors, motors and internal parameters. For more information type [command][-h]\r\n\r\nAvailable commands:\r\n\r\n");
    UARTDEBUG_printf("scal\t\t calibrate sensor\r\n");
    UARTDEBUG_printf("scheck\t\t check sensor status\r\n");
    UARTDEBUG_printf("sread\t\t read raw sensor data\r\n");
    UARTDEBUG_printf("imuread\t\t read IMU data\r\n");
    UARTDEBUG_printf("ledtest\t\t test indication LEDs\r\n");
    UARTDEBUG_printf("vbat\t\t read battery voltage\r\n");
    UARTDEBUG_printf("rcread\t\t read radio receiver channels\r\n");
    UARTDEBUG_printf("mcheck\t\t test motors\r\n");
    UARTDEBUG_printf("help\t\t print this help message\r\n");
    UARTDEBUG_printf("cls\t\t clear terminal screen\r\n");
    UARTDEBUG_printf("quit\t\t exit cli application\r\n");

    return CMD_OK;
}



