# motion.c++

The burrito boy was building some Arduino device for flipping his light switch
on & off depending on whether an IR sensor was receiving input periodically.
(Opening the door or sitting at the desk interrupts the signal.)  As long as
it was receiving a signal every second, the light should stay off; and once
the light was on, it should stay on until we'd gone some number of seconds
without interruption in the signal.

We jibber-jabbered on the phone about how to do this; this was my version.  I
don't have an Arduino device to test it on (and am not linking with Arduino
libraries), so this wraps the `loop()` in a `main()` which just sets the time,
calls `loop()`, and sleeps for a bit.

The only mildly interesting thing about this is that it uses pthreads to
listen for input on `cin` on a second thread, because none of the approaches
I used to know for non-blocking input were working on `cin`.

To build & run this:

    $ g++ -pthread motion.c++
    $ ./a.out

And then hit return every second or so to simulate the IR signal.
