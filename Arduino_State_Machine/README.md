# Arduino State Machine

This project focuses on controlling multiple LEDs with tactile switches. There are multiple ways to do this, but I wanted this system to work in a non blocking way, meaning no delay(). To achieve this, I structured the program to work using a Finite State Machine. Each state has transitions and defines the behavior of the LEDs. Scroll down to the bottom of this document to see a live demo of the circuit.

## State Diagram

Legend: L means LED (L is blinking, L with the bar on top is off)
		B means button (B is not pressed or logic 1, B with the bar on top is pressed or logic 0)

<img width="799" height="421" alt="FSM" src="https://github.com/user-attachments/assets/0ed3ebbc-d2d8-46a7-8e2b-76f6eee1cc28" />


## State Description

### None
This is the default we enter when the **RESET** button is pressed. The machine automatically transitions into the next state.

### State 1 (S1)
Initial state, here, both LEDs blink every 500ms. If **Button 1** is pressed, we transition to S2 where **LED 1** is off and **LED 2** is blinking. On the other hand, if **Button 2** is pressed, we transition to S3, where **LED 1** is now blinking and **LED 2** is off.

### State 2 (S2)
In this state **LED 1** is off and **LED 2** is blinking. If **Button 1** is pressed again we transition back to S1 where both LEDs are flashing. If **Button 2** is pressed however, both LEDs will be off.

## State 3 (S3)
This state is the opposite of S2, **LED 1** is blinking while **LED 2** is off. If **Button 2** is pressed again we transition back to S1 where both LEDs are flashing. If **Button 1** is pressed however, both LEDs will be off.

## State 4 (S4)
In this state both LEDs are off. If **Button 1** is pressed, we transition back to S3 where **LED 1** is blinking while **LED 2** is off. If **Button 2** is pressed, we transition back to S2 where **LED 1** is off and **LED 2** is blinking.


## Notes

I faced too many weird bugs with bouncing buttons, so I had to add a debouncing mechanism in software. The approach is time based and really simple. I defined a struct to hold the tracked pieces:
- The physical pin where the button is
- The debounced (stable) state
- The last raw reading
- The timestamp of the last raw change
Essentially what I did was report the reading once it's held steady for long enough, in my case, I set the window to 100ms. The function **read_button()** walks through that logic.

I saw online that a debouncing circuit can be done in hardware using an SR latch or an RC debouncing circuit. Unfortunately, I don't have any extra capacitors with which is why I had to go with the software solution.


## Live Demo

https://drive.google.com/file/d/1jI8Dj5LPjdWLCo-mCJyhEIhjF5VkLQrp/view?usp=sharing

