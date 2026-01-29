#include "main.h"
#include "cmsis_os.h"
#include "postfix.h"
#include "find_root.h"
#include "key_board.h"
#include "ssd1306.h"
#include "fonts.h"
#include <stdio.h>
#include <math.h>
#include <FreeRTOSConfig.h>

typedef struct {
    Token *postfix;
    float result;
} ThreadData;

I2C_HandleTypeDef hi2c1;


#define NUM_THREADS 3



void SystemClock_Config(void);
static void MX_I2C1_Init(void);
static void MX_GPIO_Init(void);

void StartNewtonTask(void const *argument);
void StartBisectionTask(void const *argument);
void StartSecantTask(void const *argument);
void StartDefautTask(void const *argument);

float newtonRaphson(Token *postfix);
float bisectionMethod(Token *postfix);
float secantMethod(Token *postfix);

// Task handles

/*osThreadId newtonTaskHandle;
osThreadId bisectionTaskHandle;
osThreadId secantTaskHandle;*/

osThreadId taskHandles[NUM_THREADS];  
osMutexDef(myMutex);
osMutexId mutexHandle;


Token *output;
ThreadData threadData[3];

volatile int found = 0;

volatile float best_result = 0.0;

//char str[MAX];


/*void cancel_other_tasks(osThreadId currentTaskId) {
    if (newtonTaskHandle != currentTaskId) {
        osThreadTerminate(newtonTaskHandle);
    }
    if (bisectionTaskHandle != currentTaskId) {
        osThreadTerminate(bisectionTaskHandle);
    }
    if (secantTaskHandle != currentTaskId) {
        osThreadTerminate(secantTaskHandle);
    }
}*/
void cancel_other_tasks(osThreadId currentTaskId) {
    for (int i = 0; i < NUM_THREADS; i++) {
        if (taskHandles[i] != currentTaskId) { 
            osThreadTerminate(taskHandles[i]);
            osDelay(100); // Delay giúp h? th?ng c?p nh?t
        }
    }
}

//uint32_t startTick;

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    SSD1306_Init();
    SSD1306_Clear();
    uint16_t x = 0, y = 10;
	
    SSD1306_Fill(SSD1306_COLOR_BLACK);
   printString(0, 0, "Nhap bt");
		while (1) {
        char key = scanKeypad();
        if (key != 0) {
            char* str = printKey(&x, &y, key);
            if (str != NULL) {
                output = infixToPostfix(str);
                break;
            }
        }
        HAL_Delay(50);
    }

		//output = infixToPostfix(str);
			
		//startTick = osKernelSysTick();  

    if (output != NULL) {
        mutexHandle = osRecursiveMutexCreate(osMutex(myMutex));

        threadData[0].postfix = output;
        osThreadDef(NewtonTask, StartNewtonTask, 3 , 0, 512);
        taskHandles[0] = osThreadCreate(osThread(NewtonTask), &threadData[0]);

        threadData[1].postfix = output;
        osThreadDef(BisectionTask, StartBisectionTask, 3, 0, 512);
        taskHandles[1] = osThreadCreate(osThread(BisectionTask), &threadData[1]);

        threadData[2].postfix = output;
        osThreadDef(SecantTask, StartSecantTask, 3, 0, 512);
        taskHandles[2] = osThreadCreate(osThread(SecantTask), &threadData[2]);  
    }
    osKernelStart();
}

void StartNewtonTask(void const *argument) {
		uint32_t startTick = osKernelSysTick();
    ThreadData *data = (ThreadData *)argument;		
    data->result = newtonRaphson(data->postfix);
	  osRecursiveMutexWait(mutexHandle, osWaitForever);
    if (!found && !isnan(data->result)) {
        best_result = data->result;
        found = 1;
				SSD1306_Clear();
				uint32_t endTick = osKernelSysTick();  
				uint32_t elapsedTime = endTick - startTick;
				float elapsedTime_ms = (float)elapsedTime / 10000.0f;
				printString(0, 0 , "Newton:");
				printFloat(best_result, 0, 10);
				char timeStr[20];
				snprintf(timeStr, sizeof(timeStr), "Time: %.4f s", elapsedTime_ms);
				printString(0, 20, timeStr);
				char timeStr1[40];
				float k = evaluatePostfix(data->postfix, best_result);
				snprintf(timeStr1, sizeof(timeStr1), "Kq %.2f: %.4f", best_result,fabs(k));
				printString(0, 30, timeStr1);
			  cancel_other_tasks(osThreadGetId());  
		}
		
		osRecursiveMutexRelease(mutexHandle);
    osThreadTerminate(osThreadGetId());
}

