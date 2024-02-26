/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
#define buffsize 1
#define Numcoeff 5 //biquad cascade usa b0,b1,b2,a1,a2
#define Cascada 1 //cantidad de biquad a diseñar
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint16_t DATO_ADC= 0;
uint16_t DATO_DAC=0;
uint16_t prueba;
float32_t iir_coeff[Numcoeff]={0.25,0,0,1,-0.25};
float32_t y[3]={0};
int i=0;
int j=0;
float32_t iir_state[4*Cascada]={0};
float32_t iir_int,iir_out;
arm_biquad_casd_df1_inst_f32 S;
//A partir de acá definimos los filtros para luego copiar y pegar en el oficial
//Filtro 1
float32_t iir_coeff1[10]={0.05,0,0,0.9775,0,0.026,0,0,0.9775,0};
float32_t iir_state1[4*2]={0};
float32_t  iir_out1;
arm_biquad_casd_df1_inst_f32 S1;
//Filtro 2
float32_t iir_coeff2[10]={0.32,-0.32,0,0.9775,0,0.1,0.1,0,0.9582,0};
float32_t iir_state2[4*2]={0};
float32_t iir_out2;
arm_biquad_casd_df1_inst_f32 S2;
//Filtro 3
float32_t iir_coeff3[10]={0.147,-0.147,0,0.9582,0,0.41,0.41,0,0.918,0};
float32_t iir_state3[4*2]={0};
float32_t iir_out3;
arm_biquad_casd_df1_inst_f32 S3;
//Filtro 4
float32_t iir_coeff4[10]={0.96,-0.96,0,0.918,0,0.15,0.15,0,0.8425,0};
float32_t iir_state4[4*2]={0};
float32_t iir_out4;
arm_biquad_casd_df1_inst_f32 S4;
//Filtro 5
float32_t iir_coeff5[15]={0.3,-0.3,0,0.8425,0,0.6,0.6,0,0.708,0,0.2,0.2,0,0.708,0};
float32_t iir_state5[4*3]={0};
float32_t iir_out5;
arm_biquad_casd_df1_inst_f32 S5;
//Filtro 6
float32_t iir_coeff6[20]={0.6,-0.6,0,0.708,0,0.5,0.5,0,0.4747,0,0.4,0.4,0,0.4747,0,1,-1,0,0.708,0};
float32_t iir_state6[4*4]={0};
float32_t iir_out6;
arm_biquad_casd_df1_inst_f32 S6;
//Filtro 7
float32_t iir_coeff7[20]={1,-1,0,0.4747,0,0.5,0.5,0,0.168,0,0.9,-0.9,0,0.4747,0,0.5,0.5,0,0.168,0};
float32_t iir_state7[4*4]={0};
float32_t iir_out7;
arm_biquad_casd_df1_inst_f32 S7;
//Filtro 8
float32_t iir_coeff8[10]={1,-1,0,0.168,0,0.5,-0.5,0,0.168,0};
float32_t iir_state8[4*2]={0};
float32_t iir_out8;
arm_biquad_casd_df1_inst_f32 S8;

//filtros de media movil
float32_t fir_coeff[4] ={0.25,0.25,0.25,0.25};
arm_fir_instance_f32 fir_instance1;
float32_t fir_in_arm1, fir_out_arm1, fir_state1[4];
arm_fir_instance_f32 fir_instance2;
float32_t fir_in_arm2,fir_out_arm2, fir_state2[4];
arm_fir_instance_f32 fir_instance3;
float32_t fir_in_arm3,fir_out_arm3, fir_state3[4];
arm_fir_instance_f32 fir_instance4;
float32_t fir_in_arm4,fir_out_arm4, fir_state4[4];
arm_fir_instance_f32 fir_instance5;
float32_t fir_in_arm5,fir_out_arm5, fir_state5[4];
arm_fir_instance_f32 fir_instance6;
float32_t fir_in_arm6,fir_out_arm6, fir_state6[4];
arm_fir_instance_f32 fir_instance7;
float32_t fir_in_arm7,fir_out_arm7, fir_state7[4];
arm_fir_instance_f32 fir_instance8;
float32_t fir_in_arm8,fir_out_arm8, fir_state8[4];


