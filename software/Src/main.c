/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "./OLED/ssd1306.h"
#include "menu.h"
#include "gps.h"
#include "string.h"
#include <string.h>
#include "stdbool.h"
#include "functions.h"
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
SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

int trenningTimeSek = 0;
int trenningTimeMin = 0;
int trenningTimeHours = 0;
double distance = 0;
double dystansKM = 0;

HAL_SD_CardInfoTypeDef SDCardInfo;

int stan = 0;
int *x = &stan;

//czas wlaczenia ekranu
int czas_wlaczenia = 10;

bool otworz_plik = true;
bool utworzono_plik = false;

FATFS fatfs;
FIL myfile;
FRESULT fresult;

uint8_t buffer[50];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI3_Init(void);
static void MX_UART4_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

//wyslanie pliku
void bluetooth(){

		int numer_pliku = podaj_indeks();

		char str[10]=".txt";
		FIL fil;       /* File object */
	    char line[47]; /* Line buffer */
	    char debug_buffer[47];

	    char plik[7];
	    sprintf(plik, "%d%s", numer_pliku, str);

	    /* Register work area to the default drive */
	    if(f_mount(&fatfs, "", 0)){
	    	HAL_UART_Transmit(&huart4, (uint8_t *)line, sprintf(line, " mount "), 300);
	    }

	    /* Open a text file */
	    if(f_open(&fil, plik, FA_READ)){
	    	HAL_UART_Transmit(&huart4, (uint8_t *)line, sprintf(line, " plik %d otwarty ", numer_pliku), 300);
	    }

	    /* Read all lines and display it */
	    while (f_gets(line, sizeof line, &fil)){
	    	sprintf(debug_buffer,"%s", &line);
	    	HAL_UART_Transmit(&huart4, (uint8_t *)debug_buffer, strlen(debug_buffer), 300);
	    	//HAL_UART_Transmit(&huart6, (uint8_t *)line, sprintf(line, " srodek pliku "), 300);
	    }

	    /* Close the file */
	    if( f_close(&fil)){
	    	 	HAL_UART_Transmit(&huart4, (uint8_t *)line, sprintf(line, " plik zamkniety "), 300);
	   	 }

}

void zmien(){
	if(*x == 0){
		*x = 1;
	} else{
		*x = 0;
		menu_refresh();
	}
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// przechowuje bufory i liczniki znakow oraz aktualne dane odebrane z GPS
volatile struct gps_state gps_handle;
volatile uint8_t recv_char;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef * uart) {
	if (uart == &huart3) {
		//zapisnaie znakow do bufora
		gps_recv_char(&gps_handle, recv_char);
		HAL_UART_Receive_IT(&huart3, &recv_char, 1);
	}
}

