#include "main.h"
#include "functions.h"

#ifndef MENU_H_
#define MENU_H_

typedef struct menu_struct menu_t;

struct menu_struct {

	const char * name;
	menu_t * next;
	menu_t * prev;
	menu_t * child;
	menu_t * parent;
	void (*menu_function)(void);

};

menu_t menu1;
menu_t menu2;
menu_t menu3;
menu_t menu4;


void menu_refresh(void);
void menu_next(void);
void menu_prev(void);
void menu_enter(void);
void menu_back(void);

#endif /* MENU_H_ */
