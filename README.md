Has this ever happened to you?

You work on a physical prototyping project only to find out your microcontroller board doesn't have the resources (meaning memory) to actually make your code, and consequently your device, work as designed. :(

Luckily there is a solution...

Interfacing outboard memory with a microcontroller board is not an impossible task, it's actually rather straightforward. If you're using SPI ICs (like the ones this library was designed for), you just need to 
connect the lines for clock, data in, data out, and chip select. EZPZ.

HOWEVER, that doesn't mean you have a whole new piece of working memory for your device. Most of what actually ensures functionality for your new ICs is handled by software run on the microcontroller.

Which is where this comes into play...
