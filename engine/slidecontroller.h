#ifndef SLIDECONTROLLER_H
#define SLIDECONTROLLER_H

class scSlider {
	public:
		double starttimems;
		unsigned char state = 0;
		unsigned char show;
		// @@@ unsigned char show_rnd; blnRandomGeneratedShow kan dan verwijdert worden
		unsigned char show_wm; // White Magic
		unsigned char show_so; // Sound
		unsigned char show_pr; // Projection
        unsigned char show_hit; // Sound for hitting the smashpoint
		unsigned char show_ls; // Laser
		unsigned char show_mh; // MovingHead
		bool aquasmash;
		bool photo = false;
		double statetimems;
		char* rfid;
                char* show_cuecore;
};

class scSlide {
	public:
		unsigned char state;
		double statetimems;
		unsigned char selectedshow;
};

// prototypes
void setup();
void loop();
void finish(int bogus);
void intTimer(int signum);
void timerthread();

void stateInitEnter();
void stateInitUpdate();
void stateInitExit();
void stateStartEnter();
void stateStartUpdate();
void stateStartExit();
void stateSlideEnter();
void stateSlideUpdate();
void stateSlideExit();
void stateLeaveEnter();
void stateLeaveUpdate();
void stateLeaveExit();
void statePanicEnter();
void statePanicUpdate();
void statePanicExit();
void stateEmergencyEnter();
void stateEmergencyUpdate();
void stateEmergencyExit();

bool sliderPreCone1Enter(int i);
bool sliderPreCone1Leave();
bool sliderCone1Enter(int i);
bool sliderCone1Leave();
bool sliderPreCone2Enter(int i);
bool sliderPreCone2Leave();
bool sliderCone2Enter(int i);
bool sliderCone2Leave();
bool sliderCone3Enter(int i);
bool sliderCone3Leave();
bool sliderPostCone3Enter(int i);
bool sliderPostCone3Leave();

#endif
#ifdef FEATURE_BUTTON_SCORERESET
bool slideScoreReset();
#endif 