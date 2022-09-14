#ifndef BUTTON_H
#define BUTTON_H

#include <time.h>
#include <atomic>

class Button {
	private:
		std::atomic_bool pushed{false};
		float pushStartTime = ((float) clock() / CLOCKS_PER_SEC);
		float debounceTime = 0.05f;
		bool prevState = false;

		bool simplePushed = false;
		bool impulsePushed = false;
		bool continuousPushed = false;
		bool fasteningPushed = false;
		
		bool impulseChange = false;
		float impulseTime = 0.1f;
		
		float fasteningTime;
		float fasteningRef;
		
		bool change = false;
		bool release = false;
		
	public:
		void detect(bool);
		void detectAlter(bool);
		bool simple();			
		bool impulse();			
		bool continuous();
		bool fastening();		

		void setDebounceTime(float);
		void setImpulseTime(float);
		
		bool isPushed();
		bool released();
};

#endif