//barras del vumetro
uint8_t barra1=1;
uint8_t barra2=1;
uint8_t barra3=1;
uint8_t barra4=1;
uint8_t barra5=1;
uint8_t barra6=1;
uint8_t barra7=1;
uint8_t barra8=1;
uint16_t promedio_n;
uint8_t flag_promedio;
uint8_t contador_adc;
float max1=1860;
float max2=1860;
float max3=1860;
float max4=1860;
float max5=1860;
float max6=1860;
float max7=1860;
float max8=1860;
float32_t estado1=1;
float32_t estado2=1;
float32_t estado3=1;
float32_t estado4=1;
float32_t estado5=1;
float32_t estado6=1;
float32_t estado7=1;
float32_t estado8=1;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
DATO_ADC = HAL_ADC_GetValue(&hadc1);
//HAL_GPIO_TogglePin(GPIOD, LD6_Pin);
HAL_GPIO_TogglePin(GPIOC,  GPIO_PIN_6);
//GPIOD->ODR = DATO_ADC;
iir_int = (float32_t)DATO_ADC;
if(i<10)
{GPIOD->ODR = 0;
i++;
	}
else
{
arm_biquad_cascade_df1_f32(&S,&iir_int,&iir_out,1);
/*Para usar los filtros
 *arm_biquad_cascade_df1_f32(&S1,&iir_int,&iir_out1,1);
 *arm_biquad_cascade_df1_f32(&S2,&iir_int,&iir_out2,1);
 *arm_biquad_cascade_df1_f32(&S3,&iir_int,&iir_out3,1);
 *arm_biquad_cascade_df1_f32(&S4,&iir_int,&iir_out4,1);
 *arm_biquad_cascade_df1_f32(&S5,&iir_int,&iir_out5,1);
 *arm_biquad_cascade_df1_f32(&S6,&iir_int,&iir_out6,1);
 *arm_biquad_cascade_df1_f32(&S7,&iir_int,&iir_out7,1);
 *arm_biquad_cascade_df1_f32(&S8,&iir_int,&iir_out8,1);
 */

}
DATO_DAC = (uint16_t)iir_out;
GPIOD->ODR = DATO_DAC;
contador_adc++;

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

//Una funcion para procesar los datos de los filtros
void valor_maximo(float32_t out1,float32_t out2,float32_t out3,float32_t out4,float32_t out5,float32_t out6,float32_t out7,float32_t out8){
/*Frecuencias maximas:
 * filtro 1=160 Hz
 * filtro 2=300 Hz
 * filtro 3=600 Hz
 * filtro 4= 1200 Hz
 * filtro 5= 2400 Hz
 * filtro 6= 5000 Hz
 * filtro 7= 10000 Hz
 * filtro 8= 20000 Hz
 * Con 16 puntos por senoidal de maxima frecuencia el algoritmo de busqueda de
 * valor maximo tiene una precision de abs(1-sen(pi/2+-pi/16))=1.92%<2%
 */
if(max8<out8){
	max8=out8;
}
if(max7<out7){
	max7=out7;
}
if(max6<out6){
	max6=out6;
}
if(max5<out5){
	max5=out5;
}
if((max4<out4)&(contador_adc%2==0)){
	max4=out4;
}
if((max3<out3)&(contador_adc%4==0)){
	max3=out3;
}
if((max2<out2)&(contador_adc%8==0)){
	max2=out2;
}
if((max1<out1)&(contador_adc%16==0)){
	max1=out1;
}
}

