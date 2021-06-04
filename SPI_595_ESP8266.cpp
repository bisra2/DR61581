
#include "SPI_595_ESP8266.h"

#define GPOS ESP8266_REG(0x304)
#define GPOC ESP8266_REG(0x308)

/***********************************************************************************************/
SPI_595_ESP8266::SPI_595_ESP8266(void) : Adafruit_GFX(TFT_WIDTH, TFT_HEIGHT)
{

  Serial.begin(115200);
  delay(1000);

  #ifdef TFT_SPI_CS
  pinMode(TFT_SPI_CS,OUTPUT);
  digitalWrite(TFT_SPI_CS,HIGH);
  #endif
 
#ifdef TFT_CS
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH); // Chip select high (inactive)
#endif


#ifdef TFT_WR
  pinMode(TFT_WR, OUTPUT);
  digitalWrite(TFT_WR, HIGH); // Set write strobe high (inactive)
#endif

#ifdef TFT_RS
  pinMode(TFT_RS, OUTPUT);
  digitalWrite(TFT_RS, HIGH); // Data/Command high = data mode
#endif
}


/*****************************************************************************/
void inline writebus(uint8_t d) {
  
  GPOC=((uint32_t)1<<TFT_SPI_CS);
  SPI.transfer(d);
  GPOS=((uint32_t)1<<TFT_SPI_CS);
  
  CS_ACTIVE; 
  GPOC =(1<<TFT_WR);
  //delay(0.005);     //strob sur WR de l'afficheur
  GPOS= (1<<TFT_WR);
  CS_IDLE;
}
/*****************************************************************************/
void writedata16(uint16_t c)
{
  CD_DATA;
  //CS_ACTIVE;
  writebus(c>>8);
  writebus(c&0xFF);
  //CS_ACTIVE
  //CS_IDLE;
}
/*****************************************************************************/
void writedata16(uint16_t color, uint32_t num)
{
  CD_DATA;
  //CS_ACTIVE;
  for (int n=0;n<num;n++) {
    writebus(color>>8);
    writebus(color&0xFF);
  }
  // CS_IDLE;
}
/*****************************************************************************/
void writecommand(uint8_t c)
{
  CD_COMMAND;
  // CS_ACTIVE;
    writebus(c);
  // CS_IDLE;
}
/*****************************************************************************/
void writedata(uint8_t c)
{
  CD_DATA;
  // CS_ACTIVE;
    writebus(c);
  // CS_IDLE;
}

void SPI_595_ESP8266::clearScreen()
{
  int i;
  setAddrWindow(0, 0,  _width - 1, _height - 1);
  //la commande
  writecommand(0x2C);
  writedata(0);         //Met zero sur le bus
  GPOC=(1<<TFT_CS);
  //écriture des 306700 fois zéro dans le lcd
  for(i=0;i<320*480;i++) {
  //premier paramètre 
  //SPI.transfer(0);
  GPOC=((1<<TFT_WR));
  GPOS=((1<<TFT_WR)); //remonte WR_LCD
  //ddeuxième paramètre
  //SPI.transfer(0);
  GPOC=((1<<TFT_WR));
  GPOS=((1<<TFT_WR)); //remonte WR_LCD
  }
  GPOS=(1<<TFT_CS); //inutile de remonter le CS mais bon..
}


/*****************************************************************************/
void SPI_595_ESP8266::begin(void)
{
  //pinMode(TFT_RST, OUTPUT);
  SPI.begin();
  //SPI.setBitOrder();
  delay(550);
  SPI.setHwCs(true);
  SPI.setFrequency(40000000L);
  //invertDisplay(true);
  //#include"HX8357_Init.h";
  //#include "TFT_Drivers/R61581_init.h"
  #include "TFT_Drivers/R61581_Init.h"
  delay(1000);
}
/*****************************************************************************/
void SPI_595_ESP8266::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  writecommand(TFT_CASET);  // Column addr set
  writedata(x0 >> 8);
  writedata(x0 & 0xFF);     // XSTART
  writedata(x1 >> 8);
  writedata(x1 & 0xFF);     // XEND

  writecommand(TFT_PASET);  // Row addr set
  writedata(y0 >> 8);
  writedata(y0);     // YSTART
  writedata(y1 >> 8);
  writedata(y1);     // YEND

  writecommand (TFT_RAMWR);   // write to RAM
}

/*****************************************************************************/
void SPI_595_ESP8266::pushColor(uint16_t color)
{
  writedata16(color);
}

/*****************************************************************************/
void SPI_595_ESP8266::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x, y, x + 1, y + 1);
  pushColor(color);
}

