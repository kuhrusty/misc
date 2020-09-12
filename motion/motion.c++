//  compile this with: g++ -pthread motion.c++

#include <time.h>
#include <pthread.h>
#include <iostream>

using namespace std;

//  how many ms we'll wait for a signal
const long SIGNAL_THRESHOLD_MS = 1200;
//  how many ms we'll wait before turning off the light
const long LIGHT_OFF_THRESHOLD_MS = 10000;

class LightStuff {
    public:
    bool lightIsOn = false;
    long expectSignalBy = 0;
    long turnOffLightAt = 0;

    bool gotSignal = false;
    long now = 0;  //  clock time in ms
};

static LightStuff ls;  //  look, it's just a proof-of-concept!

//  This is our pretend Arduino main loop; it gets called repeatedly by main().
void loop() {
    //cout << "loop:" <<
    //    "\n  now            " << ls.now <<
    //    "\n  expectSignalBy " << ls.expectSignalBy <<
    //    "\n  turnOffLightAt " << ls.turnOffLightAt <<
    //    "\n  light is " << (ls.lightIsOn ? "ON" : "OFF") << ", " <<
    //    (ls.gotSignal //? "GOT SIGNAL" : "no signal") << endl;
    if (ls.lightIsOn) {
        if (ls.gotSignal) {
            //  reset/extend timer
            ls.expectSignalBy = ls.now + SIGNAL_THRESHOLD_MS;
            if (ls.turnOffLightAt == 0) {
                //  this is our first signal since the light was turned on!
                cout << "Starting turn off timer" << endl;
                ls.turnOffLightAt = ls.now + LIGHT_OFF_THRESHOLD_MS;
            }
        } else if (ls.turnOffLightAt != 0) {
            if (ls.now >= ls.turnOffLightAt) {
                //  turn off light
                cout << "TURNING OFF LIGHT" << endl;
                ls.lightIsOn = false;
                ls.turnOffLightAt = 0;  //  clear timer
            } else if (ls.now >= ls.expectSignalBy) {
                //  signal is blocked again, so never mind, leave the light on!
                cout << "Stopping turn off timer" << endl;
                ls.turnOffLightAt = 0;  //  clear timer
            }
            //  else we're waiting to see whether we really do need to turn off
        }
        //  else we're not *expecting* a signal; do nothing
    } else {
        if (ls.gotSignal) {
            //  reset/extend timer
            ls.expectSignalBy = ls.now + SIGNAL_THRESHOLD_MS;
        } else if (ls.now >= ls.expectSignalBy) {
            //  we haven't received a signal; turn the light on
            cout << "TURNING ON LIGHT" << endl;
            ls.lightIsOn = true;
        }
        //  else light is off; we're just waiting for a signal
    }
}

//  We wait for keyboard input on another thread
void *waitForInput(void *tp) {
    char buf[80];
    do {
        cin.getline(buf, 80);
        ls.gotSignal = true;
    } while (cin.good());
    //  that croaks if we get a line longer than 80 characters--cin.getline()
    //  will have set cin.fail()--but we don't care.
    exit(0);  //  nuking entire program
}

int main(int argc, char **argv) {
    pthread_t inputThread;
    timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = 100000000;  //  tenth of a second
    //  If you want your loop to run very slowly:
    //sleeptime.tv_sec = 1;
    //  but also crank up SIGNAL_THRESHOLD_MS

    pthread_create(&inputThread, NULL, waitForInput, NULL);

    cout << "Hit return every second to simulate the IR light..." << endl;
    while (1) {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ls.now = (ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L);
        loop();
        ls.gotSignal = false;
        nanosleep(&sleeptime, NULL);
    }
    //  no pthread_join() because we don't care
    return 0;
}
