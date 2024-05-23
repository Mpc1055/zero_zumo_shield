#include <Wire.h>
#include <ZeroZumoShield.h>


#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT);


ZumoIMU imu;

char report[120];

void setup()
{

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  Wire.begin();

  if (!imu.init())
  {
    // Failed to detect the compass.
    while(1)
    {
       display.println(F("Failed to initialize IMU sensors."));
       display.display();
      delay(100);
    }
  }

  imu.enableDefault();
}

void loop()
{
  imu.read();

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  snprintf_P(report, sizeof(report),
    PSTR("A: %3d %3d %3d    M: %3d %3d %3d    G: %3d %3d %3d"),
    imu.a.x, imu.a.y, imu.a.z,
    imu.m.x, imu.m.y, imu.m.z,
    imu.g.x, imu.g.y, imu.g.z);
   display.println(report);
   display.display();

  delay(100);
}