//Genera las barras antes de mandarlas, a su vez quiero que los datos sean
//suaves antes de mandarlos, asi que una media movil no estaria mal
void barras(void){

//primer arbol de ifs
if(max1>2977){
	if(max1>3536){
		if(max1>3815){
			fir_in_arm1=8;
		}
		else{
			fir_in_arm1=7;
		}
	}
	else{
		if(max1>3256){
			fir_in_arm1=6;
		}
		else{
			fir_in_arm1=5;
		}
	}
}
else{
	if(max1>2418){
		if(max1>2697){
			fir_in_arm1=4;
		}
		else{
			fir_in_arm1=3;
		}
     }
	else{
		if(max1>2139){
			fir_in_arm1=2;
		}
		else{
			fir_in_arm1=1;
		}
	}
}

//segundo arbol de ifs
if(max2>2977){
	if(max2>3536){
		if(max2>3815){
			fir_in_arm2=8;
		}
		else{
			fir_in_arm2=7;
		}
	}
	else{
		if(max2>3256){
			fir_in_arm2=6;
		}
		else{
			fir_in_arm2=5;
		}
	}
}
else{
	if(max2>2418){
		if(max2>2697){
			fir_in_arm2=4;
		}
		else{
			fir_in_arm2=3;
		}
     }
	else{
		if(max2>2139){
			fir_in_arm2=2;
		}
		else{
			fir_in_arm2=1;
		}
	}
}

//tercer arbol de ifs
if(max3>2977){
	if(max3>3536){
		if(max3>3815){
			fir_in_arm3=8;
		}
		else{
			fir_in_arm3=7;
		}
	}
	else{
		if(max3>3256){
			fir_in_arm3=6;
		}
		else{
			fir_in_arm3=5;
		}
	}
}
else{
	if(max3>2418){
		if(max3>2697){
			fir_in_arm3=4;
		}
		else{
			fir_in_arm3=3;
		}
     }
	else{
		if(max3>2139){
			fir_in_arm3=2;
		}
		else{
			fir_in_arm3=1;
		}
	}
}

//cuarto arbol de ifs
if(max4>2977){
	if(max4>3536){
		if(max4>3815){
			fir_in_arm4=8;
		}
		else{
			fir_in_arm4=7;
		}
	}
	else{
		if(max4>3256){
			fir_in_arm4=6;
		}
		else{
			fir_in_arm4=5;
		}
	}
}
else{
	if(max4>2418){
		if(max4>2697){
			fir_in_arm4=4;
		}
		else{
			fir_in_arm4=3;
		}
     }
	else{
		if(max4>2139){
			fir_in_arm4=2;
		}
		else{
			fir_in_arm4=1;
		}
	}
}

//quinto arbol de ifs
if(max5>2977){
	if(max5>3536){
		if(max5>3815){
			fir_in_arm5=8;
		}
		else{
			fir_in_arm5=7;
		}
	}
	else{
		if(max5>3256){
			fir_in_arm5=6;
		}
		else{
			fir_in_arm5=5;
		}
	}
}
else{
	if(max5>2418){
		if(max5>2697){
			fir_in_arm5=4;
		}
		else{
			fir_in_arm5=3;
		}
     }
	else{
		if(max5>2139){
			fir_in_arm5=2;
		}
		else{
			fir_in_arm5=1;
		}
	}
}

//sexto arbol de ifs
if(max6>2977){
	if(max6>3536){
		if(max6>3815){
			fir_in_arm6=8;
		}
		else{
			fir_in_arm6=7;
		}
	}
	else{
		if(max6>3256){
			fir_in_arm6=6;
		}
		else{
			fir_in_arm6=5;
		}
	}
}
else{
	if(max6>2418){
		if(max6>2697){
			fir_in_arm6=4;
		}
		else{
			fir_in_arm6=3;
		}
     }
	else{
		if(max6>2139){
			fir_in_arm6=2;
		}
		else{
			fir_in_arm6=1;
		}
	}
}

//septimo arbol de ifs
if(max7>2977){
	if(max7>3536){
		if(max7>3815){
			fir_in_arm7=8;
		}
		else{
			fir_in_arm7=7;
		}
	}
	else{
		if(max7>3256){
			fir_in_arm7=6;
		}
		else{
			fir_in_arm7=5;
		}
	}
}
else{
	if(max7>2418){
		if(max7>2697){
			fir_in_arm7=4;
		}
		else{
			fir_in_arm7=3;
		}
     }
	else{
		if(max7>2139){
			fir_in_arm7=2;
		}
		else{
			fir_in_arm7=1;
		}
	}
}

//octavo arbol de ifs
if(max8>2977){
	if(max8>3536){
		if(max8>3815){
			fir_in_arm8=8;
		}
		else{
			fir_in_arm8=7;
		}
	}
	else{
		if(max8>3256){
			fir_in_arm8=6;
		}
		else{
			fir_in_arm8=5;
		}
	}
}
else{
	if(max8>2418){
		if(max8>2697){
			fir_in_arm8=4;
		}
		else{
			fir_in_arm8=3;
		}
     }
	else{
		if(max8>2139){
			fir_in_arm8=2;
		}
		else{
			fir_in_arm8=1;
		}
	}
}

/*
	arm_fir_f32(&fir_instance1, &fir_in_arm1,&fir_out_arm1,1);
	arm_fir_f32(&fir_instance2, &fir_in_arm2,&fir_out_arm2,1);
	arm_fir_f32(&fir_instance3, &fir_in_arm3,&fir_out_arm3,1);
	arm_fir_f32(&fir_instance4, &fir_in_arm4,&fir_out_arm4,1);
	arm_fir_f32(&fir_instance5, &fir_in_arm5,&fir_out_arm5,1);
	arm_fir_f32(&fir_instance6, &fir_in_arm6,&fir_out_arm6,1);
	arm_fir_f32(&fir_instance7, &fir_in_arm7,&fir_out_arm7,1);
	arm_fir_f32(&fir_instance8, &fir_in_arm8,&fir_out_arm8,1);
*/

barra1=(uint8_t) fir_out_arm1;
barra2=(uint8_t) fir_out_arm2;
barra3=(uint8_t) fir_out_arm3;
barra4=(uint8_t) fir_out_arm4;
barra5=(uint8_t) fir_out_arm5;
barra6=(uint8_t) fir_out_arm6;
barra7=(uint8_t) fir_out_arm7;
barra8=(uint8_t) fir_out_arm8;
}

