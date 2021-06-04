/*
ATTENTION!!

fonctionne avec afficheur TFT avec IC RM68140
*/

#ifndef _SPI_595_ESP8266H_
#define _SPI_595_ESP8266H_
#include "Arduino.h"
#include <SPI.h>
#include "TFT_Drivers/R61581_Defines.h"
#include <Adafruit_GFX.h>

// Color definitions
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define PINK        0xF81F
#include "/home/jean/.arduino15/packages/esp8266/hardware/esp8266/2.7.4/cores/esp8266/esp8266_peri.h"
// Control pins connect to ESP8266
#define TFT_RST  //16 //NULL
#define TFT_CS   5
#define TFT_RS   4  
 #define TFT_WR  03 //10
 

// SPI pin connect to 74HC595 <-> ESP8266 | ESP32
#define TFT_SPI_SCK  14
#define TFT_SPI_MOSI 13
#define TFT_SPI_CS   15
//#define TFT_SPI_OE 2 //OE put on gnd

#define CD_COMMAND   GPOC=(1<<TFT_RS);
#define CD_DATA      GPOS=(1<<TFT_RS);

#define CS_ACTIVE    GPOC = ((uint32_t)1 << TFT_CS);
#define CS_IDLE      GPOS = ((uint32_t)1 << TFT_CS);

#define swap(a, b) { int16_t t = a; a = b; b = t; }



void inline writebus(uint8_t) ;
void writecommand(uint8_t) ;
void writedata(uint8_t) ;
void writedata16(uint16_t) ;
void writedata16(uint16_t, uint32_t) ;
void commandList(uint8_t *addr);

/*****************************************************************************/
class SPI_595_ESP8266 : public Adafruit_GFX {
  public:
    //SPIClass_ESP8266 sspi;
    SPI_595_ESP8266(void);
    void begin() ;
    void initDisplay();
    void setAddrWindow(uint16_t, uint16_t, uint16_t, uint16_t) ;
    void pushColor(uint16_t) ;
    void fillScreen(uint16_t) ;
    void clearScreen();
    void drawLine(int16_t, int16_t,int16_t, int16_t, uint16_t) ;
    void drawPixel(int16_t, int16_t, uint16_t) ;
    void drawFastVLine(int16_t, int16_t, int16_t, uint16_t) ;
    void drawFastHLine(int16_t, int16_t, int16_t, uint16_t) ;
    //void fillRect(int16_t, int16_t, int16_t, int16_t, uint16_t) ;
    void setRotation(uint8_t) ;
    void invertDisplay(boolean) ;
    uint16_t color565(uint8_t, uint8_t, uint8_t) ;
    size_t write(uint8_t);
    //void write(uint8_t);
    
    void reset();
    //void enable(bool) ;

    

   private:
    uint8_t tabcolor;
    
};

/*
void inline writebus(uint8_t) ;
void writecommand(uint8_t) ;
void writedata(uint8_t) ;
void writedata16(uint16_t) ;
void writedata16(uint16_t, uint32_t) ;
void commandList(uint8_t *addr);
*/
#endif //endif of the header file