/*****************************************************************************/
void SPI_595_ESP8266::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height || h < 1)) return;
  if ((y + h - 1) >= _height) { h = _height - y; }
  if (h < 2 ) { drawPixel(x, y, color); return; }

  setAddrWindow(x, y, x, y + h - 1);
  writedata16(color, h);
}

/*****************************************************************************/
void SPI_595_ESP8266::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height || w < 1)) return;
  if ((x + w - 1) >= _width) { w = _width - x; }
  if (w < 2 ) { drawPixel(x, y, color); return; }

  setAddrWindow(x, y, x + w - 1, y);
  writedata16(color, w);
}

/*****************************************************************************/
void SPI_595_ESP8266::fillScreen(uint16_t color) {
  setAddrWindow(0, 0,  _width - 1, _height - 1);
  writedata16(color, (_width * _height)/2);
  yield();
  writedata16(color, (_width * _height)/2);
}

/*
void SPI_595_ESP8266::fillScreen(uint16_t color) {
  writecommand(0x2C); 
  for(int i=0;i<(320)*(480);i++) 
  {
    writedata(color>>8);/*yield();writedata(color);
  } 
}
*/

/*************************************************************************************************
* Draw lines faster by calculating straight sections and drawing them with fastVline and fastHline.
*************************************************************************************************/
void SPI_595_ESP8266::drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color)
{
  if ((y0 < 0 && y1 <0) || (y0 > _height && y1 > _height)) return;
  if ((x0 < 0 && x1 <0) || (x0 > _width && x1 > _width)) return;
  if (x0 < 0) x0 = 0;
  if (x1 < 0) x1 = 0;
  if (y0 < 0) y0 = 0;
  if (y1 < 0) y1 = 0;

  if (y0 == y1) {
    if (x1 > x0) {
      drawFastHLine(x0, y0, x1 - x0 + 1, color);
    }
    else if (x1 < x0) {
      drawFastHLine(x1, y0, x0 - x1 + 1, color);
    }
    else {
      drawPixel(x0, y0, color);
    }
    return;
  }
  else if (x0 == x1) {
    if (y1 > y0) {
      drawFastVLine(x0, y0, y1 - y0 + 1, color);
    }
    else {
      drawFastVLine(x0, y1, y0 - y1 + 1, color);
    }
    return;
  }

  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  }
  else {
    ystep = -1;
  }

  int16_t xbegin = x0;
  if (steep) {
    for (; x0 <= x1; x0++) {
      err -= dy;
      if (err < 0) {
        int16_t len = x0 - xbegin;
        if (len) {
          drawFastVLine (y0, xbegin, len + 1, color);
          //writeVLine_cont_noCS_noFill(y0, xbegin, len + 1);
        }
        else {
          drawPixel(y0, x0, color);
          //writePixel_cont_noCS(y0, x0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if (x0 > xbegin + 1) {
      //writeVLine_cont_noCS_noFill(y0, xbegin, x0 - xbegin);
      drawFastVLine(y0, xbegin, x0 - xbegin, color);
    }

  }
  else {
    for (; x0 <= x1; x0++) {
      err -= dy;
      if (err < 0) {
        int16_t len = x0 - xbegin;
        if (len) {
          drawFastHLine(xbegin, y0, len + 1, color);
          //writeHLine_cont_noCS_noFill(xbegin, y0, len + 1);
        }
        else {
          drawPixel(x0, y0, color);
          //writePixel_cont_noCS(x0, y0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if (x0 > xbegin + 1) {
      //writeHLine_cont_noCS_noFill(xbegin, y0, x0 - xbegin);
      drawFastHLine(xbegin, y0, x0 - xbegin, color);
    }
  }
}

/*****************************************************************************/
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
/*****************************************************************************/
uint16_t SPI_595_ESP8266::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/*****************************************************************************
Chaque ic contrôleur nécessite une fonction rotation spécifique rangée dans 
TFT_Drivers.
*****************************************************************************/
void SPI_595_ESP8266::setRotation(uint8_t m)
{
  #include "TFT_Drivers/R61581_Rotation.h"
}

/*****************************************************************************
Cette fonction pose un soucis de warning non résolu..
*****************************************************************************/
size_t SPI_595_ESP8266::write(uint8_t c)
{
  writeCodepoint((uint16)c);
}


/*****************************************************************************/
void SPI_595_ESP8266::invertDisplay(boolean i)
{
  writecommand(i ? TFT_INVON : TFT_INVOFF);
  //writecommand(TFT_INVON);
}

/*
void SPI_595_ESP8266::enable(boolean enable) {
  writecommand(enable ? 0x29 : 0x28);
}
*/