int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  arm_biquad_cascade_df1_init_f32(&S,Cascada,&iir_coeff[0],&iir_state[0]);
  //Para inicializar los filtros
  /*
   * arm_biquad_cascade_df1_init_f32(&S1,2,&iir_coeff1[0],&iir_state1[0]);
   * arm_biquad_cascade_df1_init_f32(&S2,2,&iir_coeff2[0],&iir_state2[0]);
   * arm_biquad_cascade_df1_init_f32(&S3,2,&iir_coeff3[0],&iir_state3[0]);
   * arm_biquad_cascade_df1_init_f32(&S4,2,&iir_coeff4[0],&iir_state4[0]);
   * arm_biquad_cascade_df1_init_f32(&S5,3,&iir_coeff5[0],&iir_state5[0]);
   * arm_biquad_cascade_df1_init_f32(&S6,4,&iir_coeff6[0],&iir_state6[0]);
   * arm_biquad_cascade_df1_init_f32(&S7,4,&iir_coeff7[0],&iir_state7[0]);
   * arm_biquad_cascade_df1_init_f32(&S8,2,&iir_coeff8[0],&iir_state8[0]);
   */
  //Para inicializar los filtros de media movil
  /*
  arm_fir_init_f32(&fir_instance1,4,fir_coeff,fir_state1,1);
  arm_fir_init_f32(&fir_instance2,4,fir_coeff,fir_state2,1);
  arm_fir_init_f32(&fir_instance3,4,fir_coeff,fir_state3,1);
  arm_fir_init_f32(&fir_instance4,4,fir_coeff,fir_state4,1);
  arm_fir_init_f32(&fir_instance5,4,fir_coeff,fir_state5,1);
  arm_fir_init_f32(&fir_instance6,4,fir_coeff,fir_state6,1);
  arm_fir_init_f32(&fir_instance7,4,fir_coeff,fir_state7,1);
  arm_fir_init_f32(&fir_instance8,4,fir_coeff,fir_state8,1);
  */


  HAL_ADC_Start_IT(&hadc1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1088-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : OTG_FS_PowerSwitchOn_Pin PC6 PC7 PC8 */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11
                           PD0 PD1 PD2 PD3
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
