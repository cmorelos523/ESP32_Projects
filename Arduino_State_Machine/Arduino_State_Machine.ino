/**
 * @file      Lab_1_Part_3.ino
 * @brief     This file controls the blinking of two external LEDs based on button presses
 * @author    Carlos Morelos
 * @date      04/12/2026
 * @version   1.0
 */


// ================================ Includes ================================


// ================================ Macros ================================
// Number of output pins
#define OUTPUT_PINS         2
// Number of input pins
#define INPUT_PINS          2
// Pin functions
#define LED_PIN_1           21
#define LED_PIN_2           47
#define BUTTON_PIN_1        48
#define BUTTON_PIN_2        35
// Blink interval
#define BLINK_INTERVAL_MS   500
// Debounce time
#define DEBOUNCE_MS         100


// ================================ Global Variables ================================
// Array of output pins
uint8_t output_pins[OUTPUT_PINS] = {LED_PIN_1, LED_PIN_2};
// Array of input pins
uint8_t input_pins[INPUT_PINS] = {BUTTON_PIN_1, BUTTON_PIN_2};

// State machine states
enum states 
{
  NONE,
  BOTH_ON,
  L1_OFF_L2_ON,
  L1_ON_L2_OFF,
  BOTH_OFF,
};

// Current and prior states
states prior_state;
states state;

// Time variables to keep track of time
uint32_t current_time = 0;

// Button debounce struct
typedef struct 
{
  uint8_t pin;
  uint8_t stable_state;
  uint8_t last_raw_state;
  uint32_t last_changed_time;
} debounced_button;

// Initialize debounced button struct
debounced_button btn_1 = 
{
  BUTTON_PIN_1,
  HIGH,
  HIGH,
  0
};

debounced_button btn_2 = 
{
  BUTTON_PIN_2,
  HIGH,
  HIGH,
  0
};

// ================================ Function Prototypes ================================
/**
 * @brief   Sets the pins in the output_pins array to output mode
 * @param   pins: Pointer to the start of the array
 * @return  None
 */
void set_output_pins(uint8_t *pins);

/**
 * @brief   Sets the pins in the input_pins array to input mode
 * @param   pins: Pointer to the start of the array
 * @return  None
 */
void set_input_pins(uint8_t *pins);

/**
 * @brief   Sets the pins in the output_pins array to logic 1
 * @param   pins: Pointer to the start of the array
 * @return  None
 */
void set_pins_high(uint8_t *pins);

/**
 * @brief   Sets the pins in the output_pins array to logic 0
 * @param   pins: Pointer to the start of the array
 * @return  None
 */
void set_pins_low(uint8_t *pins);

/**
 * @brief   Sets both LEDs in blinking mode
 * @param   None
 * @return  None
 */
void both_on();

/**
 * @brief   Sets the state where LED 1 is off and LED 2 is on
 * @param   None
 * @return  None
 */
void l1_off_l2_on();

/**
 * @brief   Sets the state where LED 2 is off and LED 1 is on
 * @param   None
 * @return  None 
 */
void l1_on_l2_off();

/**
 * @brief   Sets the state where both LEDs are off
 * @param   None
 * @return  None 
 */
void both_off();


/**
 * @brief   Reads the raw button value and debounces it in software
 * @param   button: Button we wish to debounce
 * @return  Debounced signal
 */
uint8_t read_button(debounced_button *button);

// ================================ Setup and Loop ================================
void setup()
{
  // Set input/outputs
  set_output_pins(output_pins);
  set_input_pins(input_pins);
  set_pins_low(output_pins);

  // Set states
  prior_state = NONE;
  state = BOTH_ON;
}


void loop()
{
  switch(state)
  {
    case BOTH_ON:
      both_on();
      break;
    case L1_OFF_L2_ON:
      l1_off_l2_on();
      break;
    case L1_ON_L2_OFF:
      l1_on_l2_off();
      break;
    case BOTH_OFF:
      both_off();
      break;
  }
}


// ================================ Function Implementation ================================
/**
 * @brief   Sets the pins in the output_pins array to output mode
 * @param   pins: Pointer to the start of the array
 */
void set_output_pins(uint8_t *pins)
{
  // Iterate through output pins array and set mode
  for (uint8_t i = 0; i < OUTPUT_PINS; i++)
  {
    pinMode(output_pins[i], OUTPUT);
  }
}


/**
 * @brief   Sets the pins in the input_pins array to input mode
 * @param   pins: Pointer to the start of the array
 */
void set_input_pins(uint8_t *pins)
{
  for (uint8_t i = 0; i < INPUT_PINS; i++)
  {
    pinMode(input_pins[i], INPUT);
  }
}


/**
 * @brief   Sets the pins in the output_pins array to logic 1
 * @param   pins: Pointer to the start of the array
 */
void set_pins_high(uint8_t *pins)
{
  for (uint8_t i = 0; i < OUTPUT_PINS; i++)
  {
    digitalWrite(output_pins[i], HIGH);
  }
}


/**
 * @brief   Sets the pins in the output_pins array to logic 0
 * @param   pins: Pointer to the start of the array
 */
