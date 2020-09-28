#include "adc_dev.h"
#include "config_dev.h"

/*--------------------LOW LEVEL PWM HARDWARE IMPLEMENTATION, DEVICE SPECIFIC--------------------*/
/* ADC
 * P5.5 -> A0
 * P5.4 -> A1
 * P5.2 -> A3
 * P5.1 -> A4
 */
#ifdef MSP432P401R_RTOS_ADC

/*WARNING! DO NOT COMPILE THIS ON A DRA OR DRIVERLIB PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include "ti_drivers_config.h"
#include <ti/drivers/ADC.h>

ADC_Handle adc_handles[ADC_COUNT];
bool  adc_open[ADC_COUNT] = {false, false, false, false};
uint32_t adc_configs[ADC_COUNT] = {CONFIG_ADC_0, CONFIG_ADC_1, CONFIG_ADC_2, CONFIG_ADC_3};

void adc_dev_init(uint32_t index)
{
    if(adc_open[index]) return;

    ADC_Params params;
    uint32_t config = adc_configs[index];
    ADC_Handle *handle = &adc_handles[index];

    ADC_Params_init(&params);
    *handle = ADC_open(config, &params);

    if(*handle == NULL)
    {
        //Failed to initialize ADC module
        while(1);
    }

    adc_open[index] = true;
}

uint16_t adc_dev_read(uint32_t index)
{
    uint16_t value;
    ADC_Handle handle = adc_handles[index];
    ADC_convert(handle, &value);

    return value;
}

#elif defined(MSP432P401R_DRIVERLIB_ADC)

/*WARNING! DO NOT COMPILE THIS ON A DRA OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

bool adc_module_open = false;
bool adc_channel_open[ADC_COUNT] = {false, false, false, false};
const uint32_t adc_ports[ADC_COUNT] = {GPIO_PORT_P5, GPIO_PORT_P5, GPIO_PORT_P5};
const uint8_t adc_pins[ADC_COUNT] = {GPIO_PIN5, GPIO_PIN4, GPIO_PIN2, GPIO_PIN1};
const uint32_t adc_mems[ADC_COUNT] = {ADC_MEM0, ADC_MEM1, ADC_MEM3, ADC_MEM4};
const uint32_t adc_channels[ADC_COUNT] = {ADC_INPUT_A0, ADC_INPUT_A1, ADC_INPUT_A3, ADC_INPUT_A4};

void adc_dev_init(uint32_t index)
{
    if(adc_module_open == false)
    {
        ADC14_enableModule();
        ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);
        ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
        adc_module_open = true;
    }

    if(adc_channel_open) return;

    uint32_t port = adc_ports[index];
    uint8_t pin = adc_pins[index];
    uint32_t mem = adc_mems[index];
    uint32_t channel = adc_channels[index];

    GPIO_setAsPeripheralModuleFunctionInputPin(port, pin, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    ADC14_configureSingleSampleMode(mem, true);
    ADC14_configureConversionMemory(mem, ADC_VREFPOS_AVCC_VREFNEG_VSS, channel, false);

    adc_channel_open[index] = true;
}

uint16_t adc_dev_read(uint32_t index)
{
    uint16_t value;
    uint32_t mem = adc_mems[index];
    uint32_t channel = adc_channels[index];

    ADC14_disableConversion();
    ADC14_configureSingleSampleMode(mem, true);
    ADC14_configureConversionMemory(mem, ADC_VREFPOS_AVCC_VREFNEG_VSS, channel, false);

    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    //Wait for conversion to finish
    while(ADC14_isBusy());

    return ADC14_getResult(mem);
}

#elif defined(MSP432P401R_DRA_ADC)

/*WARNING! DO NOT COMPILE THIS ON A DRIVERLIB OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */
#include "msp.h"

bool adc_module_open = false;
bool adc_channel_open[ADC_COUNT] = {false, false, false, false};
const uint32_t adc_ports[ADC_COUNT] = {DIO_BASE+0x0040, DIO_BASE+0x0040, DIO_BASE+0x0040, DIO_BASE+0x0040};
const uint32_t adc_channels[ADC_COUNT] = {ADC14_MCTLN_INCH_0, ADC14_MCTLN_INCH_1, ADC14_MCTLN_INCH_3, ADC14_MCTLN_INCH_4};
const uint8_t adc_pins[ADC_COUNT] = {BIT5, BIT4, BIT2, BIT1};

void adc_dev_init(uint32_t index)
{
    if(adc_module_open == false)
    {
        // Sampling time, S&H=16, ADC14 on
        ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
        // Use sampling timer, 14-bit conversion results
        ADC14->CTL1 = ADC14_CTL1_RES_3;
        adc_module_open = true;
    }

    if(adc_channel_open[index]) return;

    uint32_t pin = adc_pins[index];
    uint32_t port = adc_ports[index];
    uint32_t channel = adc_channels[index];

    DIO_PORT_Odd_Interruptable_Type *GPIO_BASE = (DIO_PORT_Odd_Interruptable_Type*)port;

    GPIO_BASE->DIR  &= ~pin;
    GPIO_BASE->SEL0 |=  pin;
    GPIO_BASE->SEL1 |=  pin;

    ADC14->MCTL[channel] |= channel;

    adc_channel_open[index] = true;
}

uint16_t adc_dev_read(uint32_t index)
{
    uint32_t channel = adc_channels[index];

    //Disable conversion
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;
    //Change input channel to ADC memory address
    ADC14->CTL1 = ADC14_CTL1_RES_3 | (channel << ADC14_CTL1_CSTARTADD_OFS);
    //Enable and start conversion
    ADC14->CTL0 |= (ADC14_CTL0_ENC | ADC14_CTL0_SC);

    //Wait for conversion to finish
    while(ADC14->CTL0 & ADC14_CTL0_BUSY);

    return ADC14->MEM[channel];
}


#else
#warning USING UN-IMPLEMENTED ADC, YOU MUST PROVIDE YOUR OWN SPECIFIC ADC INTERFACE

void adc_dev_init(uint32_t index)
{

}

uint16_t adc_dev_read(uint32_t index)
{
    return 0;
}

#endif





