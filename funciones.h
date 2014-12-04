#define funciones_h


  /*TEXT FUNCTIONS
  byte putchar(int x, int y, char c, byte color = GREEN, byte attr = 0);
  void hscrolltext(int y, const char *text, byte color, int delaytime, int times = 1, byte dir = LEFT);
  void vscrolltext(int x, const char *text, byte color, int delaytime, int times = 1, byte dir = UP);
  
  GRPHIC FUNCTIONS
  void plot (byte x, byte y, byte color);
  byte getpixel (byte x, byte y);
  void line(int x0, int y0, int x1, int y1, byte color);
  void rect(int x0, int y0, int x1, int y1, byte color);
  void circle(int xm, int ym, int r, byte color);
  void ellipse(int x0, int y0, int x1, int y1, byte color);
  void fill (byte x, byte y, byte color);
  void bezier(int x0, int y0, int x1, int y1, int x2, int y2, byte color);
  void putbitmap(int x, int y, prog_uint16_t *bitmap, byte w, byte h, byte color);
  void setfont(byte userfont);
  */

//Pines
#define pinCS1 2
#define pinWR 1
#define pinDATA 0
#define pinCLK 3
#define pinFCN 19
#define pinAlarma 3
#define pinStop 2
#define pinTrigger 8
#define pinEcho 11
#define pinLDR A0
#define pinLEDs 13

//Constantes
#define DS1307_ADDRESS 0x68					//RTC I2C address
#define NUM_MENUS 2  //3 Menus
#define POS_ALARMA 2
#define POS_INFO 1
#define POS_INICIO 0
#define POS_ALARMA_ON 10
#define ON 1
#define OFF 0
#define TIEMPO_MOSTRAR_ALARMA 24 //200.000 us * 25 = 5 seg
#define DIST_SNOOZE 20
#define MARGEN_LUMINOSIDAD 650
#define TIEMPO_SNOOZE 300
#define TIEMPO_ALARMA_ACTIVADA 1500
#define DIA 1
#define NOCHE 0
#define DIGITO_HORA 1
#define DIGITO_MINUTO 0
#define DIGITO_DIA 3
#define DIGITO_MES 4
#define DIGITO_DIA_SEMANA 2
#define DIGITO_ANO 5
#define NUM_CONTADOR_PARPADEO 1
#define TIEMPO_MEDIR_TEMP 20
#define TIEMPO_MEDIR_LUM 50
#define INSTENSIDAD_LEDS_SNOOZE 40
#define LIMITE_BOTON_CAP 500
#define POS_MEM 0



//Variables globales
volatile char seleccion_menu, contador_mostrar_alarma, flag_mostrar_alarma, seleccion_anterior,estado_alarma,
 flag_boton_funcion_corta, flag_boton_funcion_larga, color_digitos, contador_actualizar_hora, contador_boton_funcion,
 flag_actualizar_reloj, snooze, flag_dormido, alarma_activada, flag_parpadeo, contador_parpadeo, estado_parpadeo,
 flag_medir_luminosidad, contador_medir_luminosidad, digito_seleccionado, flag_parpadeo_digitos, estado_parpadeo_digitos,
 contador_parpadeo_digitos, color_parpadeo, flag_edicion, flag_clean_screen, flag_actualizar_temp, flag_boton_stop,
 flag_boton_fcn, estado_boton_stop, estado_boton_fcn, salida_pulsacion_larga, contador_boton_stop, flag_sonido;
 
 int temp;
 
 volatile int contador_alarma_activada, contador_snooze, contador_dormido, contador_actualizar_temp, intensidad_leds;
 
 char modo_edicion, luz;

	






