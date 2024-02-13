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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//Defino el valor para el ciclo activo del cero y el uno
//Se escriben en el capture compare register o pulse
#define Neopixel_cero 38
#define Neopixel_uno 76
//Ahora veo la cantidad de pixels, osea ,leds, de mi matriz
#define Num_pixels 64
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
DMA_HandleTypeDef hdma_tim1_ch1;

/* USER CODE BEGIN PV */
#define buffsize 1
#define Numcoeff 5 //biquad cascade usa b0,b1,b2,a1,a2
#define Cascada 1 //cantidad de biquad a diseñar
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
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

volatile int flag_envio_datos = 0;
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
//No lo detenemos porque eso estaba dando un error pero dejamos la funcion
//HAL_TIM_PWM_Stop_DMA (&htim1, TIM_CHANNEL_1);
	flag_envio_datos = 1;

}

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

//Me armo una funcion que controle el brillo
//float brillo=100; //La manejo en porcentaje
//encontre que es mejor manejarse con una funcion con curva suave. Uso la tangente

float brillo= 45;
void Set_Brillo(float brillo){
	if(brillo>45) brillo = 45;
	for(int i=0;i<Num_pixels;i++)
	{
		Datos_Pixel_F[i][0] = Datos_Pixel[i][0];
		for(int j=1;j<4;j++)
		{
			float angulo = 90 - brillo;
			angulo = angulo*3.14/180;
			Datos_Pixel_F[i][j] = Datos_Pixel[i][j]/(tan(angulo));
		}
	}


}

//Me armo una función que envíe los datos desde DMA a neopixeles

void Enviar_datos (void)
{
// Para el programa final trabajar a 24 fps
	//uint32_t datos;
	uint16_t Prueba_pwmDatos = 0;
	datos datos;
	uint16_t index = 0;


	for (int i = 0; i<Num_pixels; i++)
	{
		//si no funciona tambien deberia andar el codigo siguiente
		datos.Datos8[0]=Datos_Pixel_F[i][3];
		datos.Datos8[1]=Datos_Pixel_F[i][2];
		datos.Datos8[2]=Datos_Pixel_F[i][1];

		//datos = ((Datos_Pixel_F[i][1]<<16) | (Datos_Pixel_F[i][2]<<8)|(Datos_Pixel_F[i][3]));


		for (int i = 23; i>=0; i--)
		{

			if (datos.Datos32&(1<<i) ) pwmDatos[index] = Neopixel_uno; //ver counter&(1<<j)
			else pwmDatos[index] = Neopixel_cero;
			Prueba_pwmDatos = pwmDatos[index];
			index++;



		}

	}

	for (int i=0; i<50;i++)
	{
		pwmDatos[index] = 0;
		index++;
		Prueba_pwmDatos = pwmDatos[index];
	}



	//Envio la informacion con esta funcion de la HAL
		if(HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1,(uint32_t *)pwmDatos,index)!=HAL_OK)
			{
			Error_Handler();
			};
//		while(!flag_envio_datos){}; //Cambiarlo por un if
//		flag_envio_datos=0;
		//Aca va la otra opcion de modificar directamente el registro "Pulse"
	/*for (int i = Num_pixels-1; i>=0;i--)
	 * {
	 *    TIM2->CCR1 = pwmDatos[i]
	 * }
	 *Tambien se puede cambiar el duty cicle de PWM con __HAL_TIM_SetCompare(__HANDLE__,__CHANNEL__;__COMPARE__)
	 *Y cambiar el periodo de PWM con __HAL_TIM_SetAutoreload(__HANBLE__,__AUTORELOAD__)
	 *
	 */
	/*
	Acordarse de cambiar la función de carga al TIM2 para PWM por DMA para hacerla por
	Interrupciones. De esa forma podemos procesar informacion durante los tiempo
	muertos de envio de datos. El HAL_TIM_PWM_Start_DMA dice que es no bloqueante
	*/
	}
