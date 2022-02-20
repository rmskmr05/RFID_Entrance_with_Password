/*
 * tollPlaza.cpp
 *
 * Created: 19-08-2019 15:51:08
 * Author : Bhavik
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include "lcd16x2.h"
#include "usart.h"
#include <avr/interrupt.h>

serialcom uart;
LiquidCrystal lcd;

int match(char str1[],char str2[])
{
	int m=1;
	for (int i=0;i<12;i++){
		if (str1[i]!=str2[i]){
			m=0;
		}
	}
	return m;
}

int match_pin(int str1,int str2)
{
	if (str1==str2){
		return 1;
	}
	return 0;
}

volatile int i=0,r=0; char tag[12];char pin[3];
ISR(USART_RXC_vect){
	tag[i]=UDR;
	i++;
	if (i==12){
		uart.print(tag);
		i=0;r=1;
	}
}

void nullTag(char tag[])
{
	for (i=0;i<12;i++)
	{
		tag[i]='\0';
	}
}

int getKey()
{
	DDRB = 0x1F;
	PORTB = 0xE0;//pull up
	for (int i=32,k=1;i<=128;i*=2,k++) {
		if ((PINB & 0xE0) == (~i & 0xE0)){
			while((PINB & 0xE0) == (~i & 0xE0));
			_delay_ms(100);
			return k;
		}
	}
	return -1;
}

int getPassword(){
	int n=0,password=0 ;
	while(1){
		int k = getKey();
		{  //keypad matrix Function
			if (k>0 && k<=3){
				password = 10*password + k;
				n++;
				lcd.print(k);
				if (n>=3){
					return password;
					n=0;
				}
			}
		}
	}
}


int count = 0;
int main(void)
{
	DDRA = 0xFF;
	uart.begin();
	lcd.Begin();
	sei();
	
	restart:
	lcd.clear();
	lcd.print("Welcome to Toll");
	lcd.setCursor(1,0);
	lcd.print("Vehicles:");
	lcd.setCursor(1,10);
	lcd.print(count);
	char unique[12] = {'0','2','0','0','1','0','A','0','4','2','F','0'}; //"020010A042F0";//
	int unique_pin=111;
	int flag = 0;
	DDRC = 0x03;
	
	DDRD = 0x00;
	int pswd;
	
	while (1)
	{
		if (r==1)
		{
			r=0;
			int m = match(tag,unique);
			
			_delay_ms(1000);
			if (m==1)
			{
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("match");
				_delay_ms(1000);
				reEnter:
				
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("Enter pin:");
				lcd.setCursor(1,0);
				pswd = getPassword();
				int n=match_pin(pswd,unique_pin);
				
				if (n==1)
				{
					lcd.clear();
					lcd.print("THANK YOU!");
					PORTC = 0x01;
					_delay_ms(500);
					PORTC = 0;
					// IR Count
					DDRB = 0xFE;_delay_ms(10);
					while((PINB & 0x01));
					count++;
					_delay_ms(1000);
					PORTC = 2;
					_delay_ms(1000);
					PORTC = 0;
					pswd = 0;
					goto restart;
				}
				else
				{
					flag++;
					lcd.clear();
					lcd.print("Wrong Password");
					_delay_ms(500);
					if (flag<=3)
					{
						lcd.lcdCmd(0xC0);
						lcd.print("Enter Again");
						_delay_ms(500);
						goto reEnter;
						
					}
					else goto restart;
				}
			}
			else{
				lcd.clear();
				lcd.setCursor(1,0);
				lcd.print("Unregistered");
				_delay_ms(2000);
				goto restart;
			}
		}
	}
}

