/* 
* tiempo.h
*
* Created: 18/01/2014 11:22:34
* Author: Cesar
*/


#ifndef __TIEMPO_H__
#define __TIEMPO_H__


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

#endif //__TIEMPO_H__
