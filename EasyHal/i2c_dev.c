#include "i2c_dev.h"
#include "config_dev.h"

/*--------------------LOW LEVEL I2C HARDWARE IMPLEMENTATION, DEVICE SPECIFIC--------------------*/
/* EUSCI_B1
 * P6.4 -> SDA
 * P6.5 -> SCL
 */
#ifdef MSP432P401R_RTOS_I2C

/*WARNING! DO NOT COMPILE THIS ON A DRA OR DRIVERLIB PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include <unistd.h>
#include <ti/drivers/I2C.h>
#include "ti_drivers_config.h"

I2C_Handle MPU9250_handle;
bool i2c_initialized = false;

void i2c_dev_init(uint32_t i2c_clock)
{
    if(i2c_initialized) return;

    I2C_Params params;
    I2C_Params_init(&params);

    switch(i2c_clock)
    {
    case I2C_DEV_100KHZ:
        params.bitRate = I2C_100kHz;
        break;
    case I2C_DEV_400KHZ:
        params.bitRate = I2C_400kHz;
        break;
    case I2C_DEV_1MHZ:
        params.bitRate = I2C_1000kHz;
        break;
    }

    MPU9250_handle = I2C_open(CONFIG_I2C_0, &params);

    if(MPU9250_handle == 0)
    {
        //Failed to open I2C
        while(1);
    }

    i2c_initialized = true;
}

bool i2c_dev_write(uint8_t slave_address, uint8_t *data, uint32_t size)
{
    I2C_Transaction t = {0};

    t.slaveAddress = slave_address;
    t.writeBuf = data;
    t.writeCount = size;
    t.readBuf = NULL;
    t.readCount = 0;

    return I2C_transfer(MPU9250_handle, &t);
}

bool i2c_dev_read(uint8_t slave_address, uint8_t reg, uint8_t *data, uint32_t size)
{
    I2C_Transaction t = {0};

    t.slaveAddress = slave_address;
    t.readBuf = data;
    t.readCount = size;
    t.writeBuf = &reg;
    t.writeCount = 1;

    return I2C_transfer(MPU9250_handle, &t);
}

#elif defined(MSP432P401R_DRIVERLIB_I2C)

/*WARNING! DO NOT COMPILE THIS ON A DRA OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

bool i2c_initialized = false;

eUSCI_I2C_MasterConfig i2c_config =
{
    EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
    0,                                      // Will be modified later
    0,                                      // Will be modified later
    0,                                      // No byte counter threshold
    EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void i2c_dev_init(uint32_t i2c_clock)
{
    if(i2c_initialized) return;

    i2c_config.i2cClk = SMCLK_DEV_FREQUENCY;
    i2c_config.dataRate = i2c_clock;

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
    I2C_initMaster(EUSCI_B1_BASE, &i2c_config);
    I2C_enableModule(EUSCI_B1_BASE);

    i2c_initialized = true;
}

bool i2c_dev_write(uint8_t slave_adddress, uint8_t *data, uint32_t size)
{
    //Wait for last transaction to finish
    while (I2C_masterIsStopSent(EUSCI_B1_BASE));
    //Set slave address
    I2C_setSlaveAddress(EUSCI_B1_BASE, slave_adddress);

    //Send start condition
    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, data[0]);

    uint32_t i;
    for(i = 1; i < size - 1; i++)
    {
        //Poll for data to be sent
        I2C_masterSendMultiByteNext(EUSCI_B1_BASE, data[i]);
    }

    //Send stop condition
    I2C_masterSendMultiByteFinish(EUSCI_B1_BASE, data[i]);

    return true;
}

bool i2c_dev_read(uint8_t slave_adddress, uint8_t reg, uint8_t *data, uint32_t size)
{
    //Set slave address
    I2C_setSlaveAddress(EUSCI_B1_BASE, slave_adddress);
    //Send start condition
    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, reg);
    //Poll for byte sent
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));

    //Send start and receive condition
    I2C_masterReceiveStart(EUSCI_B1_BASE);

    //Poll for start condition
    while(I2C_masterIsStartSent(EUSCI_B1_BASE));

    uint32_t i;
    for(i = 0; i < size - 1; i++)
    {
        //Poll for receive data
        data[i] = I2C_masterReceiveSingle(EUSCI_B1_BASE);
    }

    //Send stop condition and receive last byte
    data[i] = I2C_masterReceiveMultiByteFinish(EUSCI_B1_BASE);

    return true;
}

#elif defined(MSP432P401R_DRA_I2C)

#include "msp.h"

/*WARNING! DO NOT COMPILE THIS ON A DRIVERLIB OR RTOS PROJECT!
 * COMPILATION WILL MOST LIKELY FAIL!
 */

