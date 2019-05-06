#include <string.h>
#include "stdint.h"
#include "./OLED/ssd1306.h"
#include "menu.h"


#define LCD_ROWS 64
#define LCD_COLS 20

// definition of menu's components: (*name, *next, *prev, *child, *parent, (*menu_function))
menu_t menu1 = { "START", &menu2, &menu4, NULL, NULL, zmien };
menu_t menu2 = { "PLIKI", &menu3, &menu1, NULL, NULL, disp_pliki_callback };
menu_t menu3 = { "OPCJE", &menu4, &menu2, NULL, NULL, disp_brightness_callback };
menu_t menu4 = { "KASUJ", NULL, &menu3, NULL, NULL, NULL};

menu_t *currentPointer = &menu1;

uint8_t menu_index;
uint8_t lcd_row_pos;
uint8_t lcd_row_pos_level_1, lcd_row_pos_level_2;

char 	lcd_buf[LCD_ROWS][LCD_COLS];

//wyczyszczenie wyswietlacza
void clear_menu(void){
	ssd1306_Fill(Black);
}

//odswiezanie menu
void menu_refresh(void) {

	menu_t *temp;
	uint8_t i;
	uint8_t center;

	ssd1306_Fill(Black);
	memset(lcd_buf, '-', LCD_COLS);

	if (currentPointer->parent){
		temp = (currentPointer->parent)->child;

		center = (LCD_COLS>>1) - (strlen((temp->parent)->name)>>1);
		ssd1306_SetCursor(center - 1, 0);
		ssd1306_WriteChar(" ", Font_7x10, White);
		ssd1306_WriteString((temp->parent)->name, Font_7x10, White);
		ssd1306_WriteChar(" ", Font_7x10, White);
	}
	else {
		temp = &menu1;
		ssd1306_SetCursor(2, 2);
		ssd1306_WriteString("-------MENU-------", Font_7x10, White);
	}

	for (i = 0; i != menu_index - lcd_row_pos; i++) {
		temp = temp->next;
	}


	for (i = 2; i < LCD_ROWS; i++) {

		ssd1306_SetCursor(10, i * 10);
		if (temp == currentPointer) ssd1306_WriteChar(62, Font_7x10, White);
		else ssd1306_WriteChar(' ', Font_7x10, White);

		ssd1306_SetCursor(20, i * 10);
		ssd1306_WriteString(temp->name, Font_7x10, White);

		temp = temp->next;
		if (!temp) break;

	}

}

uint8_t menu_get_index(menu_t *q) {

	menu_t *temp;
	uint8_t i = 0;

	if (q->parent) temp = (q->parent)->child;
	else temp = &menu1;

	while (temp != q) {
		temp = temp->next;
		i++;
	}

	return i;
}

uint8_t menu_get_level(menu_t *q) {

	menu_t *temp = q;
	uint8_t i = 0;

	if (!q->parent) return 0;

	while (temp->parent != NULL) {
		temp = temp->parent;
		i++;
	}

	return i;
}

//nastepny element menu
void menu_next(void) {

	if (currentPointer->next)
	{

		currentPointer = (*currentPointer).next;
		menu_index++;
		if (++lcd_row_pos > LCD_ROWS - 1) lcd_row_pos = LCD_ROWS - 1;
	}
	else
	{
		menu_index = 0;
		lcd_row_pos = 0;

		if (currentPointer->parent) currentPointer = (currentPointer->parent)->child;
		else currentPointer = &menu1;
	}

	menu_refresh();

}

//poprzedni element menu
void menu_prev(void) {

	currentPointer = currentPointer->prev;

	if (menu_index)
	{
		menu_index--;
		if (lcd_row_pos > 0) lcd_row_pos--;
	}
	else
	{
		menu_index = menu_get_index(currentPointer);

		if (menu_index >= LCD_ROWS - 1) lcd_row_pos = LCD_ROWS - 1;
		else lcd_row_pos = menu_index;
	}

	menu_refresh();
}

//przejscie do podmenu
void menu_enter(void) {

	if (currentPointer->menu_function) currentPointer->menu_function();

	if (currentPointer->child)
	{

		switch (menu_get_level(currentPointer)) {
			case 0:
				lcd_row_pos_level_1 = lcd_row_pos;
				break;

			case 1:
				lcd_row_pos_level_2 = lcd_row_pos;
				break;
		}

		menu_index = 0;
		lcd_row_pos = 0;

		currentPointer = currentPointer->child;

		menu_refresh();
	}
}

//przejscie do menu nadrzednego
void menu_back(void) {

	if (currentPointer->parent) {

		switch (menu_get_level(currentPointer)) {
			case 1:
				lcd_row_pos = lcd_row_pos_level_1;
				break;

			case 2:
				lcd_row_pos = lcd_row_pos_level_2;
				break;
			}

		currentPointer = currentPointer->parent;
		menu_index = menu_get_index(currentPointer);

		menu_refresh();

	}
}
