#ifndef GPS_H_
#define GPS_H_

#include <stdint.h>

void GPS_init(void);
uint32_t GPS_read(char *gps_data, uint32_t size);

#endif /* GPS_H_ */
