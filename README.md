# gba-mmo-proxy

This project acts as the proxy between the [GBA MMO game ROM](https://github.com/maciel310/gba-mmo)
running on the GameBoy Advance and the [game server](https://github.com/maciel310/gba-mmo-server).
It is designed to run on a Raspberry Pi with a GameLink cable connected to the GPIO pins on the Pi.

## Setup Instructions

### Hardware

To connect the GBA to the Raspberry Pi you'll need a GameLink cable (either a GameBoy Color or GameBoy Advance version will work).
You'll either need to cut apart the GameLink cable, or use a
[breakout board](https://www.google.com/search?q=gamelink+breakout+board&tbm=isch)
of some kind.

```
     ===
/===========\
| 1 | 3 | 5 |
| 2 | 4 | 6 |
-------------
```

Above is the pinout of the GameLink cable looking at the cable that will plug into the GBA. Unfortunately not all wires are
present in all cables, and it can vary between either end of the cable (in my GBA link cable only one end was usable, but
in a GameBoy Color cable both ends can be used). For this connection we need pins 2, 3, 5, and 6. If your cable doesn't
contain pin 6 then pin 4 appears to work instead.

Connect the pins as follows. For the Raspberry Pi pins, refer to http://pinout.xyz for a diagram.

| GBA Pin    | RPi Pin 
|------------|---------
| 2          | 21 (GPIO9 / MISO)
| 3          | 19 (GPIO10 / MOSI)
| 5          | 23 (GPIO11 / SCLK)
| 6 **or** 4 | 25 (GROUND)

Note: The only real way to know which wire is in which pin is to measure it with a multimeter or pull off the plastic cover
on the connector and look how the wires go in. Do not rely on color coding diagrams you find online, as even for official
cables this might not be right. You should also verify the wiring if using a breakout connector, as the wires are crossed
from one side of the cable to the other.


### Software

First, ensure the SPI interface on the Raspberry Pi is enabled by
[following these instructions](https://www.raspberrypi-spy.co.uk/2014/08/enabling-the-spi-interface-on-the-raspberry-pi/).

Once SPI is enabled, you can run the `host` command found in the released game. If this is your first time you'll
be prompted to create an account by entering an email address and username. Once that is done just connect the GameLink
cable created above, turn on your GBA, and assuming everything is connected correctly it should load the game via multiboot.


### Troubleshooting

If you run in to any issues getting set up join the [Discord](https://discord.gg/B3wJYnUswN). As common issues are identified
I'll expand this section with some tips.


## Credits

multiboot.c is heavily derrived from the implementation at https://github.com/akkera102/gba_03_multiboot, which is licensed CC0.

# LICENSE

Other than multiboot.c, this project is licensed under the terms of GPLv3. If you would like to use any part of this project
in a way that is incompatible with the GPLv3 license please reach out and I may be willing to grant an alternative license.
