#include "ht1632c.h"
#include "TimerThree.h"
#include "tiempo.h"
#include "funciones.h"
#include "Wire.h"
#include "Wtv020sd16p.h"
#include "CapacitiveSensor.h"
#include <EEPROM.h>
  
// ht1632c(volatile uint8_t *port, const uint8_t data, const uint8_t wr, const uint8_t clk, const uint8_t cs, const uint8_t geometry, const uint8_t number);
ht1632c ledMatrix = ht1632c(&PORTA, pinDATA, pinWR, pinCLK, pinCS1, GEOM_32x16, 2);

int resetPin = 4;  // The pin number of the reset pin.
int clockPin = 5;  // The pin number of the clock pin.
int dataPin = 6;  // The pin number of the data pin.
int busyPin = 7;  // The pin number of the busy pin.

//Create an instance of the Wtv020sd16p class.
Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);
CapacitiveSensor   cs_FCN = CapacitiveSensor(10,pinFCN);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin
CapacitiveSensor   cs_STOP = CapacitiveSensor(10,pinStop);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin


tiempo tiempo_actual, tiempo_alarma;
	 
void setup()
{	
	pinMode(pinAlarma, INPUT);
	pinMode(pinTrigger, OUTPUT);
	pinMode(pinLEDs, OUTPUT);
	pinMode(A0, INPUT);

	//Start the I2C protocol
	Wire.begin();  //Start as master
	Serial.begin(9600);  //Start as master
	color_digitos=1;

	Timer3.initialize(); //this breaks analogWrite() for digital pins 9 and 10 on Arduino
	Timer3.attachInterrupt(timer, 200000);

	ledMatrix.clear(); // clear the display
	ledMatrix.pwm(1); // set brightness (max=15)
  
	attachInterrupt(1, cambiar_estado_alarma, CHANGE);
	
	//Initializes the module.
	wtv020sd16p.reset();
	
	tiempo_alarma.horas = EEPROM.read(POS_MEM);
	tiempo_alarma.minutos = EEPROM.read(POS_MEM+1);
	
	medir_luminosidad();
	actualizar_reloj();
	actualizar_temp();
}

void loop()
{
	
	menu();
	if(seleccion_menu!=seleccion_anterior || flag_clean_screen){
		ledMatrix.clear();
		flag_clean_screen=0;
	}
	seleccion_anterior=seleccion_menu;
	
	if (flag_boton_funcion_corta)
	{
		flag_boton_funcion_corta=0;
		boton_funcion_corta();
		//ledMatrix.clear(); // clear the display
	}else if (flag_boton_funcion_larga)
	{
		flag_boton_funcion_larga=0;
		boton_funcion_larga();
		ledMatrix.clear(); // clear the display
	}
	
	if (flag_actualizar_reloj)
	{
		flag_actualizar_reloj=0;
		actualizar_reloj();
	}
	
	if (flag_actualizar_temp)
	{
		flag_actualizar_temp=0;
		actualizar_temp();
	}
	
	if (flag_medir_luminosidad)
	{
		flag_medir_luminosidad=0;
		medir_luminosidad();
	}
	
	if (alarma_activada && !snooze)
	{
		activar_snooze();
		activar_sonido();
		activar_tira_leds(intensidad_leds);
		Serial.println(intensidad_leds, DEC);
	}
	
	if ((!alarma_activada || snooze) && flag_sonido)
	{
		wtv020sd16p.stopVoice();
		flag_sonido=0;

	}
	
	if (flag_dormido)
	{
		if(luz==DIA)
		{
			flag_dormido=0;
		}
	}
	
	check_cap_buttons();
	
	// send the characters to the display, and draw the screen
	ledMatrix.sendframe();

}

