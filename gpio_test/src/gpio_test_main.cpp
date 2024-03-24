
// C++ includes
#include <cmath>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

// RPI gpio include - using pigio library
#include <pigpio.h>


/* GPIO Mapping - Type 3 - Model A+, B+, Pi Zero, Pi Zero W, Pi2B, Pi3B, Pi4B

       GPIO#   pin pin     GPIO#
               --------
3V3     -     |  1   2 |    -       5V
               --------
SDA     2     |  3   4 |    -       5V
               --------
SCL     3     |  5   6 |    -       Ground
               --------
        4     |  7   8 |    14      TXD
               --------
Ground  -     |  9  10 |    15      RXD
               --------
ce1     17    | 11  12 |    18      ce0
               --------
        27    | 13  14 |    -       Ground
               --------
        22    | 15  16 |    23
               --------
3V3     -     | 17  18 |    24
               --------
MOSI    10    | 19  20 |    -       Ground
               --------
MISO    9     | 21  22 |    25
               --------
SCLK    11    | 23  24 |    8       CE0
               --------
Ground  -     | 25  26 |    7       CE1
               --------
ID_SD   0     | 27  28 |    1       ID_SC
               --------
        5     | 29  30 |    -       Ground
               --------
        6     | 31  32 |    12
        13    | 33  34 |    -       Ground
               --------
miso    19    | 35  36 |    16      ce2
               --------
        26    | 37  38 |    20      mosi
               --------
Ground  -     | 39  40 |    21      sclk
               --------

*/

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t idx;
    int32_t gpio_result = 0;
    int32_t gpio_value;
    
    int32_t led_gpio = 2;
    
    
    // initialize
    gpio_result = gpioInitialise();
    if (gpio_result < 0)
    {
        std::cout << "error initializing gpio lib" << std::endl;
        return -1;
    }
    
    // set the gpio pin to output
    gpioSetMode(led_gpio, PI_OUTPUT);
    
    
    for(idx=0; idx<100; ++idx) 
    {
        gpio_value = idx & 0x01;
        std::cout << "gpio value: " << gpio_value << std::endl;
        gpio_result = gpioWrite(led_gpio, gpio_value);
        
        if (gpio_result < 0)
        {
            std::cout << "error setting gpio: " << gpio_result << std::endl;
        }        
            
        gpioDelay(50000);
    
    }
    
    std::cout << "complete!" << std::endl;
    
    gpioTerminate();

    return 0;

}   // end of main
