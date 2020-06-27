#ifndef BUTTON_H
#define BUTTON_H

#include <time.h>
#include <atomic>

class Button {
	private:
		std::atomic_bool pushed{false};
		float pushStartTime;
		float debounceTime = 0.05f;
		bool prevState = false;

		bool simplePushed;
		bool impulsePushed;
		bool continuousPushed;
		bool fasteningPushed;
		
		bool impulseChange;
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