void check_cap_buttons()
{
	//long total1 =  cs_STOP.capacitiveSensor(30);
	if (cs_STOP.capacitiveSensor(30)>LIMITE_BOTON_CAP)
	{
		estado_boton_stop=1;
		flag_boton_stop=1;
		
	}else{
		estado_boton_stop=0;
	}
	if (cs_FCN.capacitiveSensor(30)>LIMITE_BOTON_CAP)
	{
		estado_boton_fcn=1;
		flag_boton_fcn=1;
	}else{
		estado_boton_fcn=0;
}
}

void menu()
{
	switch (seleccion_menu) {
		case POS_INICIO:		
			if (modo_edicion)
			{
				dibujar_cambiar_hora();
			}else
			{
				inicio();
			}
		break;
		case POS_INFO:
			info();
			modo_edicion=0;
		break;
		case POS_ALARMA:
			if (modo_edicion)
			{
				dibujar_cambiar_alarma();
			}else
			{
				alarma();
			}		
		break;
		default:
			inicio();
	}
}

void inicio()
{
	ledMatrix.plot(31,0,GREEN);
	dibujar_hora_inicio(tiempo_actual.horas, tiempo_actual.minutos);
	
}

void info()
{
	ledMatrix.plot(31,0,GREEN);
	ledMatrix.plot(31,1,GREEN);
	dibujar_hora_info(tiempo_actual.horas, tiempo_actual.minutos, tiempo_actual.meses, tiempo_actual.dia_mes);
	
}

void alarma()
{
	ledMatrix.plot(31,0,GREEN);
	ledMatrix.plot(31,1,GREEN);
	ledMatrix.plot(31,2,GREEN);
	dibujar_hora(tiempo_alarma.horas, tiempo_alarma.minutos);
}

void ajustar_brillo()
{
	if (luz==DIA)
	{
		ledMatrix.pwm(15);
	}else
	{
		ledMatrix.pwm(1);
	}
}

void boton_funcion_larga()
{
	modo_edicion=!modo_edicion;
	digito_seleccionado=0;
	flag_parpadeo_digitos=modo_edicion;
}

void boton_funcion_corta()
{
	
	if (modo_edicion)
	{
		if (seleccion_menu==POS_INICIO)
		{
			if (digito_seleccionado==DIGITO_ANO)
			{
				digito_seleccionado=DIGITO_MINUTO;
			}else
			{
				digito_seleccionado++;
			}
		}else if (seleccion_menu==POS_ALARMA)
		{
			if (digito_seleccionado==DIGITO_HORA)
			{
				digito_seleccionado=DIGITO_MINUTO;
			}else
			{
				digito_seleccionado++;
			}
		}

	}else
	{
		if (seleccion_menu==NUM_MENUS)
		{
			seleccion_menu=POS_INICIO;
		}else
		{
			seleccion_menu++;
		}
	}
}

byte decToBcd(byte val)
{
	return ( (val/10*16) + (val%10) );
}

void setDateDs1307(char minutos, char horas, char dia_semana, char dia_mes, char mes, char ano)
{
	Wire.beginTransmission(DS1307_ADDRESS);
	Wire.write(byte(0x00));
	Wire.write(decToBcd(0));  // 0 to bit 7 starts the clock
	Wire.write(decToBcd(minutos));
	Wire.write(decToBcd(horas));    // If you want 12 hour am/pm you need to set
	// bit 6 (also need to change readDateDs1307)
	Wire.write(decToBcd(dia_semana));
	Wire.write(decToBcd(dia_mes));
	Wire.write(decToBcd(mes));
	Wire.write(decToBcd(ano));
	Wire.endTransmission();
}

