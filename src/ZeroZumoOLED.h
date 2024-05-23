#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>


#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define OLED_ADDR   0x3C

class ZumoOLED : public Adafruit_SSD1306
{

    public:
      ZumoOLED(uint8_t w, uint8_t h, TwoWire *twi = &Wire,
                   int8_t rst_pin = -1, uint32_t clkDuring = 400000UL,
                   uint32_t clkAfter = 100000UL):  Adafruit_SSD1306(w,h, twi, rst_pin,
                   clkDuring, clkAfter){

                   }
    
        bool begin(uint8_t switchvcc = SSD1306_SWITCHCAPVCC, uint8_t i2caddr = 0,
             bool reset = true, bool periphBegin = true)
        {
            Adafruit_SSD1306::begin(switchvcc,i2caddr);
        }

        void clearDisplay(void){
            Adafruit_SSD1306::clearDisplay();
        }

};