void wlaczenie_ekranu(void){

	czas_wlaczenia = 10;

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
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_SPI3_Init();
  MX_UART4_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */



  	gps_handle = gps_init(&huart3);
  	//wlaczenie obslugi przerwania interfejsu UART
  	HAL_UART_Receive_IT(&huart3, &recv_char, 1);

    char output_buffer[100];
    for (uint8_t i = 0; i < 100; i++)
        output_buffer[i] = '\0';

    ssd1306_Init();
    menu_refresh();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  	  //przypisanie fukcji klawiszom
	  	  key_back_press();
	  	  key_enter_press();
	  	  key_next_press();
	  	  key_prev_press();

	  	  ssd1306_UpdateScreen();

	  	  //sygnalizowanie obecnosci karty w slocie
	  	  if(stan == 0){
			if(BSP_PlatformIsDetected())
			{
				ssd1306_SetCursor(90,50);
				ssd1306_WriteString("K:tak", Font_7x10, White);
			}else{
				ssd1306_SetCursor(90,50);
				ssd1306_WriteString("K:nie", Font_7x10, White);

			}
	  	  }


	  	  if(stan == 1){

	  		  if(otworz_plik == true){
	  			  otworz_plik = false;
					  if(BSP_SD_Init()==MSD_OK)

				  {
						  char str[10]=".txt";
						  char calosc[10];


						  int i = sprawdz_pliki1() + 1;

						  sprintf(calosc, "%d%s", i, str);

					  fresult = f_mount(&fatfs, "", 1);
					  fresult = f_open(&myfile, calosc , FA_CREATE_ALWAYS|FA_WRITE);

					  if(fresult == FR_OK){


						  ssd1306_Fill(Black);
						  						  ssd1306_SetCursor(46, 20);

						  						  ssd1306_WriteString(calosc, Font_7x10, White);



						 ssd1306_SetCursor(22, 30);
						 ssd1306_WriteString("Plik utworzony", Font_7x10, White);
						 ssd1306_UpdateScreen();
						 HAL_Delay(500);
						 utworzono_plik = true;

					  }
					  else{
						  utworzono_plik = false;
						  ssd1306_Fill(Black);
						  ssd1306_SetCursor(22, 20);
						  ssd1306_WriteString("Nie utworzono", Font_7x10, White);
						  ssd1306_SetCursor(48, 30);
						  ssd1306_WriteString("pliku.", Font_7x10, White);
						  ssd1306_SetCursor(8, 40);
						  ssd1306_WriteString("Sprobuj ponownie", Font_7x10, White);
						  ssd1306_UpdateScreen();
						  HAL_Delay(1000);
						  *x = 0;//zmiana stanu na 1, przejscie do glownego menu
					  }

				  }
	  		  }

	  		  //gdy wcisnieto przycisk i poprawnie utworzono plik
	  		if( (HAL_GPIO_ReadPin(Button3_GPIO_Port, Button3_Pin) )  && (utworzono_plik == true)) {


	  			if(HAL_GPIO_ReadPin(Button2_GPIO_Port, Button2_Pin) == 0){
	  				czas_wlaczenia = wlacz_ekran();
	  			}


	  			if(czas_wlaczenia > 0){
	  				czas_wlaczenia--;
	  				//ekran wlaczany
	  				ssd1306_WriteCommand(0xAF);
	  			}else{
	  				//po 10 sekundach wygas ekran
	  				ssd1306_WriteCommand(0xAE);
	  			}


	  			ssd1306_UpdateScreen();
	  			sprintf(output_buffer, "\r\n");
	  			ssd1306_Fill(Black);

	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);
	  			sprintf(output_buffer, "Data: %02d-%02d-20%02d   \r\n", gps_handle.date_day, gps_handle.date_mounth, gps_handle.date_year);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);

	  			sprintf(output_buffer, "Godzina:%02d:%02d:%02d   \r\n", (gps_handle.time_hour + 1), gps_handle.time_min, gps_handle.time_sec);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);


	  			ssd1306_SetCursor(2, 0);
	  			ssd1306_WriteString(output_buffer, Font_7x10, White);
	  			ssd1306_SetCursor(120, 0);
	  			ssd1306_WriteString("        ", Font_7x10, White);

	  			//do pliku
	  			sprintf(output_buffer, "%02d:%02d:%02d ", gps_handle.time_hour, gps_handle.time_min, gps_handle.time_sec);
	  			f_printf(&myfile, output_buffer);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);

	  			sprintf(output_buffer, "Szerokosc geograficzna: %f %c\r\n", gps_handle.latitude, gps_handle.latitude_direction);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);



	  			//do pliku szerokosc
	  			sprintf(output_buffer, "%f %c ", gps_handle.latitude, gps_handle.latitude_direction);
	  			f_printf(&myfile, output_buffer);

	  			sprintf(output_buffer, "Dlugosc geograficzna  : %f %c\r\n", gps_handle.longitude, gps_handle.longitude_direction);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);


	  			//do pliku d³ugosc
	  			sprintf(output_buffer, "%f %c ", gps_handle.longitude, gps_handle.longitude_direction);
	  			f_printf(&myfile, output_buffer);

	  			sprintf(output_buffer, "Wysokosc: %f m n.p.m.\r\n", gps_handle.altitude);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);

	  			//do pliku wysokosc
	  			sprintf(output_buffer, "%f \n", gps_handle.altitude);
	  			f_printf(&myfile, output_buffer);

	  			sprintf(output_buffer, "%f\r\n", gps_handle.speed_kilometers);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);



	  			ssd1306_SetCursor(30, 35);
	  			ssd1306_WriteString(output_buffer, Font_11x18, White);
	  			ssd1306_SetCursor(75, 35);
	  			ssd1306_WriteString("km/h        ", Font_11x18, White);

	  			sprintf(output_buffer, "Ilosc satelit: %d\r\n", gps_handle.satelites_number);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);

	  			//wyswietlanie czasu treningu
	  			trenningTimeSek++;
	  			if (trenningTimeSek == 60){
	  				trenningTimeMin++;
	  			trenningTimeSek = 0;
	  			}
	  			if (trenningTimeMin == 60){
	  				trenningTimeHours++;
	  				trenningTimeMin = 0;
	  			}

	  			sprintf(output_buffer, "Czas: %02d:%02d:%02d\r\n", trenningTimeHours, trenningTimeMin, trenningTimeSek); //konwersja uint32_t na string
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);

	  			ssd1306_SetCursor(2, 20);
	  			ssd1306_WriteString(output_buffer, Font_7x10, White);
	  			ssd1306_SetCursor(100, 20);
	  			ssd1306_WriteString("   ", Font_7x10, White);

	  			//wyswietlanie pokonanej odleglosci
	  			//odswiezanie co sekunde 1h = 3600s
	  			dystansKM = gps_handle.speed_kilometers / 3600;
	  			distance = distance + dystansKM;

	  			sprintf(output_buffer, "Dystans:%f\r\n", distance);
	  			//HAL_UART_Transmit(&huart2, output_buffer, strlen(output_buffer), 100);

	  			ssd1306_SetCursor(2, 10);
	  			ssd1306_WriteString(output_buffer, Font_7x10, White);
	  			ssd1306_SetCursor(115, 10);
	  			ssd1306_WriteString("km       ", Font_7x10, White);


	  			HAL_Delay(1000);
	  		} else{
	  			f_close(&myfile);
	  			*x = 0;
	  			utworzono_plik = false;
	  			otworz_plik = true;
	  			gps_handle.speed_kilometers = 0;
	  			trenningTimeSek = 0;
	  			trenningTimeMin = 0;
	  			trenningTimeHours = 0;
	  			ssd1306_WriteCommand(0xAF);
	  			menu_refresh();

	  		}
	   }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /**Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDIO|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
  PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, W_RES_Pin|W_D_C_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(W_CS_GPIO_Port, W_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B4_Pin */
  GPIO_InitStruct.Pin = B4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(B4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SDIO_CD_Pin */
  GPIO_InitStruct.Pin = SDIO_CD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SDIO_CD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : W_RES_Pin W_D_C_Pin */
  GPIO_InitStruct.Pin = W_RES_Pin|W_D_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : W_CS_Pin */
  GPIO_InitStruct.Pin = W_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(W_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : B2_Pin B3_Pin */
  GPIO_InitStruct.Pin = B2_Pin|B3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
