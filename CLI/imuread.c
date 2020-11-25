#include "CLI.h"
#include "UARTDEBUG.h"

void imuread_help(void)
{
    UARTDEBUG_printf("Read IMU fused data using complimentary filters\r\n");
    UARTDEBUG_printf("Usage: imuread [options] samples ...\r\n");
    UARTDEBUG_printf("   options:\r\n");
    UARTDEBUG_printf("      -p \t Read pitch angle\r\n");
    UARTDEBUG_printf("      -r \t Read roll angle\r\n");
    UARTDEBUG_printf("      -y \t Read yaw angle\r\n");
    UARTDEBUG_printf("      -A \t Read pitch, roll and yaw angles\r\n");
}

void imuread_run_pitch_read(void)
{

}

void imuread_run_roll_calibration(void)
{

}

void imuread_run_yaw_calibration(void)
{

}

int imuread(char **argv, int argc)
{
    return CMD_OK;
}





