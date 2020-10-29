#ifndef EASYHAL_CONFIG_DEV_H_
#define EASYHAL_CONFIG_DEV_H_

/*
 * SMCLK frequency
 * WARNING! You must specify this value carefully, all EasyHAL hardware
 * depends on the correct clock frequency for SMCLK.
 */
#define SMCLK_DEV_FREQUENCY     12000000


/*ADC EasyHal layer selection--------------------*/
//#define MSP432P401R_RTOS_ADC
//#define MSP432P401R_DRIVERLIB_ADC
//#define MSP432P401R_DRA_ADC

/*I2C EasyHal layer selection--------------------*/
#define MSP432P401R_RTOS_I2C
//#define MSP432P401R_DRIVERLIB_I2C
//#define MSP432P401R_DRA_I2C

/*PWM EasyHal layer selection--------------------*/
#define MSP432P401R_RTOS_PWM
//#define MSP432P401R_DRIVERLIB_PWM
//#define MSP432P401R_DRA_PWM

/*SPI EasyHal layer selection--------------------*/
//#define MSP432P401R_RTOS_SPI
//#define MSP432P401R_DRIVERLIB_SPI
//#define MSP432P401R_DRA_SPI

/*Time EasyHal layer selection--------------------*/
#define MSP432P401R_RTOS_TIME
//#define MSP432P401R_DRIVERLIB_TIME
//#define MSP432P401R_DRA_TIME

/*UART EasyHal layer selection--------------------*/
#define MSP432P401R_RTOS_UART
//#define MSP432P401R_DRIVERLIB_UART
//#define MSP432P401R_DRA_UART

#endif /* EASYHAL_CONFIG_DEV_H_ */
