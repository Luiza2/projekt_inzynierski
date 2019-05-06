#include "functions.h"
#include "main.h"
#include "menu.h"
#include "./OLED/ssd1306.h"
#include "fatfs.h"
#include <string.h>

//250, 124 najwieksza jasnosc na starcie
//125, 64 œrednia jasnosc
int jasnosc = 250;
int pasek = 124;


//pozycja gwiazdki w menu
int p_x = 10;
int p_y = 12;
const int p_ymin = 12;
const int p_ymax = 52;

int wyswietl = 6;

//maksymalna ilosc plikow zapisanych na karcie
int max_ilosc_plikow = 100;

//licznik plikow na karcie
int ilosc_plikow = 0;

//indeks elementu listy plikow
int menu_indeks = 1;

char nazwa_pliku[100][7];

void (*key_next_func)(void) = &menu_next;
void (*key_prev_func)(void) = &menu_prev;
void (*key_enter_func)(void) = &menu_enter;
void (*key_back_func)(void) = &menu_back;

/* Search a directory for objects and display it */
int scan_files (char* path)
{
    FRESULT res;
    DIR dir;

    static FILINFO fno;

    for(int i = 0 ; i < max_ilosc_plikow ; i++)
    {
    	for(int j = 0; j < 7; j++)
    	{
    		nazwa_pliku[i][j] = ' ';
    	}
    }

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (int a = 0;; a+= 1) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */

                                               /* It is a file. */
            for(int i = 0 ; i < strlen(fno.fname); i++){
            	nazwa_pliku[a][i] = fno.fname[i];
            	nazwa_pliku[a][i+1] = ' ';

            }
            ilosc_plikow++;
        }

        f_closedir(&dir);
        return FR_OK;
    }
    else{
    		ssd1306_Fill(Black);
    		ssd1306_SetCursor(20, 10);
    		ssd1306_WriteString("Blad odczytu karty", Font_7x10, White);
    		ssd1306_UpdateScreen();
    		return 1;
    	}

}

//szukanie plikow zapisanych na karice pamieci
void sprawdz_pliki(){

	ilosc_plikow = 0;
	ssd1306_Fill(Black);
	FATFS fs;
	FRESULT res;
	char buff[256];
	if(BSP_SD_Init()==MSD_OK){
		res = f_mount(&fs, "", 1);
		if (res == FR_OK) {
			 strcpy(buff, "/");
			 res = scan_files(buff);


		}else{
			ssd1306_Fill(Black);
			ssd1306_SetCursor(20, 10);
			ssd1306_WriteString("Blad odczytu karty", Font_7x10, White);
			ssd1306_UpdateScreen();

		}
	}

	pliki_refresh();

}

//przejscie do gory w podgladzie plikow
void gora(void){

	if(menu_indeks > 1){

			menu_indeks--;
			if(wyswietl > 6){
				wyswietl--;
			}
		}

	pliki_refresh();

}

int podaj_indeks(void){
	return menu_indeks;
}

//przejscie w dol w podgladaniu plikow
void dol(void){

	if(menu_indeks < (ilosc_plikow - 1)){

		menu_indeks++;

	}

	if(wyswietl < ilosc_plikow + 4){
				wyswietl++;
			}

	pliki_refresh();

}

//funkcja do rysowania na ekranie odczytanych plikow z karty
void pliki_refresh(void){

	ssd1306_Fill(Black);
	ssd1306_SetCursor(p_x, p_y);
	ssd1306_WriteChar('>', Font_7x10, White);

	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(" ------PLIKI------", Font_7x10, White);

	//pomocnicze drukowanie ilosci plikow
	ssd1306_SetCursor(70, 20);
	char string[3];
	sprintf(string, "%d", ilosc_plikow);
	ssd1306_WriteString(string, Font_7x10, White);

	//pomocnicze drukowanie wskazywanego indeksu
	ssd1306_SetCursor(90, 20);
		char stringen[3];
		sprintf(stringen, "%d", menu_indeks);
		ssd1306_WriteString(stringen, Font_7x10, White);

	for(int a = wyswietl - 5, b = 1;a < wyswietl ;a++, b++){
		for(int i = 0 ; i < 13; i++){
			if(nazwa_pliku[a][i] != ' ' ){
				ssd1306_SetCursor(20 + i * 7, b * 10 + 2);
				ssd1306_WriteChar(nazwa_pliku[a][i], Font_7x10, White);
				ssd1306_UpdateScreen();
			}
				else{
					break;
			}
		}
	}
}

//zmiana funkcji przyciskow do menu wyswietlania plikow
void disp_pliki_callback(void){

	key_next_func = dol;
	key_prev_func = gora;
	key_enter_func = bluetooth;
	key_back_func = keys_back;
	sprawdz_pliki();

}


//do tworzenia nowego pliku o nowej nazwie

