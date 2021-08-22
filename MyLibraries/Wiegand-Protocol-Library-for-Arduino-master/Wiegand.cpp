#include "Wiegand.h"

volatile unsigned long WIEGAND::_cardTempHigh=0;
volatile unsigned long WIEGAND::_cardTemp=0;
volatile unsigned long WIEGAND::_lastWiegand=0;
unsigned long WIEGAND::_code=0;
volatile int WIEGAND::_bitCount=0;	

bool WIEGAND::_debug = false;

int WIEGAND::_wiegandType=0;

WIEGAND::WIEGAND()
{
}

unsigned long WIEGAND::getCode()
{
	return _code;
}

int WIEGAND::getWiegandType()
{
	return _wiegandType;
}

bool WIEGAND::available()
{
	bool ret;
    noInterrupts();
	ret=DoWiegandConversion();
	interrupts();
	return ret;
}

void WIEGAND::begin()
{
#ifdef digitalPinToInterrupt
  // newer versions of Arduino provide pin to interrupt mapping
  begin(2,digitalPinToInterrupt(2),3,digitalPinToInterrupt(3), false);
#else
  begin(2,0,3,1,false);
#endif
}

void WIEGAND::begin(int pinD0, int pinIntD0, int pinD1, int pinIntD1, bool DebEst)
{
	_lastWiegand = 0;
	_cardTempHigh = 0;
	_cardTemp = 0;
	_code = 0;
	_wiegandType = 0;
	_bitCount = 0;
    _debug = DebEst;  
	pinMode(pinD0, INPUT);					// Set D0 pin as input
	pinMode(pinD1, INPUT);					// Set D1 pin as input
	attachInterrupt(pinIntD0, ReadD0, RISING);	// Hardware interrupt - high to low pulse
	attachInterrupt(pinIntD1, ReadD1, RISING);	// Hardware interrupt - high to low pulse
}

void WIEGAND::ReadD0 ()
{
	_bitCount++;				// Increament bit count for Interrupt connected to D0
	if (_bitCount>31)			// If bit count more than 31, process high bits
	{
		_cardTempHigh |= ((0x80000000 & _cardTemp)>>31);	//	shift value to high bits
		_cardTempHigh <<= 1;
		_cardTemp <<=1;
	}
	else
	{
		_cardTemp <<= 1;		// D0 represent binary 0, so just left shift card data
	}
	_lastWiegand = millis();	// Keep track of last wiegand bit received
	
}

void WIEGAND::ReadD1()
{
	_bitCount ++;				// Increment bit count for Interrupt connected to D1
	if (_bitCount>31)			// If bit count more than 31, process high bits
	{
		_cardTempHigh |= ((0x80000000 & _cardTemp)>>31);	// shift value to high bits
		_cardTempHigh <<= 1;
		_cardTemp |= 1;
		_cardTemp <<=1;
	}
	else
	{
		_cardTemp |= 1;			// D1 represent binary 1, so OR card data with 1 then
		_cardTemp <<= 1;		// left shift card data
	}
	_lastWiegand = millis();	// Keep track of last wiegand bit received
		
}

unsigned long WIEGAND::GetCardId (volatile unsigned long *codehigh, volatile unsigned long *codelow, char bitlength)
{
	unsigned long cardID=0;

	if (bitlength==26)								// EM tag
		cardID = (*codelow & 0x1FFFFFE) >>1;

	if (bitlength==34)								// Mifare 
	{
		*codehigh = *codehigh & 0x03;				// only need the 2 LSB of the codehigh
		*codehigh <<= 30;							// shift 2 LSB to MSB		
		*codelow >>=1;
		cardID = *codehigh | *codelow;
	}
	
    if (bitlength==37)								// EM tag
		cardID = (*codelow & 0x1FFFFFE) >>1;
	
	return cardID;
}

char translateEnterEscapeKeyPress(char originalKeyPress) {
    switch(originalKeyPress) {
        case 0x0b:        // 11 or * key
            return 0x0d;  // 13 or ASCII ENTER

        case 0x0a:        // 10 or # key
            return 0x1b;  // 27 or ASCII ESCAPE

        default:
            return originalKeyPress;
    }
}

bool WIEGAND::DoWiegandConversion ()
{
	unsigned long cardID;
	unsigned long sysTick = millis();
	     
              
	
	if ((sysTick - _lastWiegand) > 25)								// if no more signal coming through after 25ms
	{
		if ((_bitCount==26) || (_bitCount==34) || (_bitCount==8) || (_bitCount==4) || (_bitCount==37)) 	// bitCount for keypress=4 or 8, Wiegand 26=26, Wiegand 34=34
		{
			_cardTemp >>= 1;			// shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			if (_bitCount>32){			// bit count more than 32 bits, shift high bits right to make adjustment
				_cardTempHigh >>= 1;	
			}
			
			if(_debug)
            {         
                    Serial.println("------Debug DoWiegandConversion()------");
                    Serial.println("");
                    Serial.print("cardTemp HEX = ");
                    Serial.print(_cardTemp,HEX);
                    Serial.print(", DECIMAL = ");
                    Serial.print(_cardTemp);
                    Serial.print(", BIN = ");
                    Serial.print(_cardTemp,BIN);
                    Serial.print(", bitCount = ");
                    Serial.println(_bitCount);
                    Serial.println("-------------  -------  -------------");    
            }          
			
			

			if((_bitCount==26) || (_bitCount==34)|| (_bitCount==37))		// wiegand 26 or wiegand 34 or My wiegand 37
			{
				cardID = GetCardId (&_cardTempHigh, &_cardTemp, _bitCount);
				_wiegandType=_bitCount;
				_bitCount=0;
				_cardTemp=0;
				_cardTempHigh=0;
				_code=cardID;
				return true;				
			}
			else if (_bitCount==8)		// keypress wiegand with integrity
			{
				// 8-bit Wiegand keyboard data, high nibble is the "NOT" of low nibble
				// eg if key 1 pressed, data=E1 in binary 11100001 , high nibble=1110 , low nibble = 0001 
				char highNibble = (_cardTemp & 0xf0) >>4;
				char lowNibble = (_cardTemp & 0x0f);
				_wiegandType=_bitCount;					
				_bitCount=0;
				_cardTemp=0;
				_cardTempHigh=0;
				
				if (lowNibble == (~highNibble & 0x0f))		// check if low nibble matches the "NOT" of high nibble.
                {
                    _code = (int)translateEnterEscapeKeyPress(lowNibble);
					return true;
				}

                // TODO: Handle validation failure case!
			}
            else if (4 == _bitCount) {
                // 4-bit Wiegand codes have no data integrity check so we just
                // read the LOW nibble.
                _code = (int)translateEnterEscapeKeyPress(_cardTemp & 0x0000000F);

                _wiegandType = _bitCount;
                _bitCount = 0;
                _cardTemp = 0;
                _cardTempHigh = 0;

                return true;
            }
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 !=37, must be noise or nothing then.
			_lastWiegand=sysTick;
			_bitCount=0;			
			_cardTemp=0;
			_cardTempHigh=0;
			return false;
		}	
	}
	else
		return false;
}
