
#define WITH_RPI 1

// C++ includes
#include <cstdint>
#include <cmath>
#include <cstring>
#include <csignal>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <complex>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <filesystem>

#include "iq_utils.h"
#include "data_logger.h"
#include "file_ops.h"

#if defined(WITH_RPI)
// RPI gpio include - libgpiod c++ bindings
#include <gpiod.hpp>
#endif

// Use following commands to install prerequisites and build
//
// sudo apt-get install autoconf-archive libtool pkg-config autotools-dev
//
// git clone -b v2.2.x https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git
// cd libgpiod
// ./autogen.sh --enable-tools --enable-bindings-cxx --prefix=/opt
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

/* Example configuration - customize to suit your situation */



//--------------------------- GLOBAL VARIABLES --------------------------------
std::atomic<bool> is_running = false;

std::atomic<bool> transmit = false;

std::atomic<bool> switch_thread_running = false;
std::atomic<uint16_t> current_switch_setting = 0;

uint32_t blade_timeout_ms = 10000;

const gpiod::line::value gpio_on = gpiod::line::value::ACTIVE;
const gpiod::line::value gpio_off = gpiod::line::value::INACTIVE;
const gpiod::line::offset rf_ctrl_pin = 20;

const gpiod::line::offsets switch_lines = { 27, 17, 4, 3, 2 };

//-----------------------------------------------------------------------------
void sig_handler(int sig_num)
{
    if ((sig_num == SIGINT) | (sig_num == SIGTERM))
    {
        std::cout << info << "Received SIGINT: " << sig_num << std::endl;
        is_running = false;
        switch_thread_running = false;
        transmit = false;
    }

}   // end of sig_handler

//-----------------------------------------------------------------------------
#if defined(WITH_RPI)
inline void poll_switch_thread(gpiod::line_request &request, std::vector<std::complex<int16_t>>& samples, std::vector<std::string> &iq_file_list)
{
    uint32_t idx;
    
    gpiod::line::values switch_values;
    uint16_t tmp_switch_value = 0;
    bool current_transmit_status = false;

    std::cout << info << "Switch thread started." << std::endl;

    // main thread loop
    while (switch_thread_running == true)
    {
        tmp_switch_value = 0;
        switch_values = request.get_values();

        // cycle through the switch pins 
        for (idx = 0; idx < switch_values.size(); ++idx)
        {
            std::cout << info << "switch_values[" << idx << "]: " << switch_values[idx] << std::endl;
            // first match to gpio_on is the one
            if (switch_values[idx] == gpio_on)
            {
                tmp_switch_value = idx+1;
                continue;
            }
        }

        // check for a state change
        if (tmp_switch_value != current_switch_setting)
        {
            current_switch_setting = tmp_switch_value;
            std::cout << info << "Switch value: " << current_switch_setting << std::endl;

            if (current_switch_setting == 0)
            {
                transmit = false;
            }
            else
            {
                current_transmit_status = transmit;
                transmit = false;

                // wait for a little for the transmit cycle to finish
                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                // load the new IQ file
                std::cout << info << "IQ filename: " << iq_file_list[idx-1] << std::endl;
                //data_log << info << "IQ filename: " << iq_file_list[idx-1] << std::endl;

                //samples = read_iq_data<int16_t>(iq_file_path + iq_file_list[idx - 1]);
                //num_tx_samples = samples.size();

                //std::cout << info << "num_tx_samples: " << num_tx_samples << std::endl;
                //data_log << info << "num_tx_samples: " << num_tx_samples << std::endl;

                transmit = current_transmit_status;
            }
        }

        // sleep and then beging polling again
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    }

    std::cout << info << "Switch thread stopped." << std::endl;

}   // end of poll_switch_thread

#endif

