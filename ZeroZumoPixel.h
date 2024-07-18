// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file ZumPixel.h */

#include <Adafruit_NeoPixel.h>

/*! \brief Plays beeps and music on the buzzer on the Zumo Shield.
 *
 * This class uses Timer 4 and pin 6 (PD7/OC4D) to play beeps and melodies on
 * the Zumo 32U4 buzzer.
 *
 * Note durations are timed using a timer overflow interrupt
 * (`TIMER4_OVF`), which will briefly interrupt execution of your
 * main program at the frequency of the sound being played. In most cases, the
 * interrupt-handling routine is very short (several microseconds). However,
 * when playing a sequence of notes in `PLAY_AUTOMATIC` mode (the default mode)
 * with the `play()` command, this interrupt takes much longer than normal
 * (perhaps several hundred microseconds) every time it starts a new note. It is
 * important to take this into account when writing timing-critical code.
 */
class ZumoPixel : public Adafruit_NeoPixel
{
    public:  
        ZumoPixel(uint16_t n, uint8_t p, neoPixelType t = NEO_GRB + NEO_KHZ800)
            : Adafruit_NeoPixel(n, p, t) {}
        ZumoPixel(void) : Adafruit_NeoPixel(){} 
        ~ZumoPixel()
        {

        }
        void show(void) {
            Adafruit_NeoPixel::show();
        }
        void setPin(int16_t p){
            Adafruit_NeoPixel::setPin(p);
        }
        void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b){
            Adafruit_NeoPixel::setPixelColor(n, r, g, b);
        }
        void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
            {Adafruit_NeoPixel::setPixelColor(n, r,g,b,w);

        }
};