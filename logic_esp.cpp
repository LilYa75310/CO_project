#include "logic_esp.h"

Adafruit_ADS1015 ads;

float volts_midArifm_pwmSig(int num_countdowns = 100) { //напруга на транзисторі
  float sum = 0;
  int16_t adc;
  float volts;                      
  for (int i = 0; i < num_countdowns; i++)  
    sum += ads.readADC_SingleEnded(pin_voltageOntransistor);                  
  volts = ads.computeVolts(sum / num_countdowns);
  return volts;
}

void dutyCycleOfPWM() { // калібруємо напругу на транзисторія для режиму вимірювання
  const int freq = 100;
  const int ledChannel = 0;
  const int resolution = 8;
  bool flag_correct_dutyCycle = 0;
  
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(PWM_SIG, ledChannel);

  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    ledcWrite(ledChannel, dutyCycle);
      //delay(50);
      
      float volt_level = volts_midArifm_pwmSig();
      Serial.println(volt_level); 
      if(volt_level >= 3.55 && volt_level <= 3.6 )
      {
        DutyCycle = dutyCycle; // ALERT!!!!! CHANGING GLOBAL VAR!!!!!!!!!!!!!!!!!!!
        Serial.print("voltage: ");  
        Serial.println(volt_level);
        Serial.println(DutyCycle);
        flag_correct_dutyCycle = 1;
        break;
      }
      
    }
    if(flag_correct_dutyCycle == 0)
    {
      Serial.println("not correct DutyCycle. Pls restart CO_analisator");
      while(1);
    }
}

int calib_mq7_Ro(int pin_voltageOn)
{
  float mq7_adc_volts;
  int mq7_adc;
  int Rs;
  int Ro;

  mq7_adc = ads.readADC_SingleEnded(pin_voltageOn);
  mq7_adc_volts = ads.computeVolts(mq7_adc);
  Rs = (((float)MQ7_REFERENCE_VOLTAGE * (float)MQ7_LOAD_RESISTOR)/mq7_adc_volts)- (float)MQ7_LOAD_RESISTOR;
  Ro = Rs/MQ7_CLEANAIR_RSRO;

  Serial.print("Rs: ");
  Serial.print(Rs);
  Serial.print("; volt_adc: ");
  Serial.println(mq7_adc_volts);
  
  return Ro;
}

float get_rawValue_mq7(int Ro, float reference_voltage_value, float pinCO)
{ 
  int ppm;
  int Rs;
  float RsRo;
  float mq7_adc_volts;
  int mq7_adc;
  mq7_adc = ads.readADC_SingleEnded(pinCO);
  mq7_adc_volts = ads.computeVolts(mq7_adc);
  Rs = (((float)MQ7_REFERENCE_VOLTAGE * (float)MQ7_LOAD_RESISTOR)/mq7_adc_volts)- (float)MQ7_LOAD_RESISTOR;
  RsRo = (float)Rs/(float)Ro; 
  if(RsRo>0.09) {
    ppm=(pow((0.3/RsRo),(1/1.3)))*1000;
  }
  
  else {ppm=3999;}

  return (float)ppm;

}
