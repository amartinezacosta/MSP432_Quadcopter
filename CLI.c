#include "CLI.h"

#include "UARTDEBUG.h"

#include <string.h>

extern int cli_sensor_calibrate(char **argv, int argc);
extern int cli_sensor_check(char **argv, int argc);
extern int cli_sensor_read(char **argv, int argc);
extern int cli_sensor_calibrate_read(char **argv, int argc);
extern int cli_imu_read(char **argv, int argc);
extern int cli_test_led(char **argv, int argc);
extern int cli_vbat_read(char **argv, int argc);
extern int cli_rc_read(char **argv, int argc);
extern int cli_motor_check(char **argv, int argc);

cmd_t cmd_table[] =
{
 {.cmd_str = "scal", .callback_fxn = cli_sensor_calibrate},
 {.cmd_str = "scheck", .callback_fxn = cli_sensor_check},
 {.cmd_str = "sread", .callback_fxn = cli_sensor_read},
 {.cmd_str = "scalread", .callback_fxn = cli_sensor_calibrate_read},
 {.cmd_str = "imuread", .callback_fxn = cli_imu_read},
 {.cmd_str = "ledtest", .callback_fxn = cli_test_led},
 {.cmd_str = "vbat", .callback_fxn = cli_vbat_read},
 {.cmd_str = "rcread", .callback_fxn = cli_rc_read},
 {.cmd_str = "mcheck", .callback_fxn = cli_motor_check}
};
uint32_t cmd_table_size = sizeof(cmd_table)/sizeof(cmd_t);

void cli(void)
{
    char *argv[10];
    int argc;
    int res;
    char buffer[128];

    UARTDEBUG_printf("---------MSP432 Quadcopter CLI application---------\r\n");
    while(1)
    {
        if(UARTDEBUG_gets(buffer, 128, UARTDEBUG_WAIT_FOREVER, true))
        {
            if(strcmp(buffer, "quit") == 0)
            {
                break;
            }

            argc = cli_parse(buffer, argv);
            res = cli_run(argv, argc);
        }
    }
    UARTDEBUG_printf("---------Exiting MSP432 Quadcopter CLI application---------\r\n");
}

int cli_parse(char *line, char **argv)
{
    int token_count = 0;

    argv[token_count++] = strtok(line, " ");

    while(argv[token_count-1] != NULL)
    {
        argv[token_count++] = strtok(NULL, " ");
    }

    return token_count - 1;
}

int cli_run(char **argv, int argc)
{
    uint32_t i;
    for(i = 0; i < cmd_table_size; i++)
    {
        if(strcmp(argv[0], cmd_table[i].cmd_str) == 0)
        {
            return cmd_table[i].callback_fxn(argv+1, argc-1);
        }
    }

    return CMD_NOT_FOUND;
}