void actualizar_reloj()
{
	
	// Reset the register pointer
	Wire.beginTransmission(DS1307_ADDRESS);
	Wire.write(0x00);
	Wire.endTransmission();
		
	Wire.requestFrom(DS1307_ADDRESS, 7);    //Request 7 bytes
		
	tiempo_actual.segundos=bcdToDec(Wire.read());
	tiempo_actual.minutos=bcdToDec(Wire.read());
	tiempo_actual.horas=bcdToDec(Wire.read() & 0b111111);  //Mask to get only the first 6 bits
	tiempo_actual.dia_semana=bcdToDec(Wire.read());
	tiempo_actual.dia_mes=bcdToDec(Wire.read());
	tiempo_actual.meses=bcdToDec(Wire.read());
	tiempo_actual.anos=bcdToDec(Wire.read());
	contador_actualizar_hora=3; //3*200ms=actualizacion cada medio segundo
	comprobar_alarma();
	//debug();
}

void actualizar_temp(){
	
	char tMSB, tLSB;
	double temperatura;

	Wire.beginTransmission(DS1307_ADDRESS);
	Wire.write(0x11);
	Wire.endTransmission();
	Wire.requestFrom(DS1307_ADDRESS, 2);

	if(Wire.available()) {
		tMSB = Wire.read(); //2's complement int portion
		tLSB = Wire.read(); //fraction portion
	
		temperatura = (tMSB & B01111111); //do 2's math on Tmsb
		temperatura += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
	}
	else {
		temperatura=0;
	}
	temp = (int)floor(temperatura*100.00);
	contador_actualizar_temp=TIEMPO_MEDIR_TEMP;
	
}

int bcdToDec(byte bcd)
{
	return (bcd/16*10 + bcd%16);
}

void modificar_horas(char* hora, char* minutos, char* dia, char* mes, char* dia_semana, char* ano)
{
	switch (digito_seleccionado) {
		case DIGITO_MINUTO:
			if (*minutos<59)
			{
				*minutos=*minutos+1;
			}else{
				*minutos=0;
			}
			
		break;
		case DIGITO_HORA:
			if (*hora<23)
			{
				*hora=*hora+1;
				}else{
				*hora=0;
			}
		break;
		case DIGITO_DIA:
			if (*dia<31)
			{
				*dia=*dia+1;
				}else{
				*dia=1;
			}	
		break;
		case DIGITO_MES:
			if (*mes<12)
			{
				*mes=*mes+1;
				}else{
				*mes=1;
			}
		break;
		case DIGITO_DIA_SEMANA:
			*dia_semana=*dia_semana+1;
		break;
		case DIGITO_ANO:
			if (*ano<30)
			{
				*ano=*ano+1;
				}else{
				*ano=14;
			}
		break;
		default:
			flag_edicion=0;
	}
	flag_edicion=0;
}

char digit1(char param){
	return param / 10 + 48;
}

char digit2(char param){
	return param % 10 + 48;
}

void adjustBoldness(int hora){
	// adjust the boldness based on the time of day. normal font = less pixels lit = darker for sleeping
	if (hora <= 8) ledMatrix.setfont(FONT_7x13); // 12:00a to 8:59a use a 7x14 normal font
	else ledMatrix.setfont(FONT_7x13B); // 9:00a to 11:59a use a bold font for improved distance visibility
}

void putchar(char horas, char minutos){
	
	ledMatrix.putchar(1,-2,digit1(minutos),color_digitos);
	ledMatrix.putchar(8,-2,digit2(minutos),color_digitos);                 // second digit of hour
	ledMatrix.plot(15,3,color_digitos);                           // hour:min colon, top
	ledMatrix.plot(15,6,color_digitos);                           // hour:min colon, bottom
	ledMatrix.putchar(17,-2,digit1(horas),color_digitos);                // first digit of minute
	ledMatrix.putchar(24,-2,digit2(horas),color_digitos);                // second digit of minute
		
}

