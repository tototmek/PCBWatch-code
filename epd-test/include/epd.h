
#ifndef EPD_H
#define EPD_H

#include <cstdint>
// #include "fonts.h"

// Display resolution
#define EPD_WIDTH       200
#define EPD_HEIGHT      200

// EPD1IN54 commands
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x18
#define TEMPERATURE_SENSOR_CONTROL2                 0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF

// Pins
#define CS_PIN 7
#define RST_PIN 2
#define DC_PIN 1
#define BUSY_PIN 3
#define SDI_PIN 6
#define SCL_PIN 4


extern const unsigned char lut_full_update[];
extern const unsigned char lut_partial_update[];

/* Hardware operating functions */
/* public functions */
int  EPD_Init();
void EPD_SendCommand(unsigned char command);
void EPD_SendData(unsigned char data);
void EPD_WaitUntilIdle();
void EPD_Reset();
void EPD_SetFrameMemory(
  const unsigned char* image_buffer,
  int x,
  int y,
  int image_width,
  int image_height
);
void EPD_ClearFrameMemory(unsigned char color);
void EPD_UpdateDisplay();
void EPD_Sleep();
// void EPD_DelayMs(EPD* epd, unsigned int delay_time);
// void EPD_DigitalWrite(EPD* epd, int pin, int value);
// int  EPD_DigitalRead(EPD* epd, int pin);

/* private functions */
static void EPD_SetLut(const unsigned char* lut);
static void EPD_SetMemoryArea(int x_start, int y_start, int x_end, int y_end);
static void EPD_SetMemoryPointer(int x, int y);

#endif /* EPD_H */
