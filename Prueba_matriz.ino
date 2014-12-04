#include "ht1632c.h"
#include "funciones.h"
#include "tiempo.h"

#include "TimerThree.h"
#include "Wire.h"


  
// ht1632c(volatile uint8_t *port, const uint8_t data, const uint8_t wr, const uint8_t clk, const uint8_t cs, const uint8_t geometry, const uint8_t number);






	dibujar dch;
	dibujar dca;
	dibujar dh;
	dibujar dhi;
	dibujar dao;
	 
void setup()
{	
	
	//Start the I2C protocol
//	Wire.begin();  //Start as master
	
	//Timer3.initialize(); //this breaks analogWrite() for digital pins 9 and 10 on Arduino
	//Timer3.attachInterrupt(timer, 200000);

	//ledMatrix.clear(); // clear the display
	//ledMatrix.pwm(15); // set brightness (max=15)
  
	//attachInterrupt(0, cambiar_funcion, FALLING);
	//attachInterrupt(1, cambiar_estado_alarma, CHANGE);
	//attachInterrupt(2, stop, FALLING);
	
	//medir_luminosidad();
	//actualizar_reloj();
	//medir_temp();

}

void loop()
{
	color_digitos=1;

	ledMatrix.putchar(2,-2,'a',2,2,1);
	ledMatrix.sendframe();
	//menu();
	
	//if (flag_boton_funcion_corta)
	//{
		//flag_boton_funcion_corta=0;
		//boton_funcion_corta();
	//}else if (flag_boton_funcion_larga)
	//{
		//flag_boton_funcion_larga=0;
		//boton_funcion_larga();
	//}
	//
	//if (flag_actualizar_reloj)
	//{
		//flag_actualizar_reloj=0;
		//actualizar_reloj();
	//}
	//
	//if (flag_medir_temp)
	//{
		//flag_medir_temp=0;
		//medir_temp();
	//}
	//
	//if (flag_medir_luminosidad)
	//{
		//flag_medir_luminosidad=0;
		//medir_luminosidad();
	//}
	//
	//if (alarma_activada && !snooze)
	//{
		//alarma_on();
		//activar_snooze();
		//activar_sonido();
		//activar_tira_leds();
	//}else
	//{
		//seleccion_menu=POS_INICIO;
	//}
	//
	//if (flag_dormido)
	//{
		//if(luz==DIA)
		//{
			//flag_dormido=0;
		//}
	//}
//
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
		case POS_ALARMA_ON:
			dibujar_alarma_on(tiempo_actual.horas, tiempo_actual.minutos);
		break;
		default:
			inicio();
	}
}

void inicio()
{
	dibujar_hora(tiempo_actual.horas, tiempo_actual.minutos);
}

void info()
{
	dibujar_hora_info(tiempo_actual.horas, tiempo_actual.minutos, tiempo_actual.meses, tiempo_actual.dia_mes);
}

void alarma()
{
	dibujar_hora(tiempo_alarma.horas, tiempo_alarma.minutos);
}

void alarma_on()
{
	if (!flag_parpadeo)
	{
		flag_parpadeo=1;
		contador_parpadeo=2;
	}
	if (estado_parpadeo)
	{
		seleccion_menu=POS_ALARMA_ON;
	}else
	{
		seleccion_menu=POS_INICIO;
	}
	
}

void ajustar_brillo()
{
	if (luz==DIA)
	{
		ledMatrix.pwm(15);
	}else
	{
		ledMatrix.pwm(4);
	}
	
}

void boton_funcion_larga()
{
	modo_edicion=!modo_edicion;
	digito_seleccionado=0;
}

