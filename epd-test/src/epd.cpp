#include <stdlib.h>
#include "epd.h"
#include "Arduino.h"
#include "SPI.h"

int EPD_Init(EPD* epd, const unsigned char* lut) {
  epd->width = EPD_WIDTH;
  epd->height = EPD_HEIGHT;
  /* this calls the peripheral hardware interface, see epdif */
  epd->lut = lut;
  /* EPD hardware init start */

  pinMode(CS_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  pinMode(DC_PIN, OUTPUT);
  pinMode(BUSY_PIN, INPUT);
  SPI.begin(SCL_PIN, 20, SDI_PIN, CS_PIN);

  EPD_Reset(epd);
  EPD_SendCommand(epd, DRIVER_OUTPUT_CONTROL);
  EPD_SendData(epd, (EPD_HEIGHT - 1) & 0xFF);
  EPD_SendData(epd, ((EPD_HEIGHT - 1) >> 8) & 0xFF);
  EPD_SendData(epd, 0x00);                     // GD = 0; SM = 0; TB = 0;
  EPD_SendCommand(epd, BORDER_WAVEFORM_CONTROL);
  EPD_SendData(epd, 0b01010000); // Black border (VSH1)
  EPD_SendCommand(epd, TEMPERATURE_SENSOR_CONTROL);
  EPD_SendData(epd, 0x80);

  // EPD_SendCommand(epd, BOOSTER_SOFT_START_CONTROL);
  // EPD_SendData(epd, 0xD7);
  // EPD_SendData(epd, 0xD6);
  // EPD_SendData(epd, 0x9D);
  // EPD_SendCommand(epd, WRITE_VCOM_REGISTER);
  // EPD_SendData(epd, 0xA8);                     // VCOM 7C
  // EPD_SendCommand(epd, SET_DUMMY_LINE_PERIOD);
  // EPD_SendData(epd, 0x1A);                     // 4 dummy lines per gate
  // EPD_SendCommand(epd, SET_GATE_TIME);
  // EPD_SendData(epd, 0x08);                     // 2us per line
  EPD_SendCommand(epd, DATA_ENTRY_MODE_SETTING);
  EPD_SendData(epd, 0x03);                     // X increment; Y increment
  // EPD_SetLut(epd, epd->lut);
  /* EPD hardware init end */
  return 0;
}

void SpiTransfer(unsigned char data) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(data);
  digitalWrite(CS_PIN, HIGH);
}


/**
 *  @brief: basic function for sending commands
 */
