#include "CLI.h"
#include "UARTDEBUG.h"

#include <string.h>

extern int scal(char **argv, int argc);
extern int scheck(char **argv, int argc);
extern int sread(char **argv, int argc);
extern int imuread(char **argv, int argc);
extern int ledtest(char **argv, int argc);
extern int vbat(char **argv, int argc);
extern int rcread(char **argv, int argc);
extern int mcheck(char **argv, int argc);
extern int help(char **argv, int argc);

cmd_t cmd_table[] =
{
 {.cmd_str = "scal", .callback_fxn = scal},
 {.cmd_str = "scheck", .callback_fxn = scheck},
 {.cmd_str = "sread", .callback_fxn = sread},
 {.cmd_str = "imuread", .callback_fxn = imuread},
 {.cmd_str = "ledtest", .callback_fxn = ledtest},
 {.cmd_str = "vbat", .callback_fxn = vbat},
 {.cmd_str = "rcread", .callback_fxn = rcread},
 {.cmd_str = "mcheck", .callback_fxn = mcheck},
 {.cmd_str = "help", .callback_fxn = help}
};
uint32_t cmd_table_size = sizeof(cmd_table)/sizeof(cmd_t);

void cli(void)
{
    char *argv[10];
    int argc;
    int res;
    char buffer[128];

    UARTDEBUG_printf("\x1B[2J\x1B[H");
    UARTDEBUG_printf("\x1B[35m\r\n");
    UARTDEBUG_printf("========================================================\r\n");
    UARTDEBUG_printf("\tMSP432 Quadcopter CLI application\r\n");
    UARTDEBUG_printf("========================================================\r\n\r\n");
    UARTDEBUG_printf("\x1B[37m");
    while(1)
    {
        UARTDEBUG_printf("\x1B[32m>>\x1B[33m");
        if(UARTDEBUG_gets(buffer, 128, UARTDEBUG_WAIT_FOREVER, true))
        {
            if(strcmp(buffer, "quit") == 0)
            {
                break;
            }

            if(strcmp(buffer, "cls") == 0)
            {
                UARTDEBUG_printf("\x1B[2J\x1B[H");
                continue;
            }

            UARTDEBUG_printf("\x1B[39m");
            argc = cli_parse(buffer, argv);
            res = cli_run(argv, argc);

            cli_reply(argv, res);
        }
    }
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

void cli_reply(char **argv, int result)
{
    switch(result)
    {
    case CMD_OK:
        //UARTDEBUG_printf("CMD OK\r\n");
        break;
    case CMD_NOT_FOUND:
        UARTDEBUG_printf("\'%s\' is not recognized as valid command\r\n", argv[0]);
        break;
    default:
        break;
    }
}
