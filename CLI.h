#ifndef CLI_H_
#define CLI_H_

#include <stdint.h>

#define CMD_OK           0
#define CMD_NOT_FOUND   -1

typedef int(*cli_callback)(char **argv, int argc);

typedef struct
{
    char *cmd_str;
    cli_callback callback_fxn;
}cmd_t;

void cli(void);
int cli_parse(char *line, char **argv);
int cli_run(char **argv, int argc);

#endif /* CLI_H_ */