void dibujar_cambiar_hora()
{

	if (flag_edicion)	modificar_horas(&tiempo_actual.horas, &tiempo_actual.minutos, &tiempo_actual.dia_mes, &tiempo_actual.meses, &tiempo_actual.dia_semana, &tiempo_actual.anos);
	setDateDs1307(tiempo_actual.minutos, tiempo_actual.horas, tiempo_actual.dia_semana, tiempo_actual.dia_mes, tiempo_actual.meses, tiempo_actual.anos);
	
	adjustBoldness(tiempo_actual.horas);

	ledMatrix.putchar(1,-2,digit1(tiempo_actual.horas),color_parpadeo | !(digito_seleccionado==DIGITO_HORA));
	ledMatrix.putchar(8,-2,digit2(tiempo_actual.horas),color_parpadeo | !(digito_seleccionado==DIGITO_HORA));                 // second digit of hour

	ledMatrix.plot(15,3,color_digitos);                                  // hour:min colon, top
	ledMatrix.plot(15,6,color_digitos);                                  // hour:min colon, bottom
	
	ledMatrix.putchar(17,-2,digit1(tiempo_actual.minutos),color_parpadeo | !(digito_seleccionado==DIGITO_MINUTO));                // first digit of minute
	ledMatrix.putchar(24,-2,digit2(tiempo_actual.minutos),color_parpadeo | !(digito_seleccionado==DIGITO_MINUTO));               // second digit of minute
	
	ledMatrix.setfont(FONT_4x6);

	ledMatrix.putchar(0,11,digit1(tiempo_actual.dia_semana),color_parpadeo | !(digito_seleccionado==DIGITO_DIA_SEMANA));
	ledMatrix.putchar(4,11,digit2(tiempo_actual.dia_semana),color_parpadeo | !(digito_seleccionado==DIGITO_DIA_SEMANA));

	ledMatrix.putchar(8,11,digit1(tiempo_actual.dia_mes),color_parpadeo | !(digito_seleccionado==DIGITO_DIA));
	ledMatrix.putchar(12,11,digit2(tiempo_actual.dia_mes),color_parpadeo | !(digito_seleccionado==DIGITO_DIA));

	ledMatrix.putchar(16,11,digit1(tiempo_actual.meses),color_parpadeo | !(digito_seleccionado==DIGITO_MES));
	ledMatrix.putchar(20,11,digit2(tiempo_actual.meses),color_parpadeo | !(digito_seleccionado==DIGITO_MES));

	ledMatrix.putchar(24,11,digit1(tiempo_actual.anos),color_parpadeo | !(digito_seleccionado==DIGITO_ANO));
	ledMatrix.putchar(28,11,digit2(tiempo_actual.anos),color_parpadeo | !(digito_seleccionado==DIGITO_ANO));
	
}

void dibujar_cambiar_alarma()
{
	if (flag_edicion){
			modificar_horas(&tiempo_alarma.horas, &tiempo_alarma.minutos, &tiempo_alarma.dia_mes, &tiempo_alarma.meses, &tiempo_alarma.dia_semana, &tiempo_alarma.anos);
			EEPROM.write(POS_MEM, tiempo_alarma.horas);
			EEPROM.write(POS_MEM+1, tiempo_alarma.minutos);
	}
	
	adjustBoldness(tiempo_alarma.horas);

	ledMatrix.putchar(1,-2,digit1(tiempo_alarma.horas),color_parpadeo | !(digito_seleccionado==DIGITO_HORA));
	ledMatrix.putchar(8,-2,digit2(tiempo_alarma.horas),color_parpadeo | !(digito_seleccionado==DIGITO_HORA));                 // second digit of hour

	ledMatrix.plot(15,3,color_digitos);                                  // hour:min colon, top
	ledMatrix.plot(15,6,color_digitos);                                  // hour:min colon, bottom
	
	ledMatrix.putchar(17,-2,digit1(tiempo_alarma.minutos),color_parpadeo | !(digito_seleccionado==DIGITO_MINUTO));                // first digit of minute
	ledMatrix.putchar(24,-2,digit2(tiempo_alarma.minutos),color_parpadeo | !(digito_seleccionado==DIGITO_MINUTO));               // second digit of minute
	
}

