
#include "SPI_595_ESP8266.h"

#define GPOS ESP8266_REG(0x304)
#define GPOC ESP8266_REG(0x308)

/***********************************************************************************************/

/*****************************************************************************/

SPI_595_ESP8266::SPI_595_ESP8266(void) : Adafruit_GFX(TFT_WIDTH, TFT_HEIGHT)

{

   Serial.begin(115200);
  delay(1000);
  //#ifdef TFT_SPI_OE
  //pinMode(TFT_SPI_OE,OUTPUT);
  //digitalWrite(TFT_SPI_OE,LOW);
  //Serial.print("\n OE est défini..\n");
//#endif
  //Serial.begin(115200);
  delay(1000);
  //#ifdef TFT_SPI_OE
  //pinMode(TFT_SPI_OE,OUTPUT);
  //digitalWrite(TFT_SPI_OE,LOW);
  //#endif

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
/*
//Reset afficheur
#ifdef RST_LCD
  pinMode(RS_LCD, OUTPUT);
  digitalWrite(RST_LCD,HIGH);
  digitalWrite(RST_LCD,LOW);
  delay(100);
  digitalWrite(RST_LCD,HIGH);
  writeCmd(0x01); // software reset
  delay(400);
#endif
*/
  
  delay(1000);
//setResolution(TFT_WIDTH,TFT_HEIGHT);
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
// https://github.com/notro/fbtft/blob/master/fb_ili9481.c
#define DELAY 0x80
/*****************************************************************************/

const uint8_t default_init_sequence[] =
{
  // SLP_OUT - Sleep out 
  1, 0x11,
  DELAY, 50,
  //Normal Mode (no scroll)
  1,0x13,
  
  //Display invertion OFF
  1,0x20,
  
  // Power setting 
  4, 0xD0, 0x07, 0x42, 0x18, //OK
  // VCOM 
  4, 0xD1, 0x00, 0x07, 0x10, //OK
  // Power setting for norm. mode 
  3, 0xD2, 0x01, 0x02, //OK
  // Panel driving setting 
  6, 0xC0, 0x10, 0x3B, 0x00, 0x02, 0x11, //OK
  // Frame rate & inv. 
  2, 0xC5, 0x03, //OK

  //2, 0x36, 0x0A, //OK TFT_MADCTL
  2, 0x36, 0x0B, //OK TFT_MADCTL
  // Pixel format 
  2, 0x3A, 0x55, //OK
  
  //5, 0x2A, 0x00, 0x00, 0x01, 0x3F, //OK TFT_CASET (319)
  5,  0x2A, 0x00, 0x00, 0x01, 0x40, //OK TFT_CASET (320)
  5, 0x2B, 0x00, 0x00, 0x01, 0xE0, //OK TFT_PASET (480)
  // Gamma 
  13, 0xC8, 0x00, 0x32, 0x36, 0x45, 0x06, 0x16, //OK 
      0x37, 0x75, 0x77, 0x54, 0x0C, 0x00, //OK
  // DISP_ON 
  
  1, 0x29,
   1, 0x2C,  0
};
/*****************************************************************************/
// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
/*****************************************************************************/
void commandList(const uint8_t *addr)
{
  uint8_t  numBytes, tmp;

  while ( (numBytes=(*addr++))>0 ) { // end marker == 0
    if ( numBytes&DELAY ) {
      //Serial.print("delay ");
      tmp = *addr++;
      //Serial.println(tmp);
      delay(tmp); // up to 255 millis
    } else {
      //Serial.print(numBytes); Serial.print("byte(s): ");
      tmp = *addr++;
      //Serial.write('<'); Serial.print(tmp, HEX); Serial.write('>');
      writecommand(tmp); // first byte is command
      while (--numBytes) { //   For each argument...
        tmp = *addr++;
        //Serial.print(tmp, HEX); Serial.write('.');
        writedata(tmp); // all consecutive bytes are data
      }
      //Serial.write('\n');
    }
  }
}

void initlcd()
{
   
    writecommand(TFT_SLPOUT);
    delay(20);

    writecommand(0xB0);
    writedata(0x00);

    writecommand(0xD0);
    writedata(0x07);
    writedata(0x42);
    writedata(0x18);

    writecommand(0xD1);
    writedata(0x00);
    writedata(0x07);
    writedata(0x10);

    writecommand(0xD2);
    writedata(0x01);
    writedata(0x02);

    writecommand(0xC0);
    writedata(0x12);
    writedata(0x3B);
    writedata(0x00);
    writedata(0x02);
    writedata(0x11);

    writecommand(0xC5);
    writedata(0x03);

    writecommand(0xC8);
    writedata(0x00);
    writedata(0x32);
    writedata(0x36);
    writedata(0x45);
    writedata(0x06);
    writedata(0x16);
    writedata(0x37);
    writedata(0x75);
    writedata(0x77);
    writedata(0x54);
    writedata(0x0C);
    writedata(0x00);

    writecommand(TFT_MADCTL);
    writedata(0x0A);
    
    writecommand(0x20); // normal mode
    //writecommand(0x21); // invert mode

    writecommand(0x3A);
    writedata(0x55);

    writecommand(TFT_CASET);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0x3F);

    writecommand(TFT_PASET);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0xDF);

    delay(120);
    writecommand(TFT_DISPON);

    delay(25);
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
  
  // ,invertDisplay(true);
  //#include"HX8357_Init.h";
  //#include "TFT_Drivers/R61581_init.h"
  //#include "TFT_Drivers/R61581_Init.h"
  initlcd();
  //commandList(default_init_sequence);
  delay(1000);
}
/*****************************************************************************/
void SPI_595_ESP8266::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  //writecommand(ILI9481_CASET); // Column addr set
  writecommand(TFT_CASET);
  writedata(x0 >> 8);
  writedata(x0 & 0xFF);     // XSTART
  writedata(x1 >> 8);
  writedata(x1 & 0xFF);     // XEND

  //writecommand(ILI9481_PASET); // Row addr set
  writecommand(TFT_PASET);
  writedata(y0 >> 8);
  writedata(y0);     // YSTART
  writedata(y1 >> 8);
  writedata(y1);     // YEND

  //writecommand(ILI9481_RAMWR); // write to RAM
  writecommand (TFT_RAMWR);
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
  //writedata16(color, 100);
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