void boton_funcion_corta()
{
	
	if (modo_edicion)
	{
		if (seleccion_menu==POS_INICIO | seleccion_menu==POS_INFO)
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
}

int bcdToDec(byte bcd)
{
	return (bcd/16*10 + bcd%16);
}

void incrementar_parametro(char* parametro)
{	
	if (!digitalRead(pinStop))
	{
		*++parametro;
	}
}

void modificar_horas(char* hora, char* minutos, char* dia, char* mes, char* dia_semana, char* ano)
{
	switch (digito_seleccionado) {
		case DIGITO_MINUTO:
			incrementar_parametro(minutos);
		break;
		case DIGITO_HORA:
			incrementar_parametro(hora);
		break;
		case DIGITO_DIA:
			incrementar_parametro(dia);	
		break;
		case DIGITO_MES:
			incrementar_parametro(mes);
		break;
		case DIGITO_DIA_SEMANA:
		incrementar_parametro(dia_semana);
		break;
		case DIGITO_ANO:
		incrementar_parametro(ano);
		break;
		default:
		incrementar_parametro(minutos);
	}
}

void dibujar_cambiar_hora()
{
	dch.clear_digits();

	modificar_horas(&tiempo_actual.horas, &tiempo_actual.minutos, &tiempo_actual.dia_mes, &tiempo_actual.meses, &tiempo_actual.dia_semana, &tiempo_actual.anos);
	setDateDs1307(tiempo_actual.minutos, tiempo_actual.horas, tiempo_actual.dia_semana, tiempo_actual.dia_mes, tiempo_actual.meses, tiempo_actual.anos);
	
	dch.digits_hours_mins(&tiempo_actual.minutos, &tiempo_actual.horas);
	dch.digits_months_days();
	dch.digits_anos_diames();
	
	ledMatrix.setfont(FONT_7x14);
	
	if (digito_seleccionado==DIGITO_HORA)
	{
		dch.digito_hora();
	}
	
	ledMatrix.plot(16,3,color_digitos);                                  // hour:min colon, top
	ledMatrix.plot(16,6,color_digitos);                                  // hour:min colon, bottom
	
	if (digito_seleccionado==DIGITO_MINUTO)
	{
		dch.digito_minuto();
	}

	ledMatrix.setfont(FONT_4x6);

	if (digito_seleccionado==DIGITO_DIA_SEMANA)
	{
		dch.digito_dia_semana();
	}
	
	if (digito_seleccionado==DIGITO_DIA)
	{
		dch.digito_dia();
	}
	
	if (digito_seleccionado==DIGITO_MES)
	{
		dch.digito_mes();
	}
	
	if (digito_seleccionado==DIGITO_ANO)
	{
		dch.digito_ano();
	}
	
	// send the characters to the display, and draw the screen
	ledMatrix.sendframe();
	
}

void dibujar_cambiar_alarma()
{
	dca.clear_digits();
	
	modificar_horas(&tiempo_alarma.horas, &tiempo_alarma.minutos, &tiempo_alarma.dia_mes, &tiempo_alarma.meses, &tiempo_alarma.dia_semana, &tiempo_alarma.anos);
	
	dca.digits_hours_mins();
	
	dca.adjustBoldness(tiempo_alarma.horas);
	
	if (digito_seleccionado==DIGITO_HORA)
	{
		dca.digito_hora();
	}
	
	ledMatrix.plot(16,3,color_digitos);                                  // hour:min colon, top
	ledMatrix.plot(16,6,color_digitos);                                  // hour:min colon, bottom
	
	if (digito_seleccionado==DIGITO_MINUTO)
	{
		dca.digito_minuto();
	}
	
	ledMatrix.sendframe();
}

void dibujar_hora(char hora, char minutos)
{
	dh.clear_digits();
	
	dh.digits_hours_mins();
	
	dh.adjustBoldness(hora);
	
	dh.putchar();
	
	// send the characters to the display, and draw the screen
	ledMatrix.sendframe();
	
}

void dibujar_hora_info(char hora, char minutos, char mes,char dia)
{
	
	dhi.clear_digits();
	dhi.digits_hours_mins();
	dhi.digits_months_days();
	ledMatrix.setfont(FONT_7x14);
	dhi.putchar();
	ledMatrix.setfont(FONT_4x6);
	dhi.show_temp_fecha();
	
	// send the characters to the display, and draw the screen
	ledMatrix.sendframe();
	
}

void dibujar_alarma_on(char hora, char minutos)
{
	
	// turn all screen on
	ledMatrix.rect(0,10,35,35,RED);
	
	dao.digits_hours_mins();
	
	dao.adjustBoldness(hora);
	
	dao.putchar();
	
	// send the characters to the display, and draw the screen
	ledMatrix.sendframe();
}

void medir_temp()
{
	temp = (5.0 * analogRead(pinTemp)*100.0)/1023.0;
	contador_medir_temp=300; //300*200ms=1 minuto
}

void comprobar_alarma()
{
	if ((tiempo_actual.horas==tiempo_alarma.horas) && (tiempo_actual.minutos==tiempo_alarma.minutos) && !alarma_activada && (!tiempo_actual.segundos))
	{
		alarma_activada=1;
		contador_alarma_activada=1500; //1500*200ms=5 minutos
	}
}

void activar_tira_leds()
{
	
}

void activar_sonido()
{
	
}

void medir_luminosidad()
{
	if (analogRead(pinLDR) < MARGEN_LUMINOSIDAD)
	{
		luz= NOCHE;
	}
	else
	{
		luz=DIA;
	}
	
	contador_medir_luminosidad=150;
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
		contador_snooze=300;
	}
	
}

void stop() //INT
{
	alarma_activada=0;
	flag_dormido=1;
	snooze=0;
	contador_dormido=600;
	

}

void cambiar_funcion()  //INT
{

	contador_boton_funcion=3;  //3*200ms = 400-600ms
	
}

void cambiar_estado_alarma()  //INT
{
	estado_alarma=!estado_alarma;
	
	if (estado_alarma) color_digitos=GREEN;
	else color_digitos=RED;
	
	flag_mostrar_alarma=1;
	contador_mostrar_alarma=TIEMPO_MOSTRAR_ALARMA;
	seleccion_anterior=seleccion_menu;
	seleccion_menu=POS_ALARMA;
}

void timer()  //INT
{
	
	if (flag_mostrar_alarma)
	{
		if (!contador_mostrar_alarma--)
		{
			seleccion_menu=seleccion_anterior;
			flag_mostrar_alarma=0;
		}
			
	}
	
	if (contador_boton_funcion)
	{
		if (!digitalRead(pinFCN))
		{
			if (!(--contador_boton_funcion))
			{
				flag_boton_funcion_larga=1;
			}
			
		}else
		{
			flag_boton_funcion_corta=1;
		}
		
	}
	
	if (!(--contador_actualizar_hora))
	{
		flag_actualizar_reloj=1;

	}
	
	if (!(--contador_medir_temp))
	{
		flag_medir_temp=1;

	}
	
	if (snooze)
	{
		if (!contador_snooze--)
		{
			snooze=0;
		}
	}
	
	if (contador_alarma_activada && !snooze && alarma_activada)
	{
		contador_alarma_activada--;
		
	}else
	{
		snooze=1;
		contador_snooze=300;
	}
	
	if (flag_dormido)
	{
		if (!contador_dormido--)
		{
			alarma_activada=1;
			flag_dormido=0;
		}
	}
	
	if (flag_parpadeo)
	{
		if (!contador_parpadeo--)
		{
			flag_parpadeo=0;
			estado_parpadeo=!estado_parpadeo;
		}
	}
	
	if (!(--contador_medir_luminosidad))
	{
		flag_medir_luminosidad=1;

	}
}