void dibujar_hora(char hora, char minutos)
{
	if (alarma_activada && !snooze) ledMatrix.fill(1,1, 1+estado_parpadeo+estado_parpadeo);

	adjustBoldness(hora);
	
	putchar(minutos, hora);
}

void dibujar_hora_inicio(char horas, char minutos)
{
	if (alarma_activada && !snooze){
		ledMatrix.line(0,0,30,0,1+estado_parpadeo+estado_parpadeo);
		ledMatrix.line(0,15,30,15,1+estado_parpadeo+estado_parpadeo);
	}

	if (horas <= 8) ledMatrix.setfont(FONT_7x14); // 12:00a to 8:59a use a 7x14 normal font
	else ledMatrix.setfont(FONT_7x14B); // 9:00a to 11:59a use a bold font for improved distance visibility
	
	ledMatrix.putchar(0,1,digit1(horas),color_digitos);
	ledMatrix.putchar(8,1,digit2(horas),color_digitos);                 // second digit of hour
	ledMatrix.plot(16,6,color_digitos);                           // hour:min colon, top
	ledMatrix.plot(16,9,color_digitos);                           // hour:min colon, bottom
	ledMatrix.putchar(19,1,digit1(minutos),color_digitos);                // first digit of minute
	ledMatrix.putchar(26,1,digit2(minutos),color_digitos);                // second digit of minute
}

void dibujar_hora_info(char hora, char minutos, char mes,char dia)
{
	char buf[5];
		
	adjustBoldness(hora);
	putchar(minutos, hora);
		
	ledMatrix.setfont(FONT_4x6);
	// mostrar temperatura

	sprintf(buf, "%04d", temp);
	ledMatrix.putchar(0,11,buf[0],color_digitos+1);
	ledMatrix.putchar(4,11,buf[1],color_digitos+1);
	ledMatrix.putchar(8,11,buf[2],color_digitos);
	ledMatrix.putchar(12,11,buf[3],color_digitos);
	
	// mostrar fecha
	ledMatrix.putchar(17,11,digit1(dia),color_digitos+1);
	ledMatrix.putchar(21,11,digit2(dia),color_digitos+1);
	ledMatrix.putchar(25,11,digit1(mes),color_digitos);
	ledMatrix.putchar(29,11,digit2(mes),color_digitos);
	
}

void comprobar_alarma()
{
	
	if ((tiempo_actual.horas==tiempo_alarma.horas) && (tiempo_actual.minutos==tiempo_alarma.minutos) && !alarma_activada && (!tiempo_actual.segundos))
	{
		alarma_activada=1;
		contador_alarma_activada=TIEMPO_ALARMA_ACTIVADA; //1500*200ms=5 minutos
		contador_parpadeo=NUM_CONTADOR_PARPADEO;
	}
}

void activar_tira_leds(int intensidad_leds)
{
	analogWrite(pinLEDs, intensidad_leds);
}

void activar_sonido()
{
	if (!flag_sonido)
	{
		wtv020sd16p.asyncPlayVoice(1);
		flag_sonido=1;
	}
}

void medir_luminosidad()
{
	int medida=analogRead(pinLDR);
	Serial.println(medida);
	
	if (medida < MARGEN_LUMINOSIDAD)
	{
		luz=NOCHE;
	}
	else
	{
		if (luz==NOCHE)
		{
			snooze=1;
		}
		luz=DIA;
	}
	
	contador_medir_luminosidad=TIEMPO_MEDIR_LUM;
	ajustar_brillo();
}

