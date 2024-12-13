#include "modes/Smash64.hpp"

#define ANALOG_STICK_MIN 0
#define ANALOG_STICK_NEUTRAL 128
#define ANALOG_STICK_MAX 255

bool c_aerial_allowed = true;
int aerial_timer = 0; //increments to TIME before resetting
 //5 works well on console but doesn't consistently output aerials on emulator
//ideally this would be 5 when in console mode and 20 on emulator

Smash64::Smash64(socd::SocdType socd_type, Smash64Options options) {
    _socd_pair_count = 4;
    _socd_pairs = new socd::SocdPair[_socd_pair_count]{
        socd::SocdPair{&InputState::left,    &InputState::right,   socd_type},
        socd::SocdPair{ &InputState::down,   &InputState::up,      socd_type},
        socd::SocdPair{ &InputState::c_left, &InputState::c_right, socd_type},
        socd::SocdPair{ &InputState::c_down, &InputState::c_up,    socd_type},
    };

    _options = options;
}

void Smash64::UpdateDigitalOutputs(InputState &inputs, OutputState &outputs) {
    outputs.a = inputs.a;
    outputs.b = inputs.b;
    outputs.x = inputs.x;
    outputs.y = inputs.y;
    outputs.buttonR = inputs.r || inputs.l; //buttonR is shield in Smash64
    if (inputs.nunchuk_connected) {
        // Lightshield with C button.
        if (inputs.nunchuk_c) {
            outputs.triggerLAnalog = 49;
        }
        outputs.triggerLDigital = inputs.nunchuk_z;
    } else {
        outputs.triggerLDigital = inputs.lightshield; //triggerLDigital = taunt in 64
    }

    outputs.triggerRDigital = inputs.z; // triggerRDigital is grab in 64
    outputs.start = inputs.start;
    outputs.select = inputs.select;
    outputs.home = inputs.home;
    outputs.buttonL = inputs.midshield;

    // Activate D-Pad layer by holding Mod X + Mod Y.
    if (inputs.mod_x && inputs.mod_y) {
        outputs.dpadUp = inputs.c_up;
        outputs.dpadDown = inputs.c_down;
        outputs.dpadLeft = inputs.c_left;
        outputs.dpadRight = inputs.c_right;
    }
}



