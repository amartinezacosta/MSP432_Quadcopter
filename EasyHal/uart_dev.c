#include "uart_dev.h"
#include "config_dev.h"

/*--------------------LOW LEVEL UART HARDWARE IMPLEMENTATION, DEVICE SPECIFIC--------------------*/
/* EUSCI_A0
 * P1.2 -> RX
 * P1.3 -> TX
 *
 * EUSCI_A2
 * P3.2 -> RX
 * P3.3 -> TX
 *
 */
#ifdef MSP432P401R_RTOS_UART

/* WARNING! DO NOT COMPILE THIS ON A DRA OR DRIVERLIB PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

bool uart_open[UART_COUNT] = {false, false};
const uint32_t uart_configs[UART_COUNT] = {CONFIG_UART_0, CONFIG_UART_1};
UART_Handle UART_handles[UART_COUNT];

void uart_dev_init(uint32_t index, uint32_t baudrate)
{
    if(uart_open[index]) return;

    UART_Params params;

    UART_Params_init(&params);
    params.baudRate = baudrate;
    params.readMode = UART_MODE_BLOCKING;
    params.writeMode = UART_MODE_BLOCKING;
    params.readTimeout = UART_WAIT_FOREVER;
    params.writeTimeout = UART_WAIT_FOREVER;
    params.readEcho = UART_ECHO_OFF;
    params.dataLength = UART_LEN_8;

    UART_handles[index] = UART_open(uart_configs[index], &params);
    if (UART_handles[index] == NULL)
    {
        while(1);
    }

    uart_open[index] = true;
}

void uart_dev_write(uint32_t index, uint8_t *data, uint32_t size)
{
    UART_Handle handle = UART_handles[index];
    UART_write(handle, data, size);
}

void uart_dev_read(uint32_t index, uint8_t *data, uint32_t size)
{
    UART_Handle handle = UART_handles[index];
    UART_read(handle, data, size);
}

#elif defined(MSP432P401R_DRIVERLIB_UART)

/*WARNING! DO NOT COMPILE THIS ON A DRA OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

bool uart_open[UART_COUNT] = {false, false};

const uint32_t eusci[UART_COUNT] = {EUSCI_A0, EUSCI_A2};
const uint32_t ports[UART_COUNT] = {GPIO_PORT_P1, GPIO_PORT_P3};
const uint32_t pins[UART_COUNT] = {GPIO_PIN2 | GPIO_PIN3, GPIO_PIN2 | GPIO_PIN3};

extern uint32_t SystemCoreClock;

eUSCI_UART_ConfigV1 uart_config[UART_COUNT] =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        0,                                      // will be changed later
        0,                                       // will be changed later
        0,                                      // will be changed later
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_MSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                  // 8 bit data length
};

void uart_dev_init(uint32_t index, uint32_t baudrate)
{
    if(uart_open[index]) return;

    uint32_t N = SMCLK_DEV_FREQUENCY/baudrate;
    uint32_t UCBR = N/16;
    uint32_t UCBRF = (uint32_t)( ((float)(N/16.0) - (uint32_t)(N/16))*16 );

    //TODO: Pointer to config struct? It could make the code look better
    //TODO: Find a better way to compute UCBRF and UCBRS values, THIS DOES NOT WORK!
    uart_config[index].clockPrescalar = UCBR;
    uart_config[index].firstModReg = (UCBRF << EUSCI_A_MCTLW_BRF_OFS);
    uart_config[index].secondModReg = 0;

    GPIO_setAsPeripheralModuleFunctionInputPin(ports[index], pins[index], GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(ports[index], &uart_config);
    UART_enableModule(ports[index]);

    uart_open[index] = true;
}

void uart_dev_write(uint32_t index, uint8_t *data, uint32_t size)
{
    uint32_t BASE = uart_bases[index];

    uint32_t i = 0;
    for(i = 0; i < size; i++)
    {
        UART_transmitData(BASE, data[i]);
    }
}

void uart_dev_read(uint32_t index, uint8_t *data, uint32_t size)
{
    uint32_t BASE = uart_bases[index];

    uint32_t i = 0;
    for(i = 0; i < size; i++)
    {
        data[i] = UART_receiveData(BASE);
    }
}

#elif defined(MSP432P401R_DRA_UART)

/*WARNING! DO NOT COMPILE THIS ON A DRIVERLIB OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */
#include "msp.h"

const uint32_t uart_bases[UART_COUNT] = {EUSCI_A0_BASE, EUSCI_A1_BASE};
const uint32_t uart_ports[UART_COUNT] = {DIO_BASE, DIO_BASE + 0x0020};
const uint8_t uart_pins[UART_COUNT] = {BIT2|BIT3, BIT2|BIT3};

void uart_dev_init(uint32_t index, uint32_t baudrate)
{
    DIO_PORT_Odd_Interruptable_Type *GPIO_BASE = (DIO_PORT_Odd_Interruptable_Type*)uart_ports[index];
    uint8_t pins = uart_pins[index];

    GPIO_BASE->SEL0 |= pins;
    GPIO_BASE->SEL1 &= ~pins;

    //TODO: Change this for a table?
    uint32_t N = SMCLK_DEV_FREQUENCY/baudrate;
    uint32_t UCBR = N/16;
    uint32_t UCBRF = (uint32_t)( ((float)(N/16.0) - (uint32_t)(N/16))*16 );

    EUSCI_A_Type *UART_BASE = (EUSCI_A_Type*)uart_bases[index];

    UART_BASE->CTLW0 |= EUSCI_A_CTLW0_SWRST;
    UART_BASE->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;
    UART_BASE->BRW = UCBR;
    UART_BASE->MCTLW = (UCBRF << EUSCI_A_MCTLW_BRF_OFS)| EUSCI_A_MCTLW_OS16;
    UART_BASE->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
}

void uart_dev_write(uint32_t index, uint8_t *data, uint32_t size)
{
    EUSCI_A_Type *UART_BASE = (EUSCI_A_Type*)uart_bases[index];

    uint32_t i;
    for(i = 0; i < size; i++)
    {
        while(!(UART_BASE->IFG & EUSCI_A_IFG_TXIFG));
        UART_BASE->TXBUF = data[i];
    }
}

void uart_dev_read(uint32_t index, uint8_t *data, uint32_t size)
{
    EUSCI_A_Type *UART_BASE = (EUSCI_A_Type*)uart_bases[index];

    uint32_t i;
    for(i = 0; i < size; i++)
    {
        while(!(UART_BASE->IFG & EUSCI_A_IFG_RXIFG));
        data[i] = UART_BASE->RXBUF;
    }
}

#else
#warning USING UN-IMPLEMENTED UART COMMUNICATION, YOU MUST PROVIDE YOUR OWN SPECIFIC UART INTERFACE

void uart_dev_init(uint32_t index, uint32_t baudrate)
{

}

void uart_dev_write(uint32_t index, uint8_t *data, uint32_t size)
{

}

void uart_dev_read(uint32_t index, uint8_t *data, uint32_t size)
{

}

#endif