void StartBisectionTask(void const *argument) {
		uint32_t startTick = osKernelSysTick();
    ThreadData *data = (ThreadData *)argument;
    data->result = bisectionMethod(data->postfix);
		osRecursiveMutexWait(mutexHandle, osWaitForever);
		
    if (!found && !isnan(data->result)) {
        best_result = data->result;
        found = 1;
        SSD1306_Clear();
				uint32_t endTick = osKernelSysTick();  
				uint32_t elapsedTime = endTick - startTick;
				float elapsedTime_ms = (float)elapsedTime  / 10000.0f;
        printString(0, 0, "Bisection:");
        printFloat(best_result, 0, 10);
				char timeStr[20];
				snprintf(timeStr, sizeof(timeStr), "Time: %.4f s", elapsedTime_ms);
				printString(0, 20, timeStr);
				char timeStr1[40];
				float k = evaluatePostfix(data->postfix, best_result);
				snprintf(timeStr1, sizeof(timeStr1), "Kq %.2f: %.4f", best_result,fabs(k));
				printString(0, 30, timeStr1);
			  cancel_other_tasks(osThreadGetId());
         
    }
		
    osRecursiveMutexRelease(mutexHandle);
    osThreadTerminate(osThreadGetId());

}

void StartSecantTask(void const *argument) {
		uint32_t startTick = osKernelSysTick();
    ThreadData *data = (ThreadData *)argument;
    data->result = secantMethod(data->postfix);
		osRecursiveMutexWait(mutexHandle, osWaitForever);
	
    if (!found && !isnan(data->result) ) {
        best_result = data->result;
        found = 1;
        SSD1306_Clear();
				uint32_t endTick = osKernelSysTick();  
				uint32_t elapsedTime = endTick - startTick;
				float elapsedTime_ms = (float)elapsedTime  / 10000.0f;
        printString(0, 0, "Secant:");
        printFloat(best_result, 0, 10);
				char timeStr[20];
				snprintf(timeStr, sizeof(timeStr), "Time: %.4f s", elapsedTime_ms);
				printString(0, 20, timeStr);
				char timeStr1[40];
				float k = evaluatePostfix(data->postfix, best_result);
				snprintf(timeStr1, sizeof(timeStr1), "Kq %.2f: %.4f", best_result,fabs(k));
				printString(0, 30, timeStr1);
			  cancel_other_tasks(osThreadGetId());
    }
		
    osRecursiveMutexRelease(mutexHandle);
    osThreadTerminate(osThreadGetId());
}

float newtonRaphson(Token *postfix) {
    float x0 = 0.0;
		//int p = 20;
    for (int i = 0; i < 100000  ; i++) {
				if (found) { // Ki?m tra found
            return NAN;
        }
				//printString(0,p,"1");
			  //p+=10;
        float f = evaluatePostfix(postfix, x0);
        float df = derivativePostfix(postfix, x0);
        if (fabs(f) < 1e-10) {
            return x0;
        } else {
            x0 = x0 - f / df;
        }
    }
    return NAN;
}

float bisectionMethod(Token *postfix) {
		float a,b;
    find_isolation_interval(postfix, &a, &b);
    float c;
		//int j=20;
    for (int m = 0; m < 100000 ; m++) {	
				if (found) { // Ki?m tra found
            return NAN;
        }
			  //printString(10,j,"2");
				//j+=10;
        c = (a + b) / 2;
        float f_c = evaluatePostfix(postfix, c);
        if (fabs(f_c) < 1e-7) {
            return c;
        }
        if (evaluatePostfix(postfix, a) * f_c < 0) {
            b = c;
        } else {
            a = c;
        }
    }
    return NAN;
}

float secantMethod(Token *postfix) {
	  float a, b;
    find_isolation_interval(postfix, &a, &b);
    float x0  = a, x1 = b, x2;
		//int q=20;
    for (int i = 0; i < 100000; i++) {
				if (found) { // Ki?m tra found
            return NAN;
        }
				//printString(20,q,"3");
				//q+=10;
        float f_x0 = evaluatePostfix(postfix, x0);
        float f_x1 = evaluatePostfix(postfix, x1);
        x2 = x1 - f_x1 * (x1 - x0) / (f_x1 - f_x0);
        if (fabs(x2 - x1) < 1e-7) {
            return x2;
        }
        x0 = x1;
        x1 = x2;
    }
    return NAN;
}



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}

static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
		osSystickHandler(); 
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
