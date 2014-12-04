/* 
* tiempo.h
*
* Created: 18/01/2014 11:22:34
* Author: Cesar
*/


#ifndef __TIEMPO_H__
#define __TIEMPO_H__

ht1632c ledMatrix = ht1632c(&PORTA, pinDATA, pinWR, pinCLK, pinCS1, GEOM_32x16, 2);

class tiempo
{
	//variables
	public:
	char horas;
	char minutos;
	char segundos;
	char anos;
	char meses;
	char dia_mes;
	char dia_semana;
	protected:
	private:

	//functions
	public:
	tiempo();
	~tiempo();
	protected:
	private:
	tiempo( const tiempo &c );
	tiempo& operator=( const tiempo &c );

}; //tiempo
tiempo tiempo_actual, tiempo_alarma;

class dibujar{
	
	char buf[5];
	char dig[13];
	
	public:
	void clear_digits(){
		ledMatrix.rect(10,1,20,5,BLACK);
		ledMatrix.rect(11,2,19,4,BLACK);
		ledMatrix.rect(12,3,18,3,BLACK);
		ledMatrix.rect(13,4,17,2,BLACK);
	}
	
	void digits_hours_mins(char pos, char dig[], char param){
		char buf[2];
		// set the hours digits
		itoa(param, buf, 10);
		(param<10) ? dig[pos]='0':dig[pos]=buf[0]; 

			dig[pos+1] = buf[1];

	}
	void adjustBoldness(int hora){
		// adjust the boldness based on the time of day. normal font = less pixels lit = darker for sleeping
		if (hora <= 8) ledMatrix.setfont(FONT_7x14); // 12:00a to 8:59a use a 7x14 normal font
		else ledMatrix.setfont(FONT_7x14B); // 9:00a to 11:59a use a bold font for improved distance visibility
	}
	
	void putchar(){
		ledMatrix.putchar(2,-2,dig[1],color_digitos);
		ledMatrix.putchar(9,-2,dig[2],color_digitos);                 // second digit of hour
		ledMatrix.plot(16,3,color_digitos);                           // hour:min colon, top
		ledMatrix.plot(16,6,color_digitos);                           // hour:min colon, bottom
		ledMatrix.putchar(18,-2,dig[3],color_digitos);                // first digit of minute
		ledMatrix.putchar(25,-2,dig[4],color_digitos);                // second digit of minute
		
	}
	
	void digits_months_days(char mes, char dia){
		
		// set the month digits
		itoa(mes, buf, 10);
		if (mes < 10){
			dig[5] = '\0';
			dig[6] = buf[0];
		}
		else{
			dig[5] = buf[0];
			dig[6] = buf[1];
		}
		
		// set the day digits
		itoa(dia, buf, 10);
		if (dia < 10){
			dig[7] = '\0';
			dig[8] = buf[0];
		}
		else{
			dig[7] = buf[0];
			dig[8] = buf[1];
		}
	}
	
	void digits_anos_diames(){
		// set the day digits
		itoa(tiempo_actual.anos, buf, 10);
		dig[9] = buf[0];
		dig[10] = buf[1];
		
		
		// set the day digits
		itoa(tiempo_actual.dia_mes, buf, 10);
		if (tiempo_actual.dia_mes < 10){
			dig[11] = '0';
			dig[12] = buf[0];
		}
		else{
			dig[11] = buf[0];
			dig[12] = buf[1];
		}
	}
	
	void show_temp_fecha(){
		ledMatrix.setfont(FONT_4x6);
		// mostrar temperatura
		sprintf(buf, "%2.1f", temp);
		ledMatrix.putchar(0,11,buf[0],color_digitos);
		ledMatrix.putchar(4,11,buf[1],color_digitos);
		ledMatrix.putchar(8,11,buf[2],color_digitos);
		ledMatrix.putchar(12,11,buf[3],color_digitos);
		
		// mostrar fecha
		ledMatrix.putchar(16,11,dig[7],color_digitos);
		ledMatrix.putchar(20,11,dig[8],color_digitos);
		ledMatrix.putchar(24,11,'/',color_digitos);
		ledMatrix.putchar(28,11,buf[5],color_digitos);
		ledMatrix.putchar(32,11,buf[6],color_digitos);
	}
	
	void digito_hora(){
		ledMatrix.putchar(2,-2,dig[1],BLINK);
		ledMatrix.putchar(9,-2,dig[2],BLINK);                 // second digit of hour
	}
	void digito_minuto(){
		ledMatrix.putchar(18,-2,dig[3],BLINK);                // first digit of minute
		ledMatrix.putchar(25,-2,dig[4],BLINK);                // second digit of minute
	}
	void digito_dia_semana(){
		ledMatrix.putchar(0,11,dig[7],BLINK);
		ledMatrix.putchar(4,11,dig[8],BLINK);
	}
	void digito_dia(){
		ledMatrix.putchar(8,11,dig[11],BLINK);
		ledMatrix.putchar(12,11,dig[11],BLINK);
	}
	void digito_mes(){
		ledMatrix.putchar(16,11,dig[5],BLINK);
		ledMatrix.putchar(20,11,dig[6],BLINK);
	}
	void digito_ano(){
		ledMatrix.putchar(24,11,dig[9],BLINK);
		ledMatrix.putchar(28,11,dig[10],BLINK);
	}
	
};

#endif //__TIEMPO_H__
