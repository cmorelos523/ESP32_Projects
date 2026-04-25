/**
 * @file      Lab_2_Part_4.ino
 * @brief     This code ajusts the duty cycle of the LED based on light conditions
 * @author    Carlos Morelos
 * @date      04/22/2026
 * @version   1.0
 */


// ================================ Includes ================================
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"
#include "soc/timer_group_reg.h"


// ================================ Macros ================================
#define LED_PIN     36
#define ADC_PIN     1
#define LEDC_FREQ   5000

// LED Low (State 1): Duty cycle 10% -> 410
#define L_SLOW_DUTY   410
// LED Mid (State 2): Duty cycle 40% -> 2048
#define L_MID_DUTY    1638
// LED High (State 3): Duty cycle 100% -> 4095 
#define L_HIGH_DUTY   4095

// Transition thresholds
#define LED_LOW_TH    1000
#define LED_MID_TH    2000  

// Reset value defined in user manual
#define TIM0_RST    0x60002000
// Prescaler value of 80, left shifted 13 times
#define TIM0_PRESC  0xA0000
// 1 second stay in LED Low state
#define L_LOW_STAY  1000000
// 2 second stay in LED Mid state
#define L_MID_STAY  2000000
// 3 second stay in LED High state
#define L_HIGH_STAY 3000000


// ================================ Global Variables ================================
// FSM states
enum states
{
  NONE,
  LED_LOW,
  LED_MID,
  LED_HIGH
};

// Current and prior state
states prior_state;
states curr_state;

// Time trackers
unsigned long current_time = 0;

// Global timer config
uint32_t tim0_config = TIM0_RST | TIM0_PRESC | TIMG_T0_EN;


// ================================ Function Prototypes ================================
/**
 * @brief   Sets the state where the LED is at its lowest brightness
 * @param   None
 * @return  None
 */
void led_low();

/**
 * @brief   Sets the state where the LED is at its medium brightness
 * @param   None
 * @return  None
 */
void led_mid();

/**
 * @brief   Sets the state where the LED is at its highest brightness
 * @param   None
 * @return  None
 */
void led_high();


// ================================ Setup and Loop ================================
void setup() {
  // Set up LEDC pin
  ledcAttach(LED_PIN, LEDC_FREQ, 12);

  // Set states
  prior_state = NONE;
  curr_state = LED_LOW;
}

void loop() {
  switch(curr_state)
  {
    case LED_LOW:
      led_low();
      break;
    case LED_MID:
      led_mid();
      break;
    case LED_HIGH:
      led_high();
      break;
  }
}


// ================================ Function Implementation ================================
/**
 * @brief   Sets the state where the LED is at its lowest brightness
 * @param   None
 * @return  None
 */
void led_low()
{
  uint16_t adc_read = 0;
  static unsigned long led_low_time = 0;

  // State initialization
  if (curr_state != prior_state)
  {
    // Set the timer
    *((volatile uint32_t*)TIMG_T0CONFIG_REG(0)) = tim0_config;

    // Update timer to get snapshot
    *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) |= TIMG_T0_UPDATE;
    led_low_time = *((volatile uint32_t*)TIMG_T0LO_REG(0));  

    // Light up LED (low brightness)
    ledcWrite(LED_PIN, L_SLOW_DUTY);

    // Synchronize state variables
    prior_state = curr_state;
  }

  // State tasks
  // Get current ADC value
  adc_read = analogRead(ADC_PIN);
  // Update timer to get snapshot
  *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) |= TIMG_T0_UPDATE;  
  current_time = *((volatile uint32_t*)TIMG_T0LO_REG(0));

  // State transitions
  // Check if enough time has passed and if there is enough light to transition to the medium brightness state
  if ((current_time >= led_low_time + L_LOW_STAY) &&
      (adc_read > LED_LOW_TH))
  {
    curr_state = LED_MID;
  }

  // If we are leaving the state, reset timer and current time
  if (curr_state != prior_state)
  {
    current_time = 0;
    *((volatile uint32_t*)TIMG_T0CONFIG_REG(0)) = TIM0_RST;
  }
}


/**
 * @brief   Sets the state where the LED is at its medium brightness
 * @param   None
 * @return  None
 */
void led_mid()
{
  uint16_t adc_read = 0;
  static unsigned long led_mid_time = 0;

  // State initialization
  if (curr_state != prior_state)
  {
    // Set the timer
    *((volatile uint32_t*)TIMG_T0CONFIG_REG(0)) = tim0_config;

    // Update timer to get snapshot
    *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) |= TIMG_T0_UPDATE;
    led_mid_time = *((volatile uint32_t*)TIMG_T0LO_REG(0));  

    // Light up LED (medium brightness)
    ledcWrite(LED_PIN, L_MID_DUTY);

    // Synchronize state variables
    prior_state = curr_state;
  }

  // State tasks
  // Get current ADC value
  adc_read = analogRead(ADC_PIN);
  // Update timer to get snapshot
  *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) |= TIMG_T0_UPDATE;  
  current_time = *((volatile uint32_t*)TIMG_T0LO_REG(0));

  // State transitions
  // Check if enough time has passed and if there is enough light to transition to the brightest state
  if ((current_time >= led_mid_time + L_MID_STAY) &&
      (adc_read > LED_MID_TH))
  {
    curr_state = LED_HIGH;
  }

  // Check if enough time has passed and if there is not enough light to stay in this state
  if ((current_time >= led_mid_time + L_MID_STAY) &&
      (adc_read < LED_LOW_TH))
  {
    curr_state = LED_LOW;
  }

  // If we are leaving the state, reset timer and current time
  if (curr_state != prior_state)
  {
    current_time = 0;
    *((volatile uint32_t*)TIMG_T0CONFIG_REG(0)) = TIM0_RST;
  }
}


/**
 * @brief   Sets the state where the LED is at its highest brightness
 * @param   None
 * @return  None
 */
void led_high()
{
  uint16_t adc_read = 0;
  static unsigned long led_high_time = 0;

  // State initialization
  if (curr_state != prior_state)
  {
    // Set the timer
    *((volatile uint32_t*)TIMG_T0CONFIG_REG(0)) = tim0_config;

    // Update timer to get snapshot
    *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) |= TIMG_T0_UPDATE;
    led_high_time = *((volatile uint32_t*)TIMG_T0LO_REG(0));  

    // Light up LED (high brightness)
    ledcWrite(LED_PIN, L_HIGH_DUTY);

    // Synchronize state variables
    prior_state = curr_state;
  }

  // State tasks
  // Get current ADC value
  adc_read = analogRead(ADC_PIN);
  // Update timer to get snapshot
  *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) |= TIMG_T0_UPDATE;  
  current_time = *((volatile uint32_t*)TIMG_T0LO_REG(0));

  // State transitions
  // Check if enough time has passed and if there is not enough light to stay in this state
  if ((current_time >= led_high_time + L_HIGH_STAY) &&
      (adc_read < LED_MID_TH))
  {
    curr_state = LED_MID;
  }

  // If we are leaving the state, reset timer and current time
  if (curr_state != prior_state)
  {
    current_time = 0;
    *((volatile uint32_t*)TIMG_T0CONFIG_REG(0)) = TIM0_RST;
  }
}



