void inicio_medida_dist()
{
	digitalWrite(pinTrigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(pinTrigger, LOW);
}

float medir_distancia()
{
	long duracion;
	duracion = pulseIn(pinEcho, HIGH);
	return duracion/58.2;
}

void activar_snooze()
{
	inicio_medida_dist();
	if (medir_distancia()<DIST_SNOOZE)
	{
		snooze=1;
		flag_clean_screen=1;
		contador_snooze=TIEMPO_SNOOZE;
	}
	
}

void stop() //INT
{
	flag_boton_stop=1;
}

void cambiar_funcion()  //INT
{

	flag_boton_fcn=1;  
	
}

void cambiar_estado_alarma()  //INT
{
	if (digitalRead(pinAlarma)){
		 estado_alarma=1;
		 color_digitos=GREEN;
	}else{
		 estado_alarma=0;
		 color_digitos=RED;
	}
	
	flag_mostrar_alarma=1;
	contador_mostrar_alarma=TIEMPO_MOSTRAR_ALARMA;
	seleccion_menu=POS_ALARMA;
	
}

void timer()  //INT
{
	
	if (flag_boton_stop)
	{
		if (!estado_boton_stop | contador_boton_stop>10)
		{
				if (alarma_activada & contador_boton_stop>10)
				{
					alarma_activada=0;
					flag_dormido=1;
					snooze=0;
					contador_dormido=600;
					flag_clean_screen=1;
					intensidad_leds=0;
					activar_tira_leds(intensidad_leds);
				}

				if (modo_edicion) flag_edicion=1;
				flag_boton_stop=0;
				contador_boton_stop=0;
				
		}else{
			contador_boton_stop++;
		}
		
	}
	
	if (flag_mostrar_alarma)
	{
		if (!contador_mostrar_alarma--)
		{
			seleccion_menu=POS_INICIO;
			flag_mostrar_alarma=0;
		}
			
	}
	
	if (salida_pulsacion_larga)
	{
		salida_pulsacion_larga--;
		flag_boton_fcn=0;
	}else{
		if (flag_boton_fcn)
		{
			if (estado_boton_fcn)
			{
				if (contador_boton_funcion++==3)
				{
					flag_boton_funcion_larga=1;
					contador_boton_funcion=0;
					flag_boton_fcn=0;
					salida_pulsacion_larga=3;
				}
				
			}else
			{
				flag_boton_funcion_corta=1;
				contador_boton_funcion=0;
				flag_boton_fcn=0;
			}
			
		}
	}
	
	if (!(--contador_actualizar_hora))
	{
		flag_actualizar_reloj=1;

	}
	
	if (!(--contador_actualizar_temp))
	{
		flag_actualizar_temp=1;

	}
	
	if (snooze)
	{
		if (!contador_snooze--)
		{
			snooze=0;
			contador_alarma_activada=TIEMPO_ALARMA_ACTIVADA;
		}

		activar_tira_leds(INSTENSIDAD_LEDS_SNOOZE);
		intensidad_leds=0;
	}
	
	if (contador_alarma_activada && !snooze && alarma_activada)
	{
		contador_alarma_activada--;
		if (!contador_parpadeo--)
		{
			estado_parpadeo=!estado_parpadeo;
			contador_parpadeo=NUM_CONTADOR_PARPADEO;
			flag_clean_screen=1;
			if (intensidad_leds<254)
			{
				intensidad_leds=intensidad_leds+1;
				Serial.println(intensidad_leds);
				
			}
		}
	}else if (alarma_activada && !snooze)
	{
		snooze=1;
		contador_snooze=TIEMPO_SNOOZE;
		contador_alarma_activada=TIEMPO_ALARMA_ACTIVADA;
		flag_clean_screen=1;

	}

	if (flag_dormido)
	{
		if (!contador_dormido--)
		{
			alarma_activada=1;
			flag_dormido=0;
		}
	}
	
	if (flag_parpadeo_digitos)
	{
		if (!contador_parpadeo_digitos--)
		{
			if (estado_parpadeo_digitos)
			{
				color_parpadeo=BLACK;
				}else{
				color_parpadeo=GREEN;
			}
			estado_parpadeo_digitos=!estado_parpadeo_digitos;
			contador_parpadeo_digitos=1;
		}
	}
	
	if (!(--contador_medir_luminosidad))
	{
		flag_medir_luminosidad=1;

	}
}