void EPD_SendCommand(EPD* epd, unsigned char command) {
  digitalWrite(DC_PIN, LOW);
  SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_SendData(EPD* epd, unsigned char data) {
  digitalWrite(DC_PIN, HIGH);
  SpiTransfer(data);
}


/**
 *  @brief: Wait until the busy_pin 1oes LOW
 */
void EPD_WaitUntilIdle(EPD* epd) {
  while(digitalRead(BUSY) == HIGH) {      //0: busy, 1: idle
    delay(100);
  }      
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see EPD::Sleep();
 */
void EPD_Reset(EPD* epd) {
  // Hardware reset
  digitalWrite(RST_PIN, LOW);
  delay(50);
  digitalWrite(RST_PIN, HIGH);
  delay(50);
  EPD_SendCommand(epd, SW_RESET);
  EPD_WaitUntilIdle(epd);
  delay(10);
}

/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 */
void EPD_SetFrameMemory(
  EPD* epd,
  const unsigned char* image_buffer,
  int x,
  int y,
  int image_width,
  int image_height
) {
  int x_end;
  int y_end;

  if (
    image_buffer == NULL ||
    x < 0 || image_width < 0 ||
    y < 0 || image_height < 0
  ) {
    return;
  }
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  x &= 0xF8;
  image_width &= 0xF8;
  if (x + image_width >= epd->width) {
    x_end = epd->width - 1;
  } else {
    x_end = x + image_width - 1;
  }
  if (y + image_height >= epd->height) {
    y_end = epd->height - 1;
  } else {
    y_end = y + image_height - 1;
  }
  EPD_SetMemoryArea(epd, x, y, x_end, y_end);
  EPD_SetMemoryPointer(epd, x, y);
  EPD_SendCommand(epd, WRITE_RAM);
  /* send the image data */
  for (int j = 0; j < y_end - y + 1; j++) {
    for (int i = 0; i < (x_end - x + 1) / 8; i++) {
      EPD_SendData(epd, image_buffer[i + j * (image_width / 8)]);
    }
  }
}

/**
*  @brief: clear the frame memory with the specified color.
*          this won't update the display.
*/
void EPD_ClearFrameMemory(EPD* epd, unsigned char color) {
  EPD_SetMemoryArea(epd, 0, 0, epd->width - 1, epd->height - 1);
  EPD_SetMemoryPointer(epd, 0, 0);
  EPD_SendCommand(epd, WRITE_RAM);
  /* send the color data */
  for (int i = 0; i < epd->width / 8 * epd->height; i++) {
    EPD_SendData(epd, color);
  }
}

/**
*  @brief: update the display
*          there are 2 memory areas embedded in the e-paper display
*          but once this function is called,
*          the the next action of SetFrameMemory or ClearFrame will 
*          set the other memory area.
*/
void EPD_UpdateDisplay(EPD* epd) {
  EPD_SendCommand(epd, DISPLAY_UPDATE_CONTROL_2);
  EPD_SendData(epd, 0xF7); // Same as in gfx2 library
  EPD_SendCommand(epd, MASTER_ACTIVATION);
  EPD_SendCommand(epd, TERMINATE_FRAME_READ_WRITE);
  EPD_WaitUntilIdle(epd);
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          You can use EPD_Init() to awaken
 */
void EPD_Sleep(EPD* epd) {
  EPD_SendCommand(epd, DEEP_SLEEP_MODE);
  EPD_SendCommand(epd, DRIVER_OUTPUT_CONTROL);
}

/**
 *  @brief: set the look-up tables
 */
static void EPD_SetLut(EPD* epd, const unsigned char* lut) {
  epd->lut = lut;
  EPD_SendCommand(epd, WRITE_LUT_REGISTER);
  /* the length of look-up table is 30 bytes */
  for (int i = 0; i < 30; i++) {
    EPD_SendData(epd, epd->lut[i]);
  } 
}

/**
 *  @brief: private function to specify the memory area for data R/W
 */
static void EPD_SetMemoryArea(EPD* epd, int x_start, int y_start, int x_end, int y_end) {
  EPD_SendCommand(epd, SET_RAM_X_ADDRESS_START_END_POSITION);
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  EPD_SendData(epd, (x_start >> 3) & 0xFF);
  EPD_SendData(epd, (x_end >> 3) & 0xFF);
  EPD_SendCommand(epd, SET_RAM_Y_ADDRESS_START_END_POSITION);
  EPD_SendData(epd, y_start & 0xFF);
  EPD_SendData(epd, (y_start >> 8) & 0xFF);
  EPD_SendData(epd, y_end & 0xFF);
  EPD_SendData(epd, (y_end >> 8) & 0xFF);
}

/**
 *  @brief: private function to specify the start point for data R/W
 */
static void EPD_SetMemoryPointer(EPD* epd, int x, int y) {
  EPD_SendCommand(epd, SET_RAM_X_ADDRESS_COUNTER);
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  EPD_SendData(epd, (x >> 3) & 0xFF);
  EPD_SendCommand(epd, SET_RAM_Y_ADDRESS_COUNTER);
  EPD_SendData(epd, y & 0xFF);
  EPD_SendData(epd, (y >> 8) & 0xFF);
  EPD_WaitUntilIdle(epd);
}

const unsigned char lut_full_update[] =
{
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51, 
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

const unsigned char lut_partial_update[] =
{
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
