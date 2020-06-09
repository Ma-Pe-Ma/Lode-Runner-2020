#ifndef BUTTON_H
#define BUTTON_H

#include <time.h>  

class Button {
	private:
		bool pushed;
		float pushStartTime;
		float debounceTime = 0.05f;
		bool prevState = false;

		bool SimplePushed;
		bool impulsePushed;
		bool continousPushed;
		bool fasteningPushed;
		
		bool impulseChange;
		float impulseTime = 0.1f;
		
		float fasteningTime;
		float fasteningRef;
		
		bool change = false;
		bool release = false;
		
	public:
		void detect(bool);
		bool simple();			
		bool impulse();			
		bool continous();		
		bool fastening();		

		void setDebounceTime(float);
		void setImpulseTime(float);
		
		bool isPushed();
		bool released();		
};

#endif