#ifndef SMART_COASTER_H
#define SMART_COASTER_H

#include <CircularBuffer.h>
#include <Adafruit_NeoPixel.h>

#include <string.h>
#include "esp32/ulp.h"
#include "esp_sleep.h"
#include "esp_err.h"
#include "esp_deep_sleep.h"
#include "esp_attr.h"
#include "rom/rtc.h"
#include "rom/ets_sys.h"
#include "driver/rtc_io.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
#include <esp_adc_cal.h>

int initialize_ULP(int fsr_pin, uint ge_value, uint le_value);

#endif /* SMART_COASTER_H */