//Me genero la función para las bandas del vumetro}
//Ver como es el recorrido de las luces en la matriz led, por lo que vi
//en el arcoiris estan conectados los bordes
//la comento porque no va a tener sentido todavia si no estan todos los valores
//declarados. Es para la funcion final
//Toma un valor entero sin signo para la intensidad de las bandas
//void vumetro(uint8_t banda1,uint8_t banda2,uint8_t banda3,uint8_t banda4,uint8_t banda5,uint8_t banda6,uint8_t banda7,uint8_t banda8){
uint8_t caso =0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	switch(caso)
	{
	case 0:	vumetro(2,4,6,7);
	Set_Brillo(brillo);
	Enviar_datos();
	break;
	case 1: vumetro(1,3,5,8);
	Set_Brillo(brillo);
	Enviar_datos();
	break;
	case 2:
	vumetro(1,5,1,5);
	Set_Brillo(brillo);
	Enviar_datos();
	break;
	case 3:	vumetro(2,1,3,4);
	Set_Brillo(brillo);
	Enviar_datos();
	break;
	case 4:	vumetro(8,7,6,2);
	Set_Brillo(brillo);
	Enviar_datos();
	break;
	case 5:	vumetro(1,2,3,4);
	Set_Brillo(brillo);
	Enviar_datos();
	break;
	case 6: vumetro(1,3,6,2);
	Set_Brillo(brillo);
	Enviar_datos();
	break;

	}
	caso = (caso+1)%6;
}



void vumetro(uint8_t banda1,uint8_t banda2,uint8_t banda3,uint8_t banda4){
//Hay que tener en cuenta que cada banda del vumetro son dos de la matriz, pero
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


/*
numled=64;
switch(banda5){
case 1:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=14;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,76,0,153);
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=13;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=12;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=11;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=10;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=9;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
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
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
	Set_Pixel(9+numled,0,204,0);
	Set_Pixel(8+numled,0,0,0);
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
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
	Set_Pixel(9+numled,0,204,0);
	Set_Pixel(8+numled,0,204,0);
	break;
}

numled=80;
switch(banda6){
case 1:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=14;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,76,0,153);
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=13;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=12;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=11;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=10;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=9;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
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
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
	Set_Pixel(9+numled,0,204,0);
	Set_Pixel(8+numled,0,0,0);
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
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
	Set_Pixel(9+numled,0,204,0);
	Set_Pixel(8+numled,0,204,0);
	break;
}

numled=96;
switch(banda7){
case 1:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=14;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,76,0,153);
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=13;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=12;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=11;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=10;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=9;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
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
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
	Set_Pixel(9+numled,0,204,0);
	Set_Pixel(8+numled,0,0,0);
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
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
	Set_Pixel(9+numled,0,204,0);
	Set_Pixel(8+numled,0,204,0);
	break;
}

numled=112;
switch(banda8){
case 1:
	Set_Pixel(0+numled,76,0,153);
	for(uint16_t j=1;j<=14;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,76,0,153);
	break;
case 2:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	for(uint16_t j=2;j<=13;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	break;

case 3:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	for(uint16_t j=3;j<=12;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	break;
case 4:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	for(uint16_t j=4;j<=11;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	break;
case 5:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	for(uint16_t j=5;j<=10;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	break;
case 6:
	Set_Pixel(0+numled,0,76,153);
	Set_Pixel(1+numled,0,153,153);
	Set_Pixel(2+numled,102,0,204);
	Set_Pixel(3+numled,204,0,0);
	Set_Pixel(4+numled,204,0,0);
	Set_Pixel(5+numled,255,255,51);
	for(uint16_t j=6;j<=9;j++){
		Set_Pixel(j+numled,0,0,0);
	}
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
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
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
	Set_Pixel(9+numled,0,204,0);
	Set_Pixel(8+numled,0,0,0);
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
	Set_Pixel(15+numled,0,76,153);
	Set_Pixel(14+numled,0,153,153);
	Set_Pixel(13+numled,102,0,204);
	Set_Pixel(12+numled,204,0,0);
	Set_Pixel(11+numled,204,0,0);
	Set_Pixel(10+numled,255,255,51);
	Set_Pixel(9+numled,0,204,0);
	Set_Pixel(8+numled,0,204,0);
	break;
	*/
}

	//Esta  funcion es solo para probar
