#include "smart_coaster.h"

#define ULP_DATA_OFFSET 36
_Static_assert(ULP_DATA_OFFSET < CONFIG_ULP_COPROC_RESERVE_MEM/4 - 6,
"ULP_DATA_OFFSET is set too high, or CONFIG_ULP_COPROC_RESERVE_MEM is not sufficient");

int initialize_ULP(int adc_pin, uint ge_value, uint le_value) {
  // Clear the part of RTC_SLOW_MEM reserved for the ULP. Makes debugging easier.
  memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
  const int gpio_num = 0;
  const int adc = 0;
  const int channel = 6;
  const int atten = 3;
  assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

  SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR1_BIT_WIDTH, 3, SENS_SAR1_BIT_WIDTH_S);
  SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR2_BIT_WIDTH, 3, SENS_SAR2_BIT_WIDTH_S);

  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_BIT, 0x3, SENS_SAR1_SAMPLE_BIT_S);
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_SAMPLE_BIT, 0x3, SENS_SAR2_SAMPLE_BIT_S);

  CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_FORCE);
  CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_FORCE);

  SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
  SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_AMP, 2, SENS_FORCE_XPD_AMP_S);

  // SAR1 invert result
  SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR1_DATA_INV);
  SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR2_DATA_INV);


  //rtc_gpio_init(gpio_num);
  CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD_FORCE_M);
  CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD_FORCE_M);

  SET_PERI_REG_BITS(SENS_SAR_ATTEN1_REG, 3, atten, 2 * channel); //set SAR1 attenuation
  SET_PERI_REG_BITS(SENS_SAR_ATTEN2_REG, 3, atten, 2 * channel); //set SAR2 attenuation

  const uint32_t measurements_per_sec = 1;
  const ulp_insn_t program[] = {
       // Read ADC and put into R0
       I_ADC(R0, adc, channel),
       M_BL(1, le_value),
       M_BGE(1, ge_value),
       M_LABEL(0),
        I_HALT(),
       M_LABEL(1),
        // initiate wakeup of the SoC
        I_WAKE(),
        // stop the ULP program
        I_HALT()
   };
   // Load ULP program into RTC_SLOW_MEM, at offset 0
   size_t size = sizeof(program)/sizeof(ulp_insn_t);
   ESP_ERROR_CHECK( ulp_process_macros_and_load(0, program, &size) );
   assert(size < ULP_DATA_OFFSET && "ULP_DATA_OFFSET needs to be greater or equal to the program size");

   // Set ULP wakeup period
   const uint32_t sleep_cycles = rtc_clk_slow_freq_get_hz() / measurements_per_sec;
   REG_WRITE(SENS_ULP_CP_SLEEP_CYC0_REG, sleep_cycles);

   // Start ULP
   ESP_ERROR_CHECK(ulp_run(0));
   esp_sleep_enable_ulp_wakeup();
   esp_deep_sleep_start();
}
