#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "radioenge_modem.h"
#include "uartRingBufDMA.h"

extern osTimerId_t PeriodicSendTimerHandle;
extern osThreadId_t AppSendTaskHandle;
extern ADC_HandleTypeDef hadc1;
extern osEventFlagsId_t ModemStatusFlagsHandle;
extern TIM_HandleTypeDef htim3;

// IP REGISTRO: 10.10.3.20:8080
// R�DIO......: 8166
// ID ........: 00 12 F8 00 00 00 08 A6
// KEY .......: C2 D5 90 92 88 6A 42 4C 08 40 EE 42 B3 95 38 58
// SUBSCRIBE..: application/5/device/0012f800000008a6/#

/*
void AppSendTaskCode(void *argument)
{
    // USER CODE BEGIN 5
    // Infinite loop

    while (1)
    {
        LoRaWaitDutyCycle();
        // write code to read from sensors and send via LoRaWAN

        osDelay(30000);
    }
}
*/

// AULA 04 - ATIVIDADE 02
typedef struct
{
    uint32_t seq_no;
    int32_t temp_oCx100;
} __attribute__((packed)) TEMPERATURE_OBJ_t;

// AULA O4 - ATIVIDADE 03
typedef struct
{
    uint16_t compressor_power;
    uint8_t warning_status;
} __attribute__((packed)) ARCONDICIONADO_OBJ_t;

void PeriodicSendTimerCallback(void *argument)
{
}

// AULA 04 - IMPLEMENTA��O PWM
void LoRaWAN_RxEventCallback(uint8_t *data, uint32_t length, uint32_t port, int32_t rssi, int32_t snr)
{
    ARCONDICIONADO_OBJ_t *ar_cond = data;
    uint16_t i;
    ar_cond->compressor_power;
    ar_cond->warning_status;

    i = ar_cond->compressor_power;

    htim3.Instance->CCR2 = (htim3.Instance->ARR * i) / 100;
}

// AULA 03A - ENVIO DE COMANDOS AT E FILAS EM RTOS
extern osMessageQueueId_t TemperatureQueueHandle;
char msg[256];

void AppSendTaskCode(void *argument)
{
    uint32_t read;
    osStatus_t status;
    uint8_t sendStr[64];

    // AULA 03 - read LM35 Temperature
    // int32_t temp;

    // AULA 04 - Dados bin�rios
    TEMPERATURE_OBJ_t temp;
    temp.seq_no = 0;

    while (1)
    {
        // read LM35 Temperature
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 100);
        read = HAL_ADC_GetValue(&hadc1);

        /*
        //AULA 03
        temp = (int32_t)(33000 * ((float)read / 4096));
        sprintf(sendStr, "Temperature: %2d.%2d", temp/100, temp%100);
        LoRaSend(5,sendStr);
        */

        // AULA 04
        temp.seq_no++;
        temp.temp_oCx100 = (int32_t)(33000 * ((float)read / 4096));
        LoRaSendB(2, (uint8_t *)&temp, sizeof(TEMPERATURE_OBJ_t));

        osDelay(15000);

        // status = osMessageQueueGet(TemperatureQueueHandle, &temp_oCx100,
        //                             NULL, osWaitForever); // wait for message
        //  if (status == osOK)
        //  {
        //        sprintf(msg, "Temperature: %d.%d oC\n", temp_oCx100 / 100,
        //      temp_oCx100 % 100);
        //  SendToUART(msg, strlen(msg));
        //  }
    }
}

void ReadFromADCTask(void *argument)
{
    uint32_t read;
    int32_t temp_oCx100;
    while (1)
    {
        // read LM35 Temperature
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 100);
        read = HAL_ADC_GetValue(&hadc1);
        temp_oCx100 = (int32_t)(33000 * ((float)read / 4096));
        // Send Message
        osMessageQueuePut(TemperatureQueueHandle, &temp_oCx100, 0U,
                          osWaitForever);
        osDelay(200);
    }
}

void PWMTaskFunction(void *argument)
{

}