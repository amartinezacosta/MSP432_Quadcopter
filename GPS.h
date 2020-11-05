#ifndef GPS_H_
#define GPS_H_

#include <stdint.h>

#define GPS_BUFFER_SIZE     256

void GPS_init(void);
void GPS_read(float *position, uint32_t *satellites);
float GPS_parse_degrees(char *data);
//Convert a string ddmm.mmmm to degress
float GPS_latitude_degrees(char *data);
//Convert a string dddmm.mmmm to degress
float GPS_longitude_degrees(char *data);

#endif /* GPS_H_ */
