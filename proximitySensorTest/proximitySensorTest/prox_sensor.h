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

#define GEST_AVAIL 1
#define POS_AVAIL 2

/* Enumeration for possible gestures */
typedef enum GestureType {
	RIGHT_SWIPE = 0x01,
	LEFT_SWIPE = 0x02,
	UP_SWIPE = 0x03,
	NO_GESTURE = 0xFF
} GestureType;

void write_prox_sensor(uint8_t regAdd, uint8_t data);
int read_prox_sensor(uint8_t regAdd);
int data_available();
GestureType readGesture();