// This #include statement was automatically added by the Particle IDE.
#include <SparkFunMicroOLED.h>
// https://learn.sparkfun.com/tutorials/photon-oled-shield-hookup-guide
#include <math.h>

MicroOLED oled;

class OLEDWrapper {
    public:
        OLEDWrapper() {
            oled.begin();    // Initialize the OLED
            oled.clear(ALL); // Clear the display's internal memory
            oled.display();  // Display what's in the buffer (splashscreen)
            delay(1000);     // Delay 1000 ms
            oled.clear(PAGE); // Clear the buffer.
        }

    void printTitle(String title, int font)
    {
      oled.clear(PAGE);
      oled.setFontType(font);
      oled.setCursor(0, 0);
      oled.print(title);
      oled.display();
    }
};

OLEDWrapper oledWrapper;

class LEDFader {
    private:
        int             pin;
        time_t          startTime;
        unsigned int    fadeTimeInSeconds;
        unsigned int    startIntensity;
        unsigned int    endIntensity;

    public:
        int             currentIntensity;

        LEDFader(int            pin,
                time_t          startTime,
                unsigned int    fadeTimeInSeconds,
                unsigned int    startIntensity,
                unsigned int    endIntensity) {
            this->pin = pin;
            pinMode(pin, OUTPUT);
            resetFader(startTime, fadeTimeInSeconds, startIntensity, endIntensity);
        }

        void resetFader(time_t       startTime,
                unsigned int    fadeTimeInSeconds,
                unsigned int    startIntensity,
                unsigned int    endIntensity) {
            this->startTime = startTime;
            this->fadeTimeInSeconds = fadeTimeInSeconds;
            this->startIntensity = startIntensity;
            this->endIntensity = endIntensity;
            this->currentIntensity = startIntensity;
        }

        void doFade() {
            time_t  now = Time.now();
            currentIntensity = endIntensity;
            if (now >= startTime) {
                if (now <= endTime()) {
                    currentIntensity = map(now, startTime, endTime(), (int)startIntensity, (int)endIntensity);
                    currentIntensity = min(255, currentIntensity);
                }
                analogWrite(pin, currentIntensity);
            }
        }
        
        time_t endTime() {
            return startTime + fadeTimeInSeconds - 1;
        }
};

LEDFader*       ledFader = NULL;
unsigned int    DURATION_IN_SECONDS = 10;
unsigned int    startIntensity = 0;
unsigned int    endIntensity = 255;
time_t          started = 0;

void setup() {
    started = Time.now();
    ledFader = new LEDFader(D0, started, DURATION_IN_SECONDS, startIntensity, endIntensity);
}

void loop() {
    ledFader->doFade();
    oledWrapper.printTitle(String(ledFader->currentIntensity), 3);
    delay(1000);
    
    if (Time.now() > started + DURATION_IN_SECONDS) {
        unsigned int tmp = endIntensity;
        endIntensity = startIntensity;
        startIntensity = tmp;
        started = Time.now();
        ledFader->resetFader(started, DURATION_IN_SECONDS, startIntensity, endIntensity);
    }
}
