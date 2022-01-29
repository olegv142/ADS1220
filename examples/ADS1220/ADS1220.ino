#include "ADS1220.h"

#define ADC_CS_PIN  10
#define ADC_RDY_PIN A0

ADS1220 g_adc(ADC_CS_PIN, ADC_RDY_PIN);

bool setup_adc(ads1220_inp_mode_t inp)
{
  if (!g_adc.reset() || !g_adc.probe())
    return false;
  g_adc.set_input_mode(inp, ads_gain1, true);
  // start in turbo mode with 2ksps rate
  g_adc.set_conv_mode(ads_1000sps, ads_dr_turbo, true);
  g_adc.start();
  return true;
}

void setup()
{
  Serial.begin(9600);

  g_adc.begin();
  if (setup_adc(ads_inp_01))
    Serial.println("ADS ok");
  else {
    Serial.println("ADS not found!");
  }
}

void loop()
{
  if (!g_adc.wait_data(1000)) {
    Serial.println("ADS failed");
  } else {
    Serial.println(g_adc.rd_data());
  }
  delay(1000);
}
