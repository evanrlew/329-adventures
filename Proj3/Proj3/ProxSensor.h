#pragma once

/* ZX Sensor register addresses */
#define ZX_STATUS           0x00
#define ZX_GESTURE          0x04

typedef enum ProxStatus{
	NONE, GEST_AVAIL, HOVER_AVAIL, HOVER_MV_AVAIL, POS_AVAIL
} ProxStatus;


/* Enumeration for possible gestures */
typedef enum GestureType {
	RIGHT_SWIPE = 0x01,
	LEFT_SWIPE = 0x02,
	UP_SWIPE = 0x03,
	NO_GESTURE = 0xFF
} GestureType;

void init_sensor();
void monitor_sensor();
void write_prox_sensor(uint8_t regAdd, uint8_t data);
int read_prox_sensor(uint8_t regAdd);
ProxStatus data_available();
GestureType readGesture();