void Smash64::UpdateAnalogOutputs(InputState &inputs, OutputState &outputs) {
    // Coordinate calculations to make modifier handling simpler.
    UpdateDirections(
        inputs.left,
        inputs.right,
        inputs.down,
        inputs.up,
        /*
        inputs.c_left,
        inputs.c_right,
        inputs.c_down,
        inputs.c_up,
        */
       //the following 4 lines are replacing
       //the previous 4 lines of code
       //to see if they give c-inputs for jumping (it does)
        inputs.x,
        inputs.y,
        inputs.midshield,
        inputs.home, //dummy input; does nothing unless you have home button - then it's a n64 cUp on console
        ANALOG_STICK_MIN,
        ANALOG_STICK_NEUTRAL,
        ANALOG_STICK_MAX,
        outputs
    );

    bool shield_button_pressed = inputs.r || inputs.l;
    
    if (!inputs.c_left && !inputs.c_right && !inputs.c_down && !inputs.c_up) {
        c_aerial_allowed = true;
        aerial_timer = 0;
    }

   if (directions.diagonal) { //added this conditional to give joystick accurate diagonals
    outputs.leftStickX = 128 + (directions.x * 85);
    outputs.leftStickY = 128 + (directions.y * 85);
   }



    if (inputs.mod_x) {
        if (directions.horizontal) {
            outputs.leftStickX = 128 + (directions.x * 50); //56 gives dash
        }

        if(directions.vertical) {
            outputs.leftStickY = 128 + (directions.y * 50); 
        }
        
            /* Up B Angles */
        if (directions.diagonal && !shield_button_pressed) {
            // (x, y), (73, 31), (66~, 19~) [coords, code_values, in-game values]
            //gave this a lower magnitude than the following angles
            //so that forward tilts could be angled up and down
            outputs.leftStickX = 128 + (directions.x * 53); //changed to 53 to prevent forward smash attacks with diag mod X
            outputs.leftStickY = 128 + (directions.y * 19);
            // (x, y), (89, 50), (84~, 41~) [coords, code_values, in-game values]
            if (inputs.c_down) {
                outputs.leftStickX = 128 + (directions.x * 84);
                outputs.leftStickY = 128 + (directions.y * 41);
            }
            // (x, y), (85, 55), (80~, 47~) [coords, code_values, in-game values]
            if (inputs.c_left) {
                 outputs.leftStickX = 128 + (directions.x * 80);
                 outputs.leftStickY = 128 + (directions.y * 47);
            }
            // (x, y), (81, 60), (75~, 52~) [coords, code_values, in-game values]
            if (inputs.c_up) {
                outputs.leftStickX = 128 + (directions.x * 75);
                outputs.leftStickY = 128 + (directions.y * 52);
            }
            // (x, y), (76, 65), (70~, 58~) [coords, code_values, in-game values]
            if (inputs.c_right) {
                outputs.leftStickX = 128 + (directions.x * 70);
                outputs.leftStickY = 128 + (directions.y * 58);
            }
        }
    }

    if (inputs.mod_y) {
        if (directions.horizontal) {
            outputs.leftStickX = 128 + (directions.x * 21); //21 in-game. slowest walk is 8-25. 20 required to turnaround
        }

        if(directions.vertical) {
            outputs.leftStickY = 128 + (directions.y * 86); //86 in-game
        }
        /* Up B Angles */
        if (directions.diagonal && !shield_button_pressed) {
            // (x, y), (47, 92), (38~, 88~) [coords, code_values, in-game values]
            outputs.leftStickX = 128 + (directions.x * 38);
            outputs.leftStickY = 128 + (directions.y * 88);
            // (x, y), (50, 89), (40~, 84~) [coords, code_values, in-game values]
            if (inputs.c_down) {
                outputs.leftStickX = 128 + (directions.x * 40);
                outputs.leftStickY = 128 + (directions.y * 84);
            }
            // (x, y), (55, 85), (47~, 80~) [coords, code_values, in-game values]
            if (inputs.c_left) {
                outputs.leftStickX = 128 + (directions.x * 47);
                outputs.leftStickY = 128 + (directions.y * 80);
            }
            // (x, y), (60, 81), (51~, 75~) [coords, code_values, in-game values]
            if (inputs.c_up) {
                outputs.leftStickX = 128 + (directions.x * 51);
                outputs.leftStickY = 128 + (directions.y * 75);
            }
            // (x, y), (65, 76), (58~, 70~) [coords, code_values, in-game values]
            if (inputs.c_right) {
                outputs.leftStickX = 128 + (directions.x * 58);
                outputs.leftStickY = 128 + (directions.y * 70);
            }
        }
    }

    //C-Stick Aerials
    //Description of solution:
    //Needed C input to output direction + A but ONLY for a very short amount of time
    //otherwise, it influences the player's movement and aerial drift
    //the timer allows the A + direction output to persist long enough for the game
    //to read it. before I added the timer, rapidly pressing a C direction would
    //only intermittently output an aerial. 
    if (!inputs.mod_x && !inputs.mod_y) {
        if (inputs.c_left && c_aerial_allowed) {
            aerial_timer++;
            outputs.leftStickX = 128 + (-1 * 22); //*50 = 41 in-game. 56 required for Smash Attacks.
            outputs.leftStickY = 128;             //change 50 to 65 in each for Smash Stick instead of Tilt Stick//65 might cause fastfalls or tap jumps, will have to test.
            outputs.a = true;
            if (aerial_timer == _options.c_stick_time) {             //20 is min for outputting aerials, trying 22
                c_aerial_allowed = false;
                aerial_timer = 0;
            }
        }

        if (inputs.c_right && c_aerial_allowed) {
            aerial_timer++;
            outputs.leftStickX = 128 + (1 * 22);
            outputs.leftStickY = 128;
            outputs.a = true;
            if (aerial_timer == _options.c_stick_time) {
                c_aerial_allowed = false;
                aerial_timer = 0;
            }
        }

        if (inputs.c_down && c_aerial_allowed) {
            aerial_timer++;
            outputs.leftStickX = 128;
            outputs.leftStickY = 128 + (-1 * 22);
            outputs.a = true;
            if (aerial_timer == _options.c_stick_time) {
                c_aerial_allowed = false;
                aerial_timer = 0;
            }
        }

        if (inputs.c_up && c_aerial_allowed) {
            aerial_timer++;
            outputs.leftStickX = 128;
            outputs.leftStickY = 128 + (1 * 22);
            outputs.a = true;
            if (aerial_timer == _options.c_stick_time) {
                c_aerial_allowed = false;
                aerial_timer = 0;
            }
        }
    }
    // Shut off C-stick when using D-Pad layer.
    if (inputs.mod_x && inputs.mod_y) { //this probably isn't necessary w/ other buttons being rightstick inputs
        outputs.rightStickX = 128;
        outputs.rightStickY = 128;
    }

    // Nunchuk overrides left stick.
    if (inputs.nunchuk_connected) {
        outputs.leftStickX = inputs.nunchuk_x;
        outputs.leftStickY = inputs.nunchuk_y;
    }
}