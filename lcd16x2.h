#pragma GCC diagnostic ignored "-Wwrite-strings"

class LiquidCrystal{
	public: void lcdCmd(int cmd){
		PORTA = ((cmd & 0xF0)/4)+2;
		_delay_ms(2);
		PORTA-=2;
		
		PORTA = ((cmd & 0x0F)*4)+2;
		_delay_ms(2);
		PORTA-=2;
	}
	
	public:void data(int data){
		PORTA = ((data & 0xF0)/4)+3;
			_delay_ms(2);
			PORTA-=2;
			
			PORTA = ((data & 0x0F)*4)+3;
			_delay_ms(2);
			PORTA-=2;
	}
	public:void Begin()
	{
		DDRD = 0xFF;
		_delay_ms(500);
		lcdCmd(0x02);
		lcdCmd(0x28);
		lcdCmd(0x0E);
		lcdCmd(0x06);
		lcdCmd(0x01);
	}
	
	void clear(){
		lcdCmd(1);
	}
	
	void setCursor(int r, int c){
		if (r==0){
			lcdCmd(0x80+c);
		}
		else if (r==1){
			lcdCmd(0xC0+c);
		}
	}
	void print(int key){
		if (key<10)
		{
			data(key+48);
		}
		else{
			print(key/10);
			
			data((key%10) + 48);
		}
	}
	
	void print(char c[])
	{
		for (int i=0;c[i]!='\0';i++)
		{
			data(c[i]);
		}
	}
	
};