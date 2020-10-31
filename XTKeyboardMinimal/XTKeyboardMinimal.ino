 #include "myKeyboard.h"

#define DEBUG

#define ps_clk 3
#define ps_data 2

#define BREAK_KEY 0xF0
#define SPECIAL_KEY 0xE0

#define MAIN_KEYS_COUNT 85
#define SPECIAL_KEYS_COUNT 17

#define START 1
#define STOP 3
#define PARITY 4
#define INIT 5


struct map_struct {
	unsigned char usb_id;
	unsigned char xt_id;
};

typedef struct map_struct keyMap;

keyMap map_1[] = {
	{4, 0x1C}, // a
	{5, 0x32}, // b
	{6, 0x21}, // c
	{7, 0x23}, // d
	{8, 0x24}, // e
	{9, 0x2B}, // f
	{10, 0x34}, // g
	{11, 0x33}, // h
	{12, 0x43}, // i
	{13, 0x3B}, // j
	{14, 0x42}, // k
	{15, 0x4B}, // l
	{16, 0x3A}, // m
	{17, 0x31}, // n
	{18, 0x44}, // o
	{19, 0x4D}, // p
	{20, 0x15}, // q
	{21, 0x2D}, // r
	{22, 0x1B}, // s
	{23, 0x2C}, // t
	{24, 0x3C}, // u
	{25, 0x2A}, // v
	{26, 0x1D}, // w
	{27, 0x22}, // x
	{28, 0x35}, // y
	{29, 0x1A}, // z
	{39, 0x45}, // 0
	{30, 0x16}, // 1
	{31, 0x1E}, // 2
	{32, 0x26}, // 3
	{33, 0x25}, // 4
	{34, 0x2E}, // 5
	{35, 0x36}, // 6
	{36, 0x3D}, // 7
	{37, 0x3E}, // 8
	{38, 0x46}, // 9
	{53, 0x0E}, // `
	{45, 0x4E}, // -
	{46, 0x55}, // =
	{49, 0x5D}, /* \\ */
	{42, 0x66}, // backsapce
	{44, 0x29}, // space
	{43, 0x0D}, // tab
	{57, 0x58}, // caps
	{225, 0x12}, // left shift
	{224, 0x14}, // left ctrl
	{226, 0x11}, // left alt
	{229, 0x59}, // right shift
	{40, 0x5A}, // enter
	{41, 0x76}, // esc
	{58, 0x05}, // F1
	{59, 0x06}, // F2
	{60, 0x04}, // F3
	{61, 0x0C}, // F4
	{62, 0x03}, // F5
	{63, 0x0B}, // F6
	{64, 0x83}, // F7
	{65, 0x0A}, // F8
	{66, 0x01}, // f9
	{67, 0x09}, // f10
	{68, 0x78}, // f11
	{69, 0x07}, // f12
	{71, 0x7E}, // SCROLL
	{47, 0x54}, // [
	{83, 0x77}, // Num Lock
	{85, 0x7C}, // Keypad *
	{86, 0x7B}, // Keypad -
	{87, 0x79}, // Keypad +
	{99, 0x71}, // Keypad .
	{98, 0x70}, // Keypad 0
	{89, 0x69}, // Keypad 1
	{90, 0x72}, // Keypad 2
	{91, 0x7A}, // Keypad 3
	{92, 0x6B}, // Keypad 4
	{93, 0x73}, // Keypad 5
	{94, 0x74}, // Keypad 6
	{95, 0x6C}, // Keypad 7
	{96, 0x75}, // Keypad 8
	{97, 0x7D}, // Keypad 9
	{48, 0x5B}, // ]
	{51, 0x4C}, // ;
	{52, 0x52}, // \'
	{54, 0x41}, // ,
	{55, 0x49}, // .
	{56, 0x4A}, // /
  {227, 0x61}
};

keyMap map_2[] = {
	{70, 0x12}, // prt scr
  {56, 0x4A}, // /
	{40, 0x5A}, // keypad enter
	{227, 0xFF}, // left gui
	{231, 0xFF}, // right gui
	{228, 0x14}, // right ctrl
	{230, 0x11}, // right alt
	{73, 0x70}, // insert
	{74, 0x6C}, // home
	{75, 0x7D}, // page up
	{76, 0x71}, // delete
	{77, 0x69}, // end
	{78, 0x7A}, // page down
	{82, 0x75}, // u arrow
	{80, 0x6B}, // l arrow
	{81, 0x72}, // d arrow
	{79, 0x74}, // r arrow
};