/*
	uint16_t Step=0;
	int paso=0;

	uint8_t arcoiris() {
	    // Strip ID: 0 - Effect: Rainbow - LEDS: 64
	    // Steps: 106 - Delay: 27
	    // Colors: 3 (255.0.0, 0.255.0, 0.0.255)
	    // Options: rainbowlen=158, toLeft=true,
	  float factor1, factor2;
	  uint16_t ind;
	  for(uint16_t j=0;j<128;j++) {
	    ind = Step + j * 0.6708860759493671;
	    switch((int)((ind % 300) / 35.333333333333336)) {
	      case 0: factor1 = 1.0 - ((float)(ind % 106 - 0 * 35.333333333333336) / 35.333333333333336);
	              factor2 = (float)((int)(ind - 0) % 106) / 35.333333333333336;
	              Set_Pixel((j)%Num_pixels, 255 * factor1 ,  255 * factor2, 0);
	              Set_Brillo(20);
	              Enviar_datos();
	              break;
	      case 1: factor1 = 1.0 - ((float)(ind % 106 - 1 * 35.333333333333336) / 35.333333333333336);
	              factor2 = (float)((int)(ind - 35.333333333333336) % 106) / 35.333333333333336;
	              Set_Pixel((j)%Num_pixels, 0, 255 * factor1 ,  255 * factor2);
	              Set_Brillo(20);
	              Enviar_datos();
	              break;
	      case 2: factor1 = 1.0 - ((float)(ind % 106 - 2 * 35.333333333333336) / 35.333333333333336);
	              factor2 = (float)((int)(ind - 70.66) % 106) / 35.333333333333336;
	              Set_Pixel((j)%Num_pixels,  255 * factor2, 0, 255 * factor1);
	              Set_Brillo(20);
	              Enviar_datos();
	              break;
	    }
	  }
	  if(Step >= 300) {Step=0;; return 0x03; }
	  else Step++;
	  return 0x01;
	}
*/

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
DATO_DAC = (uint16_t)iir_out1;
GPIOD->ODR = DATO_DAC;
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
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
arm_biquad_cascade_df1_init_f32(&S,Cascada,&iir_coeff[0],&iir_state[0]);
  //Para inicializar los filtros

arm_biquad_cascade_df1_init_f32(&S1,2,&iir_coeff1[0],&iir_state1[0]);
arm_biquad_cascade_df1_init_f32(&S2,2,&iir_coeff2[0],&iir_state2[0]);
arm_biquad_cascade_df1_init_f32(&S3,2,&iir_coeff3[0],&iir_state3[0]);
arm_biquad_cascade_df1_init_f32(&S4,2,&iir_coeff4[0],&iir_state4[0]);
arm_biquad_cascade_df1_init_f32(&S5,3,&iir_coeff5[0],&iir_state5[0]);
arm_biquad_cascade_df1_init_f32(&S6,4,&iir_coeff6[0],&iir_state6[0]);
arm_biquad_cascade_df1_init_f32(&S7,4,&iir_coeff7[0],&iir_state7[0]);
arm_biquad_cascade_df1_init_f32(&S8,2,&iir_coeff8[0],&iir_state8[0]);

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
  HAL_TIM_Base_Start_IT(&htim3);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //arcoiris();
  //Esto no se debe hacer, convendria hacerlo por interrupciones, es para probar
/*
  for (int j=0; j<Num_pixels;j++)
  {
  Set_Pixel(j,0,255,255);
  }
  float brillo = 45;
  Set_Brillo(brillo);
  Enviar_datos();
  HAL_Delay(3000);

 for (int i=45; i>=0;i--)
 {
	 Set_Brillo(i);
	 Enviar_datos();
	 HAL_Delay(20);
 }

 for (int i=0; i<=45;i++)
 {
	 Set_Brillo(i);
	 Enviar_datos();
	 HAL_Delay(20);
 }
 for (int i=45; i>=0;i--)
 {
	 Set_Brillo(i);
	 Enviar_datos();
	 HAL_Delay(20);
 }
  for (int j=0; j<Num_pixels;j++)
  {
  Set_Pixel(j,255,0,255);
  }

 for (int i=0; i<=45;i++)
 {
	 Set_Brillo(i);
	 Enviar_datos();
	 HAL_Delay(20);
 }
 for (int i=45; i>=0;i--)
 {
	 Set_Brillo(i);
	 Enviar_datos();
	 HAL_Delay(20);
 }
  for (int j=0; j<Num_pixels;j++)
  {
  Set_Pixel(j,0,0,255);
  }
 for (int i=0; i<=45;i++)
 {
	 Set_Brillo(i);
	 Enviar_datos();
	 HAL_Delay(20);
 }
 HAL_Delay(3000);
 while(1)
 {
 arcoiris();
 if(paso>64){paso=0;
 }
 else{
 paso++;
 }
 HAL_Delay(20);
 }
*/

  while (1)
  {

		//HAL_Delay(41);
	/*	  for (int j=0; j<(Num_pixels/2)-1;j++)
		  {
		  Set_Pixel(j,0,255,0);
		  }
		  for (int j=(Num_pixels/2); j<Num_pixels;j++)
		  {
		  Set_Pixel(j,0,0,255);
		  }
		  float brillo = 20;
	      //Set_Brillo(brillo);
		  Enviar_datos();
		  */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  //Aca una forma de hacer un fade para probar
  /*
   * for (int i=45; i>=0; i--)
   * {
   * Set_brillo = i;
   * Enviar_datos();
   * HAL_Delay(50);
   * }
   */
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
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 120-1;
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
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
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
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
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
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 3676-1;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
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
  htim3.Init.Period = 332-1;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

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
