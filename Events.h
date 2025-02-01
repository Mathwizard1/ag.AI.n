#pragma once
#include "gameFunction.h"

class EventManager {
private:
    // Internal function to set the event timer
    void startEvent(int timer);

    // Function to execute events based on the event timer
    void executeEvent();

    // Event functions
    void event1();
    void event2();

    void event3();

    void event4();
    void event5();

public:
    int eventTimer = 0;
    short currentEvent = 0;

    // Function to check for and trigger an event
    void checkEvent();
};

