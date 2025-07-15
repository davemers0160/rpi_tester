import os
import sys
import numpy as np

import spidev
import time


# requires the python common repo (davemers0160/python_common)
script_path = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))))
sys.path.append(script_path + "/python_common/signal_generation/")
from sequences import maximal_length_sequence, barker_code

def sequence_to_bytearray(seq):
    # Pack the bits into a uint8 array
    packed_bits = np.packbits(seq)
    byte_array = packed_bits.tobytes()
    return byte_array


taps = np.array([8,7,6,1]).astype(np.int8)
num = 9

t = maximal_length_sequence(num, taps).astype(np.int8)
t = t//2 + 1

sequence_bytearray = sequence_to_bytearray(t)

bp = 1

# Create an SpiDev object
spi = spidev.SpiDev()

# Open the SPI bus and device (bus 0, device 0 - CE0)
# You might need to change the bus and device numbers depending on your setup
spi.open(0, 0)

# Set SPI speed (e.g., 500 KHz)
spi.max_speed_hz = 10000000

# Set SPI mode (e.g., mode 0)
# Mode 0: CPOL=0, CPHA=0
# Mode 1: CPOL=0, CPHA=1
# Mode 2: CPOL=1, CPHA=0
# Mode 3: CPOL=1, CPHA=1
spi.mode = 0b00

for idx in range(100):

    # Transfer data and receive response
    spi.xfer2(sequence_bytearray)

    time.sleep(0.005)
    print(f"Iteration: {idx}")

# Close the SPI connection
spi.close()
print("SPI connection closed.")
