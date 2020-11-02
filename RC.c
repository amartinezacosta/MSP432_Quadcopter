#include "RC.h"
#include <ti/drivers/Capture.h>
#include "ti_drivers_config.h"

Capture_Handle capture_handle;

uint32_t index = 0;
uint32_t input_channels[8];

void capture_callback(Capture_Handle handle, uint32_t interval);

void PPM_init(void)
{
    Capture_Params params;

    Capture_Params_init(&params);
    params.mode = Capture_FALLING_EDGE;
    params.periodUnit = Capture_PERIOD_US;
    params.callbackFxn = capture_callback;

    capture_handle = Capture_open(CONFIG_CAPTURE_0, &params);

    Capture_start(capture_handle);
}

void PPM_channels(uint32_t *channels)
{
    uint32_t i;
    for(i = 0; i < 8; i++)
    {
        channels[i] = input_channels[i];
    }
}

void capture_callback(Capture_Handle handle, uint32_t interval)
{
    if((interval > 6500) || (index == 8))
    {
        index = 0;
        return;
    }

    input_channels[index++] = interval;
}

