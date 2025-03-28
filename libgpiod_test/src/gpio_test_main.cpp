
// C++ includes
#include <cmath>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <iostream>
#include <thread>

// RPI gpio include - libgpiod c++ bindings
#include <gpiod.hpp>

// Use following commands to install prerequisites and build
// sudo apt install gpiod
// sudo apt install libgpiod-dev
//
// sudo apt-get install autoconf-archive libtool pkg-config autotools-dev
//
// git clone -b v2.2.x https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git
// cd libgpiod
// ./autogen.sh --enable-tools --prefix=/opt
// make
// sudo make install



/* GPIO Mapping - Type 3 - Model A+, B+, Pi Zero, Pi Zero W, Pi2B, Pi3B, Pi4B, P5 (gpiochip4)

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

namespace {

/* Example configuration - customize to suit your situation */
const ::std::filesystem::path chip_path("/dev/gpiochip0");
const ::gpiod::line::offsets led_gpio = 2;

} /* namespace */

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t idx;
    int32_t gpio_result = 0;
    
    //int32_t led_gpio = 2;
    
    //const ::std::filesystem::path chip_path("/dev/gpiochip4");
    //const ::gpiod::line::offset led_gpio = 2;
    
    // initialize
	auto gpio_request =
		::gpiod::chip(chip_path)
			.prepare_request()
			.set_consumer("toggle-line-value")
			.add_line_settings(line_offset, ::gpiod::line_settings().set_direction(::gpiod::line::direction::OUTPUT))
			.do_request();
    
    
    ::gpiod::line::value gpio_value = ::gpiod::line::value::ACTIVE;
    
    for(idx=0; idx<200; ++idx) 
    {
        
        std::cout << "gpio value: " << gpio_value << std::endl;
        
        gpio_request.set_value(led_gpio, gpio_value);   
                    
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        gpio_value = ::gpiod::line::value::ACTIVE) ? ::gpiod::line::value::INACTIVE : ::gpiod::line::value::ACTIVE;
    
    }
    
    std::cout << "complete!" << std::endl;
    
    gpioTerminate();

    return 0;

}   // end of main