byte cycles = 0;
unsigned char value = 0;
byte state = INIT;
byte got_data = 0;

byte special_k = 0; // flag 0=normal 1=special
byte break_k = 0; // flag 0=no break 1=break
byte hex_code_is = 0; // flag if 0 no data, if 1 data
unsigned char hex_code;

myKeyboard_ myKeyboard;

void setup() {
	#ifdef DEBUG
	Serial.begin(9600);
	#endif

	pinMode(ps_clk, INPUT);
	pinMode(ps_data, INPUT);
	//enable clock interupt
	attachInterrupt(0, clock, FALLING);
}

void loop() {
	//read character
	unsigned char code = read_sc();
	
	if (code != 0){
		#ifdef DEBUG
		Serial.print(code, HEX);
		Serial.print("--> ");
		#endif
		
		if(code == (unsigned char)SPECIAL_KEY){
			if(special_k == 1) hex_code_is = 0;
			special_k = 1;
			#ifdef DEBUG
			Serial.println("special found");
			#endif
		}
		else if(code == (unsigned char)BREAK_KEY){
			break_k = 1;
			#ifdef DEBUG
			Serial.println("break found");
			#endif
			hex_code_is = 0;
		}
		else{
			hex_code = code;
			#ifdef DEBUG
			Serial.println("hex found");
			#endif
			hex_code_is = 1;
		}
   
		if(hex_code_is == 1){
			if((special_k == 0) && (break_k == 0)){
				press_key(hex_code);
			}
			else if((special_k == 0) && (break_k == 1)) {
				release_key(hex_code);
				
				special_k = 0;
				break_k = 0;
				hex_code_is = 0;
				
				#ifdef DEBUG
				Serial.println("-----------------");
				#endif
			}
			else if((special_k == 1) && (break_k == 0)) {
				press_special_key(hex_code);
			}
			else if((special_k == 1) && (break_k == 1)){
        release_special_key(hex_code);
          
        special_k = 0;
        break_k = 0;
        hex_code_is = 0;
                                
  			
  			#ifdef DEBUG
  			Serial.println("-----------------");
  			#endif
		  }
		}
	}
}

void release_key(unsigned char code) {
	for(int i = 0; i < MAIN_KEYS_COUNT; i++)
	{
		if (map_1[i].xt_id == code)
		{
			#ifdef DEBUG
			Serial.println("-release-");
			#endif
			myKeyboard.release_sc(map_1[i].usb_id);
			break;
		}
	}
}

void press_key(unsigned char code) {
	for(int i = 0; i < MAIN_KEYS_COUNT; i++)
	{
		if (map_1[i].xt_id == code)
		{
			#ifdef DEBUG
			Serial.println("-pressed-");
			#endif
			myKeyboard.press_sc(map_1[i].usb_id);
			break;
		}
	}
}

void release_special_key(unsigned char code) {
	for(int i = 0; i < SPECIAL_KEYS_COUNT; i++)
	{
		if (map_2[i].xt_id == code)
		{
			#ifdef DEBUG
			Serial.println("-s_release-");
			#endif
			myKeyboard.release_sc(map_2[i].usb_id);
			break;
		}
	}
}

void press_special_key(unsigned char code) {
	for(int i = 0; i < SPECIAL_KEYS_COUNT; i++)
	{
		if (map_2[i].xt_id == code)
		{
			#ifdef DEBUG
			Serial.println(" -s_pressed-");
			#endif
			myKeyboard.press_sc(map_2[i].usb_id);
			break;
		}
	}
}


//read byte from buffer
unsigned char read_sc() {
	if (got_data) {
		got_data = 0;
		return value;
	}
	return 0;
}

void clock() {
	if (state == INIT)
	{
		if (digitalRead(ps_data) == LOW)
		{
			state = START;
			cycles = 0;
			got_data = 0;
			value = 0;
			return;
		}
	}
	if (state == START)
	{
		value |= (digitalRead(ps_data) << cycles);
		cycles++;
		if (cycles == 8) state = PARITY;
		return;
	}
	if (state == PARITY)
	{
		state = STOP;
		return;
	}
	if (state == STOP)
	{
		if (digitalRead(ps_data) == HIGH)
		{
			state = INIT;
			got_data = 1;
			return;
		}
	}
}
