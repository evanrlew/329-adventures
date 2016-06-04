#pragma once

/* ZX Sensor register addresses */
#define ZX_STATUS           0x00
#define ZX_DRE              0x01
#define ZX_DRCFG            0x02
#define ZX_GESTURE          0x04
#define ZX_GSPEED           0x05
#define ZX_DCM              0x06
#define ZX_XPOS             0x08
#define ZX_ZPOS             0x0A
#define ZX_LRNG             0x0C
#define ZX_RRNG             0x0E
#define ZX_REGVER           0xFE
#define ZX_MODEL            0xFF

extern volatile uint16_t atk_time;

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