/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : Estacion de Audio Ritmica
  * @author          : Matias Cruzate
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//Defino el valor para el ciclo activo del cero y el uno
//Se escriben en el capture compare register (pulse)
#define Neopixel_cero 38
#define Neopixel_uno 76
//Ahora veo la cantidad de pixels, osea leds, de mi matriz
#define Num_pixels 128
// Por cada uno tengo que enviar 24 bits mas 50 us de cero
#define DMA_BUFF_SIZE (Num_pixels*24+50)


//La idea es armar un arreglo de datos de 24 bits, 8 por cada color GRB, para mandar
typedef union Datos_shift
{
	//Hacer con aritmetrica de puntero
	uint32_t Datos32;
	uint16_t Datos16[2];
	uint8_t Datos8[4];
} datos;
uint8_t banda1=1;
uint8_t banda2=1;
uint8_t banda3=1;
uint8_t banda4=1;
uint8_t banda5=1;
uint8_t banda6=1;
uint8_t banda7=1;
uint8_t banda8=1;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
DMA_HandleTypeDef hdma_tim4_ch2;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int i=0;
float32_t iir_int;
//A partir de acá definimos los filtros para luego copiar y pegar en el oficial
//Filtro 1
float32_t iir_coeff1[5]={0.02254,0,0,0.9775,0};
float32_t iir_state1[4]={0};
float32_t  iir_out1;
arm_biquad_casd_df1_inst_f32 S1;
//Filtro 2
float32_t iir_coeff2[10]={0.5,-0.5,0,0.9775,0,0.1,0.1,0,0.9582,0};
float32_t iir_state2[4*2]={0};
float32_t iir_out2;
arm_biquad_casd_df1_inst_f32 S2;
//Filtro 3
float32_t iir_coeff3[10]={0.5,-0.5,0,0.9582,0,0.25,0.25,0,0.918,0};
float32_t iir_state3[4*2]={0};
float32_t iir_out3;
arm_biquad_casd_df1_inst_f32 S3;
//Filtro 4
float32_t iir_coeff4[10]={1,-1,0,0.918,0,0.3,0.3,0,0.8425,0};
float32_t iir_state4[4*2]={0};
float32_t iir_out4;
arm_biquad_casd_df1_inst_f32 S4;
//Filtro 5
float32_t iir_coeff5[15]={0.5,-0.5,0,0.8425,0,0.6,0.6,0,0.708,0,0.25,0.25,0,0.708,0};
float32_t iir_state5[4*3]={0};
float32_t iir_out5;
arm_biquad_casd_df1_inst_f32 S5;
//Filtro 6
float32_t iir_coeff6[20]={1,-1,0,0.708,0,0.5,0.5,0,0.4747,0,0.5,0.5,0,0.4747,0,1,-1,0,0.708,0};
float32_t iir_state6[4*4]={0};
float32_t iir_out6;
arm_biquad_casd_df1_inst_f32 S6;
//Filtro 7
float32_t iir_coeff7[20]={1,-1,0,0.4747,0,1,1,0,0.168,0,1,-1,0,0.4747,0,0.5,0.5,0,0.168,0};
float32_t iir_state7[4*4]={0};
float32_t iir_out7;
arm_biquad_casd_df1_inst_f32 S7;
//Filtro 8
float32_t iir_coeff8[10]={1,-1,0,0.168,0,0.75,-0.75,0,0.168,0};
float32_t iir_state8[4*2]={0};
float32_t iir_out8;
arm_biquad_casd_df1_inst_f32 S8;

uint8_t contador_adc;
float max1=0;
float max2=0;
float max3=0;
float max4=0;
float max5=0;
float max6=0;
float max7=0;
float max8=0;
uint8_t caso_laser=0;
uint8_t prueba_pwm=0;
uint32_t pwm_H = 15;
uint8_t flag_pwm =0;
uint8_t flag2_pwm =0;
uint32_t pwm_L = 185;
uint16_t pwm_pulse;

uint16_t pwmDatos[DMA_BUFF_SIZE] = {0};
uint8_t Datos_Pixel[Num_pixels][4];
uint8_t Datos_Pixel_F[Num_pixels][4];

//Me armo una funcion que setee cada LED por separado
void Set_Pixel(uint8_t n, uint8_t Verde, uint8_t Rojo, uint8_t Azul)
{
	Datos_Pixel[n][0] = n;
	Datos_Pixel[n][1] = Verde;
	Datos_Pixel[n][2] = Rojo;
	Datos_Pixel[n][3] = Azul;

}