void set_pins_low(uint8_t *pins)
{
  for (uint8_t i = 0; i < OUTPUT_PINS; i++)
  {
    digitalWrite(output_pins[i], LOW);
  }
}


/**
 * @brief   Sets both LEDs in blinking mode
 * @param   None
 * @return  None
 */
void both_on()
{
  // Internal variable to keep track of time
  uint32_t s1_time = 0;

  // If entering state, do init steps
  if (state != prior_state)
  {
    prior_state = state;
    set_pins_high(output_pins);
    current_time = millis();
  }

  // State Tasks
  // Blink both LEDs
  s1_time = millis();

  // If enough time has passed, update and toggle
  if (s1_time >= current_time + BLINK_INTERVAL_MS)
  {
    current_time = millis();
    digitalWrite(LED_PIN_1, !digitalRead(LED_PIN_1));
    digitalWrite(LED_PIN_2, !digitalRead(LED_PIN_2));
  }

  // Check for state transitions
  if (!read_button(&btn_1) && read_button(&btn_2))
  {
    // Button 1 pressed, turn LED 1 off
    state = L1_OFF_L2_ON;
  }

  if (read_button(&btn_1) && !read_button(&btn_2))
  {
    // Button 2 pressed, turn LED 2 off
    state = L1_ON_L2_OFF;
  }

  // If we are leaving the state, do cleanup steps
  if (state != prior_state)
  {
    set_pins_low(output_pins);
  }
}


/**
 * @brief   Sets the state where LED 1 is off and LED 2 is on
 * @param   None
 * @return  None
 */
void l1_off_l2_on()
{
  // Internal time variable to keep track of time
  uint32_t s2_time = 0;

  // If entering state do init steps
  if (prior_state != state)
  {
    prior_state = state;
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, HIGH);
    current_time = millis();
  }

  // State Tasks
  // Blink only LED 2
  s2_time = millis();

  // If enough time has passed, toggle LED 2
  if (s2_time >= current_time + BLINK_INTERVAL_MS)
  {
    current_time = millis();
    digitalWrite(LED_PIN_2, !digitalRead(LED_PIN_2));
  }

  // Check for state transitions
  if (!read_button(&btn_1) && read_button(&btn_2))
  {
    // Go back to state 1, both LEDs on
    state = BOTH_ON;
  }

  if (read_button(&btn_1) && !read_button(&btn_2))
  {
    // Button 2 low, both LEDs off
    state = BOTH_OFF;
  }

  // If we are leaving the state, do cleanup steps
  if (state != prior_state)
  {
    set_pins_low(output_pins);
  }
}


/**
 * @brief   Sets the state where LED 2 is off and LED 1 is on
 * @param   None
 * @return  None 
 */
void l1_on_l2_off()
{
  // Internal time variable to keep track of time
  uint32_t s3_time = 0;

  // If entering state do init steps
  if (prior_state != state)
  {
    prior_state = state;
    digitalWrite(LED_PIN_1, HIGH);
    digitalWrite(LED_PIN_2, LOW);
    current_time = millis();
  }

  // State Tasks
  // Blink only LED 1
  s3_time = millis();

  // If enough time has passed, toggle LED 1
  if (s3_time >= current_time + BLINK_INTERVAL_MS)
  {
    current_time = millis();
    digitalWrite(LED_PIN_1, !digitalRead(LED_PIN_1));
  }

  // Check for state transitions
  if (!read_button(&btn_1) && read_button(&btn_2))
  {
    // Both LEDs off
    state = BOTH_OFF;
  }

  if (read_button(&btn_1) && !read_button(&btn_2))
  {
    // Go back to initial state, both LEDs on
    state = BOTH_ON;
  }

  // If we are leaving the state, do cleanup steps
  if (state != prior_state)
  {
    set_pins_low(output_pins);
  }
}


/**
 * @brief   Sets the state where both LEDs are off
 * @param   None
 * @return  None 
 */
void both_off()
{
  // If entering state, do init steps
  if (state != prior_state)
  {
    prior_state = state;
    set_pins_low(output_pins);
    current_time = millis();
  }

  // State Tasks
  // None, both LEDs are off

  // Check for state transitions
  if (!read_button(&btn_1) && read_button(&btn_2))
  {
    // Button 1 pressed, turn LED 1 on
    state = L1_ON_L2_OFF;
  }

  if (read_button(&btn_1) && !read_button(&btn_2))
  {
    // Button 2 pressed, turn LED 2 on
    state = L1_OFF_L2_ON;
  }

  // If we are leaving the state, do cleanup steps
  if (state != prior_state)
  {
    set_pins_low(output_pins);
  }
}



/**
 * @brief   Reads the raw button value and debounces it in software
 * @param   button: Button we wish to debounce
 * @return  Debounced signal
 */
uint8_t read_button(debounced_button *button)
{
  uint8_t raw = digitalRead(button -> pin);

  // If the raw reading changed, reset the timer
  if (raw != button -> last_raw_state)
  {
    button -> last_raw_state = raw;
    button -> last_changed_time = millis();
  }

  // Check if the signal has been stable long enough
  if ((millis() - button -> last_changed_time) >= DEBOUNCE_MS)
  {
    button -> stable_state = raw;
  }

  return button -> stable_state;
}

