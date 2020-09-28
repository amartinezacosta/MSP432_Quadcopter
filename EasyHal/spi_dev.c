#include "spi_dev.h"
#include "config_dev.h"

/*--------------------LOW LEVEL I2C HARDWARE IMPLEMENTATION, DEVICE SPECIFIC--------------------*/
/* EUSCI_B0
 * P1.6 -> MOSI
 * P1.7 -> MISO
 * P1.5 -> SCLK
 * P6.1 -> CS
 */
#ifdef MSP432P401R_RTOS_SPI

/*WARNING! DO NOT COMPILE THIS ON A DRA OR DRIVERLIB PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include "ti_drivers_config.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>

SPI_Handle spi_handle;
bool spi_open = false;

void spi_dev_init(uint32_t spi_clock)
{
    if(spi_open) return;

    SPI_Params params;
    SPI_Params_init(&params);
    params.frameFormat = SPI_POL0_PHA0;
    params.bitRate = spi_clock;

    spi_handle = SPI_open(CONFIG_SPI_0, &params);
    if(spi_handle == NULL)
    {
        //Failed to initalize spi;
        while(1);
    }

    spi_open = true;
}

void spi_dev_write(uint8_t *data, uint32_t size)
{
    SPI_Transaction t;

    t.count = size;
    t.txBuf = data;
    t.rxBuf = NULL;

    SPI_transfer(spi_handle, &t);
}

void spi_dev_read(uint8_t *data, uint32_t size)
{
    SPI_Transaction t;

    t.count = size;
    t.txBuf = NULL;
    t.rxBuf = data;

    SPI_transfer(spi_handle, &t);
}

void spi_dev_cs(uint8_t state)
{
    GPIO_write(GPIO_SPI_CS, state);
}

#elif defined(MSP432P401R_DRIVERLIB_SPI)

/*WARNING! DO NOT COMPILE THIS ON A DRA OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */
#include "config_dev.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

bool spi_open = false;
eUSCI_SPI_MasterConfig spi_config =
{
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,                              // SMCLK Clock Source
        0,                                                          // Will be changed later
        0,                                                          // Will be changed later
        EUSCI_B_SPI_MSB_FIRST,                                      // MSB First
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,                  // High polarity
        EUSCI_B_SPI_3PIN                                            // 3Wire SPI Mode
};

void spi_dev_init(uint32_t spi_clock)
{
    if(spi_open) return;

    spi_config.clockSourceFrequency = SMCLK_DEV_FREQUENCY;
    spi_config.desiredSpiClock = spi_clock;

    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN1);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
    SPI_initMaster(EUSCI_B0_BASE, &spi_config);
    SPI_enableModule(EUSCI_B0_BASE);

    spi_open = true;
}

void spi_dev_write(uint8_t *data, uint32_t size)
{
    uint32_t i;
    for(i = 0; i < size; i++)
    {
        while (!(SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_SPI_TRANSMIT_INTERRUPT)));
        SPI_transmitData(EUSCI_B0_BASE, data[i]);
    }
}

void spi_dev_read(uint8_t *data, uint32_t size)
{
    uint32_t i;
    for(i = 0; i < size; i++)
    {
        while (!(SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_SPI_RECEIVE_INTERRUPT)));
        data[i] = SPI_receiveData(EUSCI_B0_BASE);
    }
}

void spi_dev_cs(uint8_t state)
{
    if(state == CS_HIGH)
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN1);
    }
    else if (state == CS_LOW)
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN1);
    }
}


#elif defined(MSP432P401R_DRA_SPI)

/*WARNING! DO NOT COMPILE THIS ON A DRIVERLIB OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */
#include "msp.h"

bool spi_open = false;

void spi_dev_init(uint32_t spi_clock)
{
    if(spi_open) return;

    //Setup CS pin
    P6->DIR |= BIT1;
    P6->OUT &= ~BIT1;

    P1->SEL0 |=  (BIT5 | BIT6 | BIT7);
    P1->SEL1 &= ~(BIT5 | BIT6 | BIT7);

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;         //EUSCI in reset to modify registers
    EUSCI_B0->CTLW0 |= (EUSCI_B_CTLW0_MST  |        //SPI master
                        EUSCI_B_CTLW0_SYNC |        //Synchronous mode
                        EUSCI_B_CTLW0_CKPL |        //Clock polarity high
                        EUSCI_B_CTLW0_MSB);         //MSB first

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;       //Select SMCLK, which runs at a default 3MHz frequency
    EUSCI_B0->BRW = SMCLK_DEV_FREQUENCY/spi_clock;          //Use user defined spi_clock frequency
    EUSCI_B0->CTLW0 &= ~(EUSCI_B_CTLW0_SWRST);

    spi_open = true;
}

void spi_dev_write(uint8_t *data, uint32_t size)
{
    uint32_t i;
    for(i = 0; i < size; i++)
    {
        while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
        EUSCI_B0->TXBUF = data[i];
    }
}

void spi_dev_read(uint8_t *data, uint32_t size)
{
    uint32_t i;
    for(i = 0; i < size; i++)
    {
        while (!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));
        data[i] = EUSCI_B0->RXBUF;
    }
}

void spi_dev_cs(uint8_t state)
{
    if(state == CS_HIGH)
    {
        P6->OUT |= BIT1;
    }
    else if(state == CS_LOW)
    {
        P6->OUT &= ~BIT1;
    }
}

#else
#warning USING UN-IMPLEMENTED SPI COMMUNICATION, YOU MUST PROVIDE YOUR OWN SPECIFIC SPI INTERFACE

void spi_dev_init(uint32_t spi_clock)
{

}

void spi_dev_write(uint8_t *data, uint32_t size)
{

}

void spi_dev_read(uint8_t *data, uint32_t size)
{

}

void spi_dev_cs(uint8_t state)
{

}

#endif