/*****************************************************************************/
/*
void SPI_595_ESP8266::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  //uint16_t n =76800;
  if ((x >= _width) || (y >= _height || h < 1 || w < 1)) return;
  if ((x + w - 1) >= _width) { w = _width  - x; }
  if ((y + h - 1) >= _height) { h = _height - y; }
  if (w == 1 && h == 1) {
    drawPixel(x, y, color);
    return;
  }

  setAddrWindow(x, y, x + w - 1, y + h - 1);
  writedata16(color, (w*h)/2); yield();writedata16(color, (w*h)/2); 
  
  //writedata16(color, (w*h)/2); yield();writedata16(color, (w*h)/2); 
}
*/
/*
* Draw lines faster by calculating straight sections and drawing them with fastVline and fastHline.
*/
/*****************************************************************************/
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
/*****************************************************************************/

void SPI_595_ESP8266::setRotation(uint8_t m)
{
  #include "TFT_Drivers/R61581_Rotation.h"
}

/*
void SPI_595_ESP8266::setRotation(uint8_t m)
{
  //writecommand(ILI9481_MADCTL); // (0x36)
  uint8_t rotation = m & 3; // can't be higher than 3
  writecommand(0x36);
  rotation = m % 4;
  switch (rotation) {
   case 0: // Portrait
     writedata(0x08); //0x08
     writecommand(0xB6);
     writedata(0);
     writedata(0x22);
     writedata(0x3B);
      _width  = TFTWIDTH;
      _height = TFTHEIGHT;
     break;
   case 1: // Landscape (Portrait + 90)
     writedata(0x20 | 0x08);//MAD_MV=0x20
     writecommand(0xB6);
     writedata(0);
     writedata(0x02);
     writedata(0x3B);
      _width  = TFTHEIGHT;
      _height = TFTWIDTH;
     break;
   case 2: // Inverter portrait
     writedata(0x08); //0x08
     writecommand(0xB6);
     writedata(0);
     writedata(0x42);
     writedata(0x3B);
      _width  = TFTWIDTH;
      _height = TFTHEIGHT;
     break;
   case 3: // Inverted landscape
     writedata(0x20 | 0x08);
     writecommand(0xB6);
     writedata(0);
     writedata(0x62);
     writedata(0x3B);
      _width  = TFTHEIGHT;
      _height = TFTWIDTH;
     break;  
  }
 
}
*/

size_t SPI_595_ESP8266::write(uint8_t c)
//void SPI_595_ESP8266::write(uint8_t c)
{
  writeCodepoint((uint16)c);
  //return 1;
}


/*****************************************************************************/

void SPI_595_ESP8266::invertDisplay(boolean i)
{
  //writecommand(i ? ILI9481_INVON : ILI9481_INVOFF);
  writecommand(TFT_INVON);
}
/*
void SPI_595_ESP8266::enable(boolean enable) {
  writecommand(enable ? 0x29 : 0x28);
}
*/