//TODO: Add a timeout for the I2C connection? It should return true or false if the
//data was sent/received
bool i2c_initialized = false;

void i2c_dev_init(uint32_t i2c_clock)
{
    if(i2c_initialized) return;

    P6->SEL0 |= (BIT4|BIT5);
    P6->SEL1 &= ~(BIT5|BIT5);

    EUSCI_B1->CTLW0 |= EUSCI_A_CTLW0_SWRST;
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_MODE_3 |       //I2C mode
                       EUSCI_B_CTLW0_MST    |       //Master mode
                       EUSCI_B_CTLW0_SYNC   |       //Sync mode
                       EUSCI_B_CTLW0_SSEL__SMCLK;   //SMCLK

    EUSCI_B1->BRW = SMCLK_DEV_FREQUENCY/i2c_clock;

    EUSCI_B1->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;

    i2c_initialized = true;
}

 /*| S | Slave Address | R/W | ACK | Data | ACK | S | Slave Address | ACK Data | ACK | P | */
bool i2c_dev_write(uint8_t slave_address, uint8_t *data, uint32_t size)
{
    /* Making sure the last transaction has been completely sent out */
    while(EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTP);

    EUSCI_B1->I2CSA = slave_address;

    //Sent start condition
    EUSCI_B1->CTLW0 |=  EUSCI_B_CTLW0_TR |   //I2C transmit
                        EUSCI_B_CTLW0_TXSTT; //Start condition

    //Poll for transmit interrupt flag and start condition flag.
    while((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTT) || !(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));

    //Send first byte
    EUSCI_B1->TXBUF = data[0];

    uint32_t i;
    for(i = 1; i < size; i++)
    {
        //Poll for TX flag
        while(!(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));
        EUSCI_B1->TXBUF = data[i];
    }

    //Poll for last byte sent
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));

    //Send stop condition
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    return true;
}

bool i2c_dev_read(uint8_t slave_address, uint8_t reg,  uint8_t *data, uint32_t size)
{
    EUSCI_B1->I2CSA = slave_address;

    /*SEND REGISTER BYTE*/
    //Sent start condition
    EUSCI_B1->CTLW0 |=  EUSCI_B_CTLW0_TR |   //I2C transmit
                        EUSCI_B_CTLW0_TXSTT; //Start condition

    //Poll for transmit interrupt flag and start condition flag
    while((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTT) || !(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));
    //Send register value
    EUSCI_B1->TXBUF = reg;
    //Poll for transmission
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0));

    /*START RECEIVE DATA*/
    EUSCI_B1->CTLW0 &=  ~EUSCI_B_CTLW0_TR;  //I2C receive
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTT; //Start condition

    //Poll for start condition flag
    while(EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTT);

    uint32_t i;
    for(i = 0; i < size - 1; i++)
    {
        while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
        data[i] = EUSCI_B1->RXBUF & EUSCI_B_RXBUF_RXBUF_MASK;
    }

    //Send stop condition
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    //Poll stop condition
    while(EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTP);
    //Poll RX flag
    while(!(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0));
    //Read last data byte
    data[i] = EUSCI_B1->RXBUF & EUSCI_B_RXBUF_RXBUF_MASK;

    return true;
}

#else
#warning USING UN-IMPLEMENTED I2C COMMUNICATION, YOU MUST PROVIDE YOUR OWN SPECIFIC I2C INTERFACE
void i2c_init(void)
{

}

void i2c_write(uint8_t slave_adddress, uint8_t *data, uint32_t size)
{

}

void i2c_read(uint8_t slave_adddress, uint8_t *data, uint32_t size)
{

}

#endif