//-----------------------------------------------------------------------------
std::vector<std::string> directory_listing(const std::string &path, const std::string &extension = ".*")
{
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(path)) 
    {
        if (std::filesystem::is_regular_file(entry) && entry.path().extension() == extension) 
        {
            files.push_back(entry.path().filename().string());
        }
    }
    
    // sort them alphabetically
    std::sort(files.begin(), files.end(), std::less<std::string>());

    return files;
}   // end of directory_listing

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t idx;
    int32_t gpio_result = 0;
    
    std::string iq_filename;
    std::vector<std::string> iq_file_list;
    
    std::vector<std::complex<int16_t>> samples(1024*8, 0);

    
    std::string iq_file_path = "../../../data/RF/";
    iq_file_list = directory_listing(iq_file_path, ".sc16");
    if (iq_file_list.empty() == false)
    {
        iq_filename = iq_file_path + iq_file_list[0];
    }

    // list the files found
    for(idx=0; idx< iq_file_list.size(); ++idx)
    {
        std::cout << info << iq_file_list[idx] << std::endl;   
    }
    

#if defined(WITH_RPI)
    // define the gpio chip and pins
    const std::filesystem::path chip_path("/dev/gpiochip0");

    gpiod::line::value gpio_value;

    // initialize the chip & gpio line
    gpiod::chip gpio_chip = gpiod::chip(chip_path);
    gpiod::request_builder gpio_request = gpio_chip.prepare_request();

    gpio_request.set_consumer("rf_control");
    gpio_request.add_line_settings(rf_ctrl_pin, gpiod::line_settings().set_direction(gpiod::line::direction::OUTPUT));

	// allocate the gpio for use
    gpiod::line_request rf_gpio_line = gpio_request.do_request();

    // setup the gpio lines that montior the switch
    gpiod::request_builder gpio_switch_request = gpio_chip.prepare_request();
    gpio_switch_request.set_consumer("switch_monitor");
    gpio_switch_request.add_line_settings(switch_lines, gpiod::line_settings().set_direction(gpiod::line::direction::INPUT));
    //gpio_switch_request.add_line_settings(switch_lines, gpiod::line_settings().set_bias(gpiod::line::bias::PULL_DOWN));
    
    // allocate the switch lines
    gpiod::line_request switch_lines = gpio_switch_request.do_request();
    switch_lines.set_bias(gpiod::line::bias::PULL_DOWN);
    
#endif

    try
    {
        
        is_running = true;

        std::thread switch_thread;

#if defined(WITH_RPI)
        switch_thread_running = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        switch_thread = std::thread(poll_switch_thread, std::ref(switch_lines), std::ref(samples), std::ref(iq_file_list));
#endif

        std::cout << std::endl << info << "GPIO Test Running..." << std::endl << std::endl;
        //-----------------------------------------------------------------------------
        // main loop
        while (is_running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            std::cout << "Current Switch Position: " << current_switch_setting << std::endl;
            
            
        }   // end of while(is_running)
            
        switch_thread_running = false;

        transmit = false;
        
#if defined(WITH_RPI)

//#if defined(WITH_SWITCH_LINES)
        switch_thread.join();
//#endif

        // close the gpio line
        rf_gpio_line.set_value(rf_ctrl_pin, gpio_off);
        rf_gpio_line.release();
        switch_lines.release();
        gpio_chip.close();

        std::cout << info << "Closing GPIO..." << std::endl;
        //data_log << info << "Closing GPIO..." << std::endl;
#endif


    }
    catch(std::exception &e)
    {
        std::cout << error(__FILE__, __LINE__) << "Error: " << e.what() << std::endl;
        //data_log << error(__FILE__, __LINE__) << "Error: " << e.what() << std::endl;
#if defined(WITH_RPI)
        // close the gpio line
        rf_gpio_line.set_value(rf_ctrl_pin, gpio_off);
        rf_gpio_line.release();
        switch_lines.release();
        gpio_chip.close();

        std::cout << info << "Closing GPIO..." << std::endl;
        //data_log << info << "Closing GPIO..." << std::endl;
#endif        
    }

    
    std::cout << "Complete!  Press enter..." << std::endl;
    std::cin.ignore();
    return 0;

}   // end of main