char scan_files1 (char* path , char nazwa[][7] )
{
    FRESULT res;
    DIR dir;

    static FILINFO fno;

    for(int i = 0 ; i < max_ilosc_plikow ; i++)
    {
    	for(int j = 0; j < 7; j++)
    	{
    		nazwa[i][j] = ' ';
    	}
    }

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (int a = 0;; a+= 1) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */

                                               /* It is a file. */
            	for(int i = 0 ; i < strlen(fno.fname); i++){

						nazwa[a][i] = fno.fname[i];

            }
        }

        f_closedir(&dir);

    }

    return nazwa;
}

//do tworzenia pliku o nowej nazwie
int sprawdz_pliki1(){

	char nazwa[max_ilosc_plikow][7];
	int licznik = 0;

	ssd1306_Fill(Black);
	FATFS fs;
	FRESULT res;
	char buff[256];
	res = f_mount(&fs, "", 1);
	if (res == FR_OK) {
		 strcpy(buff, "/");
		 res = scan_files1(buff, nazwa);
	}

	for(int a = 1 ;a < max_ilosc_plikow ;a++){
			for(int i = 0 ; i < 7; i++){
					if(nazwa[a][i] == '.'){

						licznik++;

					}

			}
	}
	return licznik;
}



//przycisk przejscia do kolejnego elementu listy
void key_next_press(void){

	static uint8_t key_next_lock=0;
	if( !key_next_lock && !(HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin) ) ) {
		key_next_lock=1;
		if(key_next_func) (*key_next_func)();
	} else if( key_next_lock && (HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin)) ) key_next_lock++;

}

//przycisk przejscia do poprzedniego elementu listy
void key_prev_press(void){

	static uint8_t key_prev_lock=0;
	if( !key_prev_lock && !(HAL_GPIO_ReadPin(Button1_GPIO_Port, Button1_Pin) ) ) {
		key_prev_lock=1;
		if(key_prev_func) (*key_prev_func)();

	} else if( key_prev_lock && (HAL_GPIO_ReadPin(Button1_GPIO_Port, Button1_Pin)) ) key_prev_lock++;

}


//przejscie do podmenu
void key_enter_press(void){

	static uint8_t key_enter_lock=0;
	if( !key_enter_lock && !(HAL_GPIO_ReadPin(Button2_GPIO_Port, Button2_Pin) ) ) {
		key_enter_lock=1;
		if(key_enter_func) (*key_enter_func)();
	} else if( key_enter_lock && (HAL_GPIO_ReadPin(Button2_GPIO_Port, Button2_Pin)) ) key_enter_lock++;

}

//przejscie do menu nadrzednego
void key_back_press(void){

	static uint8_t key_back_lock=0;
	if( !key_back_lock && !(HAL_GPIO_ReadPin(Button3_GPIO_Port, Button3_Pin) ) ) {
		key_back_lock=1;
		if(key_back_func) (*key_back_func)();
	} else if( key_back_lock && (HAL_GPIO_ReadPin(Button3_GPIO_Port, Button3_Pin)) ) key_back_lock++;

}



//powrot do pierwotnych funkcji przyciskow
void keys_back(void){
	key_next_func = menu_next;
	key_prev_func = menu_prev;
	key_enter_func = menu_enter;
	key_back_func = menu_back;

	menu_refresh();

}

//zmiana funkcji przyciskow do sterowania jasnoscia wyswietlacza
void disp_brightness_callback(void){

	key_next_func = disp_brightness_next;
	key_prev_func = disp_brightness_prev;
	key_enter_func = NULL;
	key_back_func = keys_back;

	disp_brightness_refresh();

}

//podmenu sterowania jasnoscia wyswietlacza
void disp_brightness_refresh(void){

	ssd1306_Fill(Black);
	ssd1306_SetCursor(2,10);
	ssd1306_WriteString("  JASNOSC EKRANU ", Font_7x10, White);

	ssd1306_WriteCommand(0x81);
	ssd1306_WriteCommand(jasnosc);

	for(int i = 4 ; i < pasek ; i++){
		for(int j = 36 ; j < 48 ; j++){
			ssd1306_DrawPixel(i, j, White);
		}
	}

}

//zwieszenie kontrastu wyswietlacza
void disp_brightness_next(void){

	if(pasek >= 16){
		pasek = pasek - 12;
	}else{
			pasek = 4;
	}
	if(jasnosc >= 25){
			jasnosc = jasnosc - 25;
	} else{
		jasnosc = 0;
	}

	disp_brightness_refresh();

}

//zmniejszenie kontrastu wyswietlacza
void disp_brightness_prev(void){

	if(pasek <= 112){
			pasek = pasek + 12;
		} else pasek = 124;
	if(jasnosc <= 225){
				jasnosc = jasnosc + 25;
			} else {
				jasnosc = 250;
			}
	disp_brightness_refresh();

}

int wlacz_ekran(void){
	int czas = 10;
	return czas;
}
