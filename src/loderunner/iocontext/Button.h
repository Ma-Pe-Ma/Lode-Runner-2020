#ifndef BUTTON_H
#define BUTTON_H

class Button {
private:
	bool pushed = false;
	bool release = false;

	bool simplePushed = false;

	float lastImpulse = 0.0f;
	float impulseTime = 0.05f;
public:
	void detect(bool);
	bool simple();			
	bool impulse();			
	bool continuous();		

	void setImpulseTime(float);
		
	bool released();
};

#endif