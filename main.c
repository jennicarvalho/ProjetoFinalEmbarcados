/*# ProjetoFinalEmbarcados
Códigos do projeto final para a cadeira de Projeto de Sistemas Embarcados. */

void configure_adc_temp(void)
{
 struct adc_config conf_adc;

 adc_get_config_defaults(&conf_adc);

 conf_adc.clock_source = GCLK_GENERATOR_1;
 conf_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
 conf_adc.reference = ADC_REFERENCE_INT1V;
 conf_adc.positive_input = ADC_POSITIVE_INPUT_TEMP;
 conf_adc.negative_input = ADC_NEGATIVE_INPUT_GND;
 conf_adc.sample_length = ADC_TEMP_SAMPLE_LENGTH;

 adc_init(&adc_instance, ADC, &conf_adc);

 ADC->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(2) | ADC_AVGCTRL_SAMPLENUM_4;

 adc_enable(&adc_instance);
}
/* This function return the Fine temperature value after done with
 calculation using Equation1 and Equation 1b as mentioned in data sheet
 on 36.9.8 Temperature Sensor Characteristics */
float calculate_temperature(uint16_t raw_code)
{
 float VADC; /* Voltage calculation using ADC result for Coarse Temp calculation */
 float VADCM; /* Voltage calculation using ADC result for Fine Temp calculation. */
 float INT1VM; /* Voltage calculation for reality INT1V value during the ADC conversion
*/

 VADC = ((float)raw_code * INT1V_VALUE_FLOAT)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;

 /* Coarse Temp Calculation by assume INT1V=1V for this ADC conversion */
 coarse_temp = tempR + (((tempH - tempR)/(VADCH - VADCR)) * (VADC - VADCR));

 /* Calculation to find the real INT1V value during the ADC conversion */
 INT1VM = INT1VR + (((INT1VH - INT1VR) * (coarse_temp - tempR))/(tempH - tempR));

 VADCM = ((float)raw_code * INT1VM)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;

 /* Fine Temp Calculation by replace INT1V=1V by INT1V = INT1Vm for ADC conversion */
 fine_temp = tempR + (((tempH - tempR)/(VADCH - VADCR)) * (VADCM - VADCR));

 return fine_temp;
}
void adc_temp_sensor(void)
{
 float temp;
 system_voltage_reference_enable(SYSTEM_VOLTAGE_REFERENCE_TEMPSENSE);

 configure_adc_temp();

 load_calibration_data();

 raw_result = adc_start_read_result();

 temp = calculate_temperature(raw_result);

 printf("\nThe current temperature is = %f degree Celsius", temp);
} 
