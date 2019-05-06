#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void key_next_press(void);
void key_prev_press(void);
void key_enter_press(void);
void key_back_press(void);
void keys_back(void);
void disp_brightness_callback(void);
void disp_pliki_callback(void);
void disp_brightness_refresh(void);
void disp_brightness_next(void);
void disp_brightness_prev(void);
int podaj_indeks(void);
int wlacz_ekran(void);


#define Button3_Pin GPIO_PIN_5
#define Button3_GPIO_Port GPIOA
#define Button_Pin GPIO_PIN_9
#define Button_GPIO_Port GPIOC
#define Button2_Pin GPIO_PIN_8
#define Button2_GPIO_Port GPIOB
#define Button1_Pin GPIO_PIN_9
#define Button1_GPIO_Port GPIOB

#endif /* FUNCTIONS_H_ */