void vumetro(uint8_t banda1,uint8_t banda2,uint8_t banda3,uint8_t banda4,uint8_t banda5,uint8_t banda6,uint8_t banda7,uint8_t banda8){
//Hay que tener en cuenta que cada banda del vumetro son dos lineas de la matriz, pero
//por como se recorre una esta invertida
//colores: violeta, rosa, azul, dos verdes, amarillo, dos rojos
int numled=0;

switch(banda1){
case 1||0:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,76,0,153);
	for(uint16_t j=9;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	for(uint16_t j=10;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	for(uint16_t j=11;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	for(uint16_t j=12;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	for(uint16_t j=13;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	for(uint16_t j=14;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 7:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,0,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,0,0);
	break;
default:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,204,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,204,0);
	break;
}

numled=16;
switch(banda2){
case 1||0:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,76,0,153);
	for(uint16_t j=9;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	for(uint16_t j=10;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	for(uint16_t j=11;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	for(uint16_t j=12;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	for(uint16_t j=13;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	for(uint16_t j=14;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 7:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,0,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,0,0);
	break;
default:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,204,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,204,0);
	break;
}

numled=32;
switch(banda3){
case 1||0:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,76,0,153);
	for(uint16_t j=9;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	for(uint16_t j=10;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	for(uint16_t j=11;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	for(uint16_t j=12;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	for(uint16_t j=13;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	for(uint16_t j=14;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 7:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,0,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,0,0);
	break;
default:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,204,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,204,0);
	break;
}

numled=48;
switch(banda4){
case 1||0:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,76,0,153);
	for(uint16_t j=9;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	for(uint16_t j=10;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	for(uint16_t j=11;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	for(uint16_t j=12;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	for(uint16_t j=13;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	for(uint16_t j=14;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 7:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,0,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,0,0);
	break;
default:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,204,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,204,0);
	break;
}



numled=64;
switch(banda5){
case 1:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,76,0,153);
	for(uint16_t j=9;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	for(uint16_t j=10;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	for(uint16_t j=11;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	for(uint16_t j=12;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	for(uint16_t j=13;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	for(uint16_t j=14;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 7:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,0,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,0,0);
	break;
case 8:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,204,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,204,0);
	break;
}

numled=80;
switch(banda6){
case 1:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,76,0,153);
	for(uint16_t j=9;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	for(uint16_t j=10;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	for(uint16_t j=11;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	for(uint16_t j=12;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	for(uint16_t j=13;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	for(uint16_t j=14;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 7:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,0,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,0,0);
	break;
case 8:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,204,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,204,0);
	break;
}

numled=96;
switch(banda7){
case 1:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,76,0,153);
	for(uint16_t j=9;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	for(uint16_t j=10;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	for(uint16_t j=11;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	for(uint16_t j=12;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	for(uint16_t j=13;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	for(uint16_t j=14;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 7:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,0,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,0,0);
	break;
case 8:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,204,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,204,0);
	break;
}

numled=112;
switch(banda8){
case 1:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,76,0,153);
	for(uint16_t j=9;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	for(uint16_t j=10;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	for(uint16_t j=11;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	for(uint16_t j=12;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	for(uint16_t j=13;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=7;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	for(uint16_t j=14;j<=15;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	break;
case 7:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,0,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,0,0);
	break;
case 8:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	Set_Pixel(6+numled,0,204,0);
	Set_Pixel(7+numled,0,204,0);
	Set_Pixel(8+numled,0,76,153);
	Set_Pixel(9+numled,0,153,153);
	Set_Pixel(10+numled,102,0,204);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(13+numled,255,255,51);
	Set_Pixel(14+numled,0,204,0);
	Set_Pixel(15+numled,0,204,0);
	break;
}
}


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

//Genera las barras antes de mandarlas
void barras(void){

//primer arbol de ifs
if(max1>1840){
	if(max1>2020){
		if(max1>2100){
			banda1=8;
		}
		else{
			banda1=7;
		}
	}
	else{
		if(max1>1920){
			banda1=6;
		}
		else{
			banda1=5;
		}
	}
}
else{
	if(max1>1690){
		if(max1>1760){
			banda1=4;
		}
		else{
			banda1=3;
		}
     }
	else{
		if(max1>1640){
			banda1=2;
		}
		else{
			banda1=1;
		}
	}
}

//segundo arbol de ifs
if(max2>440){
	if(max2>630){
		if(max2>700){
			banda2=8;
		}
		else{
			banda2=7;
		}
	}
	else{
		if(max2>550){
			banda2=6;
		}
		else{
			banda2=5;
		}
	}
}
else{
	if(max2>260){
		if(max2>350){
			banda2=4;
		}
		else{
			banda2=3;
		}
     }
	else{
		if(max2>150){
			banda2=2;
		}
		else{
			banda2=1;
		}
	}
}

//tercer arbol de ifs
if(max3>760){
	if(max3>1080){
		if(max3>1200){
			banda3=8;
		}
		else{
			banda3=7;
		}
	}
	else{
		if(max3>940){
			banda3=6;
		}
		else{
			banda3=5;
		}
	}
}
else{
	if(max3>450){
		if(max3>610){
			banda3=4;
		}
		else{
			banda3=3;
		}
     }
	else{
		if(max3>200){
			banda3=2;
		}
		else{
			banda3=1;
		}
	}
}

//cuarto arbol de ifs
if(max4>760){
	if(max4>1080){
		if(max4>1200){
			banda4=8;
		}
		else{
			banda4=7;
		}
	}
	else{
		if(max4>930){
			banda4=6;
		}
		else{
			banda4=5;
		}
	}
}
else{
	if(max4>450){
		if(max4>600){
			banda4=4;
		}
		else{
			banda4=3;
		}
     }
	else{
		if(max4>200){
			banda4=2;
		}
		else{
			banda4=1;
		}
	}
}

//quinto arbol de ifs
if(max5>850){
	if(max5>1180){
		if(max5>1300){
			banda5=8;
		}
		else{
			banda5=7;
		}
	}
	else{
		if(max5>980){
			banda5=6;
		}
		else{
			banda5=5;
		}
	}
}
else{
	if(max5>450){
		if(max5>700){
			banda5=4;
		}
		else{
			banda5=3;
		}
     }
	else{
		if(max5>250){
			banda5=2;
		}
		else{
			banda5=1;
		}
	}
}

//sexto arbol de ifs
if(max6>650){
	if(max6>880){
		if(max6>1000){
			banda6=8;
		}
		else{
			banda6=7;
		}
	}
	else{
		if(max6>730){
			banda6=6;
		}
		else{
			banda6=5;
		}
	}
}
else{
	if(max6>350){
		if(max6>500){
			banda6=4;
		}
		else{
			banda6=3;
		}
     }
	else{
		if(max6>200){
			banda6=2;
		}
		else{
			banda6=1;
		}
	}
}

//septimo arbol de ifs
if(max7>650){
	if(max7>950){
		if(max7>1050){
			banda7=8;
		}
		else{
			banda7=7;
		}
	}
	else{
		if(max7>830){
			banda7=6;
		}
		else{
			banda7=5;
		}
	}
}
else{
	if(max7>360){
		if(max7>520){
			banda7=4;
		}
		else{
			banda7=3;
		}
     }
	else{
		if(max7>200){
			banda7=2;
		}
		else{
			banda7=1;
		}
	}
}

//octavo arbol de ifs
if(max8>600){
	if(max8>800){
		if(max8>900){
			banda8=8;
		}
		else{
			banda8=7;
		}
	}
	else{
		if(max8>700){
			banda8=6;
		}
		else{
			banda8=5;
		}
	}
}
else{
	if(max8>350){
		if(max8>500){
			banda8=4;
		}
		else{
			banda8=3;
		}
     }
	else{
		if(max8>200){
			banda8=2;
		}
		else{
			banda8=1;
		}
	}
}

}

//Me armo una función que envíe los datos desde DMA a neopixeles

void Enviar_datos (void)
{
// Para el programa final trabajar a 24 fps
	//uint32_t datos;
	datos datos;
	uint16_t index = 0;


	for (int i = 0; i<Num_pixels; i++)
	{
		//si no funciona tambien deberia andar el codigo siguiente
		datos.Datos8[0]=Datos_Pixel[i][3];
		datos.Datos8[1]=Datos_Pixel[i][2];
		datos.Datos8[2]=Datos_Pixel[i][1];


		for (int i = 23; i>=0; i--)
		{

			if (datos.Datos32&(1<<i) ) pwmDatos[index] = Neopixel_uno; //ver counter&(1<<j)
			else pwmDatos[index] = Neopixel_cero;
			index++;

		}

	}

	for (int i=0; i<50;i++)
	{
		pwmDatos[index] = 0;
		index++;
	}



	//Envio la informacion con esta funcion de la HAL
/*		if(HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_2,(uint32_t *)pwmDatos,index)!=HAL_OK)
			{
			Error_Handler();
			};
			*/
	HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_2,(uint32_t *)pwmDatos,index);
	}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
barras();

if(banda1>7 && banda2>7 && banda3>5 && banda4>4)
{
switch(caso_laser)
{
//case 0: HAL_GPIO_TogglePin(GPIOC, LASER1_Pin);
case 0: GPIOC->ODR = 2;
break;
//case 1: 	HAL_GPIO_TogglePin(GPIOC, LASER2_Pin);
case 1: GPIOC->ODR = 4;
break;
}
caso_laser = (caso_laser+1)%2;

switch(prueba_pwm)
{
case 0:
pwm_H = 20;
pwm_L = 180;
break;
case 1:
pwm_H = 15;
pwm_L = 185;
break;
case 2:
pwm_H = 10;
pwm_L = 190;
break;
case 3:
pwm_H = 15;
pwm_L = 185;
break;
}
prueba_pwm = (prueba_pwm+1)%4;
}

Enviar_datos();
vumetro(banda1,banda2,banda3,banda4,banda5,banda6,banda7,banda8);
//Devuelvo los valores maximos de las bandas a su origen
max1=0;
max2=0;
max3=0;
max4=0;
max5=0;
max6=0;
max7=0;
max8=0;
}






void HAL_TIM_OC_DelayElapsedCallback (TIM_HandleTypeDef * htim){


if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
	//pwm_pulse = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
	pwm_pulse = TIM1->CCR1;
if(flag_pwm == 0)
{
	//__HAL_TIM_SET_COMPARE (htim, TIM_CHANNEL_1, (pwm_pulse+pwm_H)%60000);
	TIM1->CCR1 = (pwm_pulse+pwm_H)%60000;
	flag_pwm = 1;
}
else
{
	//__HAL_TIM_SET_COMPARE (htim, TIM_CHANNEL_1, (pwm_pulse+pwm_L)%60000);
	TIM1->CCR1 = (pwm_pulse+pwm_L)%60000;
	flag_pwm = 0;
}

}

if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){
	//pwm_pulse = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
	pwm_pulse = TIM1->CCR3;
if(flag2_pwm == 0)
{
	//__HAL_TIM_SET_COMPARE (htim, TIM_CHANNEL_3, (pwm_pulse+pwm_H)%60000);
	TIM1->CCR3 = (pwm_pulse+pwm_H)%60000;
	flag2_pwm = 1;
}
else
{
	//__HAL_TIM_SET_COMPARE (htim, TIM_CHANNEL_3, (pwm_pulse+pwm_L)%60000);
	TIM1->CCR3 = (pwm_pulse+pwm_L)%60000;
	flag2_pwm = 0;
}
}
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
iir_int = (float32_t)HAL_ADC_GetValue(&hadc1);
if(i<10)
{
i++;
	}
else
{
//Para usar los filtros
arm_biquad_cascade_df1_f32(&S1,&iir_int,&iir_out1,1);
arm_biquad_cascade_df1_f32(&S2,&iir_int,&iir_out2,1);
arm_biquad_cascade_df1_f32(&S3,&iir_int,&iir_out3,1);
arm_biquad_cascade_df1_f32(&S4,&iir_int,&iir_out4,1);
arm_biquad_cascade_df1_f32(&S5,&iir_int,&iir_out5,1);
arm_biquad_cascade_df1_f32(&S6,&iir_int,&iir_out6,1);
arm_biquad_cascade_df1_f32(&S7,&iir_int,&iir_out7,1);
arm_biquad_cascade_df1_f32(&S8,&iir_int,&iir_out8,1);


}
valor_maximo(iir_out1,iir_out2,iir_out3,iir_out4,iir_out5,iir_out6,iir_out7,iir_out8);

contador_adc++;

}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  //Para inicializar los filtros

arm_biquad_cascade_df1_init_f32(&S1,1,&iir_coeff1[0],&iir_state1[0]);
arm_biquad_cascade_df1_init_f32(&S2,2,&iir_coeff2[0],&iir_state2[0]);
arm_biquad_cascade_df1_init_f32(&S3,2,&iir_coeff3[0],&iir_state3[0]);
arm_biquad_cascade_df1_init_f32(&S4,2,&iir_coeff4[0],&iir_state4[0]);
arm_biquad_cascade_df1_init_f32(&S5,3,&iir_coeff5[0],&iir_state5[0]);
arm_biquad_cascade_df1_init_f32(&S6,4,&iir_coeff6[0],&iir_state6[0]);
arm_biquad_cascade_df1_init_f32(&S7,4,&iir_coeff7[0],&iir_state7[0]);
arm_biquad_cascade_df1_init_f32(&S8,2,&iir_coeff8[0],&iir_state8[0]);


  HAL_ADC_Start_IT(&hadc1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);
  HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_3);

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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

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
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
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
  sConfig.Channel = ADC_CHANNEL_2;
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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 9600;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 60000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 185;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  htim2.Init.Period = 2176-1;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 48000;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 166-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 120-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pins : OTG_FS_PowerSwitchOn_Pin PC1 PC2 */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
