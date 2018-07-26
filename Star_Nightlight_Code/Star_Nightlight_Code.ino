#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 0

//Initialise the strip wiuth 40 pixels using pin 0
Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, PIN, NEO_GRB + NEO_KHZ800);

float minAmbient = 255;
float maxAmbient = 0;
float ambient = 0.5;
float bright = 50;
bool isOn = false;
bool shouldOn = false;
long nextDecrease = 1;
long lastOff = 0;

void setup()
{
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket or Tiny85
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  strip.setBrightness(0);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // initialise pin 0 to be an analog input
  pinMode( 3, INPUT );
}

void loop()
{

  // Read the state of the LDR
  float val = analogRead( 3 );

  // adjust min and max readings to accomodate change in light
  if ( val < minAmbient )
    minAmbient = val;
  if ( val > maxAmbient )
    maxAmbient = val;

  // work out the abient light based on min and max values
  ambient = ( val - minAmbient ) / ( maxAmbient - minAmbient );
  
  // if the abmient light is less than haf the middle of the min and max, turn on the pixels
  if ( ambient * 255 <  ( maxAmbient - minAmbient ) / 2 )
    shouldOn = true;
  else // else turn them off
    shouldOn = false;

  // if the pixels are on and they should be off, set them to black and record the last time they were turned off
  if ( !shouldOn && isOn )
  {
      isOn = false;
      lastOff = millis();
      turnOff();
      
  }
  else if ( shouldOn ) // if the pixels should be on
  {
    // if the pixels were not on, if the last time they were off is more than 10 seconds agi, reset their initial brightness to 50%
    if ( !isOn )
    {
      isOn = true;
      if ( millis() - lastOff > 10000 ) 
        bright = 50;
    }

  // if the brighness is grater than 10% and the next decrease time has been passed, adjust the brightness down
  if ( bright > 10 && nextDecrease < millis() )
  {
    bright-=1;
    nextDecrease = millis() + 100;
    strip.setBrightness(bright);
    strip.show();
  }

    //display a rainbow around the star
    rainbowCycle(10);
  }
}

// Fill the dots one after the other with black
void turnOff()
{
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait)
{
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
