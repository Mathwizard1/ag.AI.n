#include "Events.h"
#include <cstdlib>

    void EventManager::checkEvent() {
        // If no event is currently running
        if (eventTimer <= 0) {
            int randomNumber = rand() % 100 + 1;

            // Based on random number, trigger an event and set its timer
            if (randomNumber <= 20) {
                startEvent(5);
                std::cout << "Starting Event 1\n";
            }
            else if (randomNumber <= 40) {
                startEvent(4);
                std::cout << "Starting Event 2\n";
            }
            else if (randomNumber <= 60) {
                startEvent(6);
                std::cout << "Starting Event 3\n";
            }
            else if (randomNumber <= 80) {
                startEvent(3);
                std::cout << "Starting Event 4\n";
            }
            else if (randomNumber <= 100) {
                startEvent(7);
                std::cout << "Starting Event 5\n";
            }
        }

        // If an event is currently running
        else
        {
            // Execute the current event
            executeEvent();

            // Decrease the timer
            --eventTimer;
        }
    }


    // Internal function to set the event timer
    void EventManager::startEvent(int timer) {
        eventTimer = timer;
    }

    // Function to execute events based on the event timer
    void EventManager::executeEvent() {
        if (eventTimer > 0) {
            if (eventTimer % 5 == 0) {
                event1();
            }
            else if (eventTimer % 4 == 0) {
                event2();
            }
            else if (eventTimer % 3 == 0) {
                event3();
            }
            else if (eventTimer % 2 == 0) {
                event4();
            }
            else {
                event5();
            }
        }
    }

    // Event functions
    void EventManager::event1() {
        std::cout << "Event 1\n";
    }

    void EventManager::event2() {
        std::cout << "Event 2\n";
    }

    void EventManager::event3() {
        std::cout << "Event 3\n";
    }

    void EventManager::event4() {
        std::cout << "Event 4\n";
    }

    void EventManager::event5() {
        std::cout << "Event 5\n";
    }
