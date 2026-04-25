# Arduino State Machine Pt. 2

This project dynamically adjusts the brightness of an LED based on different light conditions. To measure ambient light, a photoresistor was used in combination with a 10kOhm resistor in a voltage divider configuration. A simple implementation would be to just handle everything in the main loop and use blocking delays, but just like with the **Arduino State Machine** project, I wanted this system to work in a non blocking way. To achieve this, I structured the program to work using a Finite State Machine. Each state defines the behavior of the LED and handles transitions to other states. Scroll down to the bottom to see a live demo of the circuit. Also, this project uses direct register access and manipulation to set the different timer configurations.


## The Voltage Divider Circuit

I needed a way to "detect" light changes in real time so a photoresistor seemed like the perfect component. Its behavior can be modeled as an inverse relationship between resistance and light, as one goes low, the other one increases. Pairing this component with another similar sized resistor in series, and connecting an ADC pin in the middle of both allowed me to measure the voltage drop and map that to an LED brightness level. Figure 1 shows the circuit configuration used for this setup.

<img width="964" height="551" alt="circuit" src="https://github.com/user-attachments/assets/09c3e0bb-ae55-4fa4-8135-28ebbb27a995" />
Figure 1. Circuit diagram

The ESP32-S3 has 2 12 bit ADCs, each with 9 channels mapped to different GPIO pins. Using this set up, ADC 1 channel 0 (GPIO pin 1) was reading ~50 under very low brightness, ~2000 normal room light, and ~3500 when I pointed my phone's flashlight directly to it. With this information, I was able to set the desired thresholds at which the LED changed brightness.


## The Finite State Machine Implementation

The system has 3 operational states (not counting None or Reset). Each state represents a different LED brightness. The brightness was controlled by adjusting the PWM duty cycle of the LEDC peripheral connected to the pin. The transitions are defined by the ADC readings. I decided to leave the LED in its dimmest state when the ADC read below 1000 (poorly lit room). For the medium brightness state, the ADC must read between 1000 and 2000, and for the brightest state, the ADC must continuously read greater than 2000. 

When a certain threshold is met, the LED brightness changes for a minimum amount of time. At the lowest level, the LED will stay for at least 1 second, 2 seconds at the medium level, and 3 seconds at the highest level. The different thresholds, transitions, and stay times are explained in more detail in figure 2.  

<img width="1920" height="809" alt="FSM" src="https://github.com/user-attachments/assets/fd75dc88-7f7c-4574-86f9-c4102174c42d" />
Figure 2. FSM diagram

Each state was modeled using functions. The led_low() implements state 1, led_mid() state 2, and led_high() state 3. All 3 functions handle similar behavior, each following this sequence of actions:
- State initialization: If this is the first time entering this state, the system sets the appropriate timer configuration and lights up the LED according to the state rules.
  - State 1 (low brightness): Timer reset and updated to measure 1 second. PWM duty cycle set to 10%.
  - State 2 (medium brightness): Timer reset and updated to measure 2 seconds. PWM duty cycle set to 40%.
  - State 3 (highest brightness): Timer reset and updated to measure 3 seconds. PWM duty cycle set to 100%.
- Perform state tasks: This is pretty much identical for every state. 
  - Get current ADC value.
  - Write to update timer register to get a snapshot of the counter.
- Check for state transitions:
  - First check if enough time has passed (1 second for state 1, seconds for state 2, 3 seconds for state 3), then check if the current ADC value is greater/less than the transition threshold. If both conditions are met, update the state variable.
- State deinitialization: If during the previous state transition check the system determined the conditions to switch were met, the current state deinitializes the timer counter and also resets the timer for the next state to set according to its needs.


## Direct Register Manipulation

One of the things I want to start learning more about is direct hardware interaction using registers and the reference manual. Most of my projects rely on vendor HAL and I want to change that. I believe interfacing with registers is going to teach me many useful skills as an embedded programmer and could optimize future systems I work on. I have done simple things using registers such as setting a GPIO pin in output mode and writing to its data register to set the output value, but I want to do more, and this felt like the perfect opportunity to start working towards that goal. Because of that, no Arduino functions were used to set up the timer. For this project, the timer is configured like this:
- Use APB clock as the source clock (80 MHz).
- Prescaler = 80.
- Autoreload enabled.
- Time-base counter increment in every clock tick.


## Live Demo

https://drive.google.com/file/d/1fvwNyRyslhK-v88SE2rAjCgz9pLuiJlF/edit



