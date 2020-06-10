/*
 *
 * Slide Controller (Linux)
 *  by Sven van Trijp
 *  for IBG ProjeX
 *  on September 26 2013
 * 
 */


// libraries
#include <stdio.h> // printf
#include <signal.h> // SIGTERM
#include <syslog.h> // openlog
#include <stdlib.h> // EXIT_SUCCESS
#include <unistd.h> // sleep
#include <string.h>
#include <sys/time.h>
#include <thread>	// std::thread
#include "defines.h"
#include "slidecontroller.h"
#include "classFiniteStateMachine.h" // from http://playground.arduino.cc/Code/FiniteStateMachine
#include "classDelayTimer.h"
#include "scal.h"
#ifdef FEATURE_PHOTO
#include "camera.h"
#include "web.h"
#endif
#ifdef FEATURE_COUNTDOWNLIGHT
#include "countdownlight.h"
#endif
#ifdef FEATURE_PROJECTION
#include "projection.h"
#endif
#ifdef FEATURE_GUI
#include "gui.h"
#endif
#ifdef FEATURE_GUI_BUTTONS
#include "buttons.h" // not in scal.cpp beacause class will not be recognised.
#endif
#ifdef FEATURE_MP3
#include "soundbox.h"
#endif

#ifdef FEATURE_API
#include "api.h"
#endif


using namespace std;
//using std::thread;

// globals
int intIntTimer = 0;
int intShowSelected = 0; // used by gui.cpp or buttons.cpp
int intShowRunning = 0;
int intShowLast = 0;
//#ifdef SLIDE_MODE_WHITEMAGIC // @@@
int intShowSelectedWM = 0;
//#endif
// #ifdef SLIDE_MODE_WHITEMAGIC // @@@ MP3 define
int intShowSelectedSO = 0;
int intShowSelectedSOhit = 0;

// #endif
//#ifdef SLIDE_MODE_BEAMER
int intShowSelectedBM = 0;
//#endif

#ifdef FEATURE_HOLD_ONEXIT
bool boolHoldOnExitStarted = false;
#endif

#ifdef SLIDE_MODE_SENSOR
	bool blnSettingSensorsInstalled = true;
#else
	bool blnSettingSensorsInstalled = false;
#endif
#ifdef FEATURE_PHOTO
	bool blnTakePhoto = false; // @@@ global not used anymore, now local in scSetShow()
#endif
#ifdef FEATURE_TIME
	unsigned long ulnSlideTime = 0;
	float fltSlideSpeedMS = 0;
	float fltSlideSpeedKmH = 0;
	float fltSlideSpeedMph = 0;
	unsigned long ulnSlideTimeDispOld = 0;
	unsigned long ulnSlideTimeDisp = 0;
	unsigned long lngSlideSpeedKmHDisp = 0;
	unsigned long ulnSlideSpeedMphDisp = 0;
	unsigned long ulnBestSlideTime = TIME_SLIDE_MAX;
	unsigned long ulnBestSlideTimeDisp = 0;
	int intSlideScore = 0;
	int intBestScore = 0;
#endif
        int intLastRandom = 0;
#ifdef SLIDE_MODE_AQUASMASH
unsigned char nCtdColorSets[5][CTD_SUPPORTED] = {
  {CTD_COLOR_BLU, CTD_COLOR_GRN, CTD_COLOR_WHT, CTD_COLOR_RED, CTD_COLOR_GRN,  CTD_COLOR_RED, CTD_COLOR_RED, CTD_COLOR_GRN,   CTD_COLOR_BLU, CTD_COLOR_RED,  CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_RED, CTD_COLOR_GRN, CTD_COLOR_RED},
  {CTD_COLOR_RED, CTD_COLOR_BLU, CTD_COLOR_GRN, CTD_COLOR_WHT, CTD_COLOR_RED,  CTD_COLOR_RED, CTD_COLOR_GRN, CTD_COLOR_RED,   CTD_COLOR_RED, CTD_COLOR_BLU,  CTD_COLOR_RED, CTD_COLOR_GRN, CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_GRN},
  {CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_BLU, CTD_COLOR_RED, CTD_COLOR_WHT,  CTD_COLOR_RED, CTD_COLOR_RED, CTD_COLOR_GRN,   CTD_COLOR_BLU, CTD_COLOR_RED,  CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_GRN, CTD_COLOR_GRN, CTD_COLOR_RED},
  {CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_RED, CTD_COLOR_BLU, CTD_COLOR_RED,  CTD_COLOR_GRN, CTD_COLOR_GRN, CTD_COLOR_RED,   CTD_COLOR_WHT, CTD_COLOR_BLU,  CTD_COLOR_RED, CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_RED, CTD_COLOR_GRN},
  {CTD_COLOR_RED, CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_RED, CTD_COLOR_BLU,  CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_RED,   CTD_COLOR_BLU, CTD_COLOR_WHT,  CTD_COLOR_GRN, CTD_COLOR_RED, CTD_COLOR_GRN, CTD_COLOR_GRN, CTD_COLOR_RED}
};
unsigned char nCtdColorSetSelected = 0;
int intCtdColorValues[4] = {CTD_POINTS_RED, CTD_POINTS_GRN, CTD_POINTS_BLU, CTD_POINTS_WHT};
bool blnCtdHit[CTD_SUPPORTED];
int intCtdRedHit = 0;
int intCtdGreenHit = 0;
int intCtdBlueHit = 0;
int intCtdWhiteHit = 0;
int intCtdTotalHit = 0;
//int intBestScore = 0;
#endif
#ifdef SLIDE_MODE_AQUASMASH
	bool blnAquaSmash = true;
#else
	bool blnAquaSmash = false;
#endif
bool blnFirstRun = true;
scSlider scSliders[10];
int intSliderIndexLast = 0;
scSlide scSlide;
bool blnFazerPumping = false;
		
// init states
State steInit = State(stateInitEnter, stateInitUpdate, stateInitExit);
State steStart = State(stateStartEnter, stateStartUpdate, stateStartExit);
//State steSlide = State(stateSlideEnter, stateSlideUpdate, stateSlideExit);
//State steLeave = State(stateLeaveEnter, stateLeaveUpdate, stateLeaveExit);
State stePanic = State(statePanicEnter, statePanicUpdate, statePanicExit);
#ifdef FEATURE_EMERGENCY_BUTTON
State steEmergency = State(stateEmergencyEnter, stateEmergencyUpdate, stateEmergencyExit);
#endif

// init state machine
FSM fsmController = FSM(steInit);

// init delaytimers
DelayTimer dltController = DelayTimer(RESOLUTIONMS_TIMER);

scSensor scSensors[SENSOR_MAX];

#ifdef FEATURE_GUI_BUTTONS
classButtons devButtons = classButtons();
#endif

int main(int argc, char **argv) {
	SC_DEBUG("%s started...\n", argv[0]);

// Testing some stuff
	struct timeval tv;
	gettimeofday(&tv, NULL);
//	printf("time = %li + %li = %f\n", tv.tv_sec, tv.tv_usec, (double) (tv.tv_sec * 1000) + (tv.tv_usec / 1000));

	setup();
	
	// The main loop
	SC_DEBUG("@slidecontroller.cpp/main(...): mainloop...\n");
	while (true) {
		loop();
		usleep(1);
	}

	SC_ERROR("@slidecontroller.cpp/main(...): !! This should not happen!\n");
	return (EXIT_SUCCESS);
}


void setup() {
	// welcome message
	printf("\n");
	printf(PRINTF_BOLDBLUE);
	printf("Slide Controller\n");
	printf("v%s (%s)\n", VERSION_NUMBER, VERSION_NAME);
	printf("Copyright (c) %d SvT\n", VERSION_YEAR);
	printf(PRINTF_RESET);
	printf("\n");

	// Set signal handlers
	signal(SIGTERM, finish);
	signal(SIGINT, finish);

	std::thread(timerthread).detach();

	// Initialize random seed
	srand(time(NULL));
	
#ifdef DO_DEBUG
	openlog("slidecontroller", LOG_PID | LOG_NOWAIT, LOG_AUTH);
	// @@@ Use this ^
#endif
	
	// some summary of the settings:

#ifdef SLIDE_MODE_AQUASMASH
	printf("  SLIDE_MODE_AQUASMASH\n");
#endif
#ifdef SLIDE_MODE_WHITEMAGIC
	printf("  SLIDE_MODE_WHITEMAGIC\n");
#endif
#ifdef SLIDE_MODE_SLIDECHAMP
	printf("  SLIDE_MODE_SLIDECHAMP\n");
#endif
	
#ifdef SLIDE_MODE_SENSOR // @@@ This will be removed later...
	printf("  SLIDE_MODE_SENSOR\n");
#ifdef TIME_SLIDE_AVG
	printf("  TIME_SLIDE_AVG = %d\n", TIME_SLIDE_AVG);
#endif
	printf("  TIME_SLIDE_MAX = %d\n", TIME_SLIDE_MAX);
	printf("  TIME_SLIDE_MIN = %d\n", TIME_SLIDE_MIN);
	printf("  TIME_SLIDE_EXIT = %d\n", TIME_SLIDE_EXIT);
#else
	printf("  !SLIDE_MODE_SENSOR\n");
	printf("  TIME_RED = %d\n", TIME_RED);
	printf("  TIME_GREEN = %d\n", TIME_GREEN);
	printf("  TIME_ORANGE = %d\n", TIME_ORANGE);
#endif	

#ifdef SLIDE_LENGTH
	printf("  SLIDE_LENGTH = %d\n", SLIDE_LENGTH);
#endif

#ifdef DISABLE_PANIC
	printf("  DISABLE_PANIC\n");
#endif
	printf("  \n");
	
/*	
	// some testing
	unsigned char show = 1;
	
	while(true) {
		if (show < 10) {
			show++;
		} else {
			show = 2;
		}
		lightSetRed();
		sleep(5);
		lightSetOrange();
		sleep(5);
		showStart(EDX_CONE_1, show, DMX_SHOW_LAMPS);
		sleep(10);
	}
*/

	// Set up sensors
#ifdef SENSOR_IN
	scSensors[SENSOR_IN].init(IO_SENSOR_TYPE, IP_IO_SENSORS_1, 1, SENSOR_IN, false);
#endif
#ifdef SENSOR_OUT
	scSensors[SENSOR_OUT].init(IO_SENSOR_TYPE, IP_IO_SENSORS_1, 2,   SENSOR_OUT, false);
#endif
	scSensors[SENSOR_PANIC].init(IO_SENSOR_TYPE, IP_IO_SENSORS_1, 3, SENSOR_PANIC, true);
#ifdef SENSOR_EMERGENCY
	scSensors[SENSOR_EMERGENCY].init(IO_SENSOR_TYPE, IP_IO_SENSORS_1, 4, SENSOR_EMERGENCY, true);
#endif

//	scSensors[SENSOR_SCORERESET].init(IO_SENSOR_TYPE, IP_IO_SENSORS_1, 5, SENSOR_SCORERESET, false);

        
#ifdef SLIDE_MODE_AQUASMASH
#ifdef IP_IO_CTD_1
	scSensors[SENSOR_OFFSET_CTD + ( 1 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_1, 1, SENSOR_OFFSET_CTD + ( 1 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + ( 2 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_1, 2, SENSOR_OFFSET_CTD + ( 2 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + ( 3 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_1, 3, SENSOR_OFFSET_CTD + ( 3 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + ( 4 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_1, 4, SENSOR_OFFSET_CTD + ( 4 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + ( 5 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_1, 5, SENSOR_OFFSET_CTD + ( 5 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + ( 6 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_1, 6, SENSOR_OFFSET_CTD + ( 6 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + ( 7 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_1, 7, SENSOR_OFFSET_CTD + ( 7 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + ( 8 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_1, 8, SENSOR_OFFSET_CTD + ( 8 - 1), false);
#endif
#ifdef IP_IO_CTD_2
	scSensors[SENSOR_OFFSET_CTD + ( 9 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_2, 1, SENSOR_OFFSET_CTD + ( 9 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + (10 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_2, 2, SENSOR_OFFSET_CTD + (10 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + (11 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_2, 3, SENSOR_OFFSET_CTD + (11 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + (12 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_2, 4, SENSOR_OFFSET_CTD + (12 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + (13 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_2, 5, SENSOR_OFFSET_CTD + (13 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + (14 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_2, 6, SENSOR_OFFSET_CTD + (14 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + (15 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_2, 7, SENSOR_OFFSET_CTD + (15 - 1), false);
	scSensors[SENSOR_OFFSET_CTD + (16 - 1)].init(IO_CTD_TYPE, IP_IO_CTD_2, 8, SENSOR_OFFSET_CTD + (16 - 1), false);
#endif
#endif //SLIDE_MODE_AQUASMASH
	
#ifdef FEATURE_GUI_BUTTONS
	bool feature_photo = false;
	bool feature_aquasmash = false;
	bool btn1random = false;
#ifdef FEATURE_PHOTO
	feature_photo = true;
#endif
#ifdef SLIDE_MODE_AQUASMASH
	feature_aquasmash = true;
#endif
#ifdef IO_BUTTON1_RANDOM
	btn1random = true;
#endif
	devButtons.buttonsInit(10, IP_IO_BUTTONS_1, IP_IO_BUTTONS_2, 25, feature_photo, feature_aquasmash, btn1random, true);
#endif
	
	scalInit();
// @@@
//	prjSetPower(true);
#ifdef FEATURE_API
	apiInit();
#endif
}


void loop() {
	if (intIntTimer > 0) {
//		SC_DEBUG(".");
		dltController.update();
		fsmController.update();
		// do something...

/*
		// @@@ Testing
		unsigned int now = (unsigned) time(NULL);
		int oldshowid = intShowRunning;
		do {
			intShowRunning = (rand() % 11) + 1;
		} while (intShowRunning == oldshowid);
		SC_DEBUG("@slidecontroller.cpp/loop(): Starting show %d\n", intShowRunning);
		showStart(EDX_CONE_1, intShowRunning, DMX_SHOW_LAMPS);
		laserStart(intShowRunning);
		showStart(EDX_CONE_2, intShowRunning, DMX_SHOW_LAMPS);
		showStart(EDX_CONE_3, intShowRunning, DMX_SHOW_LAMPS);
		showStart(EDX_END, intShowRunning, DMX_SHOW_LAMPS);
		discoStart(intShowRunning);
//		soundStart(MPD_CONE_1, intShowRunning);
//		soundStart(MPD_CONE_2, intShowRunning);
//		soundStart(MPD_CONE_3, intShowRunning);
		int theme = (10 * (rand() % 5) + 1) + 2;
		if (theme == 52) theme = 51; 
		prjSetShow(theme);
		// make photo at startup:
 		scProcessSliderDataThread(now, intShowRunning, 0, 0, 0, 0, 0, 0, 0, 0, true);
		systemCleanup();

		usleep(10 * 1000000);

*/  
  
  
		
		if (intIntTimer > 10) {
//			printf("+^"); fflush(stdout);
		}
		intIntTimer--;
	}
}

void finish(int bogus) {
	SC_ERROR("We're closing...\n");
	systemSetSecure();
	SC_DEBUG("Bye.\n");

#ifdef DO_DEBUG
	closelog();
#endif

	exit(0);
}
/*
void intTimer(int signum) {
	intIntTimer++;
	//	SC_DEBUG("timer! %d\n", intIntTimer);
}
*/
void timerthread() {
	SC_DEBUG("@slidecontroller.cpp/timerthread(): Starting thread...\n");
	while(true) {
		usleep(RESOLUTIONMS_TIMER * 1000);
//		usleep(1000 * 1000);
		intIntTimer++;
//		printf("T");
	}
}


void stateInitEnter() {
	SC_DEBUG("@slidecontroller.cpp/stateInitEnter(): STATE INIT\n");
	systemCleanup();
        srand(time(NULL));
#ifdef FEATURE_PHOTO

	// Take test picture at init
	unsigned int now = (unsigned) time(NULL);
	scProcessSliderDataThread(now, 0, 0, 0, 0, 0, 0, 0, 0, 0, true);
#endif
	
	if (!blnFirstRun) {
		dltController.create(TIMER_SLIDE_INIT, TIME_SLIDE_INIT);
		SC_DEBUG("@slidecontroller.cpp/stateInitEnter(): Waiting for the pump to start etc...\n");
	} else {
		dltController.create(TIMER_SLIDE_INIT, 2000);
		blnFirstRun = false;
	}

#ifdef FEATURE_TOUCHSCREEN
	guiResetTouchscreen(GUI_SLIDE,BRAND_BRIGHTSIGN);
#endif
	
#ifdef FEATURE_GUI_BUTTONS
	devButtons.setButton(0);
#endif
#ifdef FEATURE_DISPLAYBOARD
	number_to_display(SC_DISPLAY_TIME, 1, SC_DISPLAY_TIME_L, SC_DISPLAY_TIME_D, true);
	
	number_to_display(SC_DISPLAY_SPEED, 1, SC_DISPLAY_SPEED_L, SC_DISPLAY_SPEED_D, true);
	
	number_to_display(SC_DISPLAY_BEST, 1, SC_DISPLAY_BEST_L, SC_DISPLAY_BEST_D, true);
#endif	
	SC_DEBUG("@slidecontroller.cpp/stateInitEnter(): Done\n");
}

void stateInitUpdate() {
	
	if (dltController.finished(TIMER_SLIDE_INIT)) {
		printf("++\n");
		SC_DEBUG("@slidecontroller.cpp/stateInitUpdate(): dltController.finished(TIMER_SLIDE_INIT)...\n");
		fsmController.transitionTo(steStart);
	}
}

void stateInitExit() {
	systemSetNormal();
}

void stateStartEnter() {
#ifdef FEATURE_EMERGENCY_BUTTON
//	if (scSensors[SENSOR_EMERGENCY].state(true)) {
//		SC_DEBUG("slidecontroller.cpp/stateStartUpdate(): EMERGENCY MODE\n");
//		fsmController.transitionTo(steEmergency);
//	}
	if(!getEmergencyInput(SENSOR_EMERGENCY))//if no 24v in
	{
		SC_DEBUG("slidecontroller.cpp/stateStartUpdate(): EMERGENCY MODE\n");
		fsmController.transitionTo(steEmergency);
	}
#endif
	SC_DEBUG("\n");
	SC_DEBUG("@slidecontroller.cpp/stateStartEnter(): STATE START\n");
	lightSetGreen();
        showStop(EDX_SLIDE,DMX_SHOW_LAMPS); //green light stop s show because of sensor location
#ifdef FEATURE_TOUCHSCREEN_SPINETIX
	guiSetScreenActive(GUI_SLIDE, true);	
#else
	// Touchscreen disabled
	char filename[100];
	sprintf(filename, "%s/slide_disabled", DIR_TMP);
	unlink(filename);
#endif
	
	if (!blnSettingSensorsInstalled) {
		SC_DEBUG("@slidecontroller.cpp/stateStartEnter(): No sensors: starting timer...\n");
		dltController.create(TIMER_GREEN, TIME_GREEN);
	}
#ifdef FEATURE_PHOTO	
	blnTakePhoto = false;
	#ifdef TEST_ALWAYS_PHOTO
	blnTakePhoto = true;
	#endif
#endif
#ifdef FEATURE_TIME
	ulnSlideTime = 0;
#endif
#ifdef SLIDE_MODE_AQUASMASH
	blnAquaSmash = false;
	// Reset all values
	
	intSlideScore = 0;
	intCtdRedHit = 0;
	intCtdGreenHit = 0;
	intCtdBlueHit = 0;
	intCtdWhiteHit = 0;
	intCtdTotalHit = 0;
	for (int i = 0; i < CTD_SUPPORTED; i++) {
		blnCtdHit[i] = false;
	}
#endif

#ifdef SLIDE_MODE_WHITEMAGIC
#ifdef SLIDE_MODE_AQUASMASH
	showStop(EDX_SLIDE,DMX_SHOW_LAMPS+CTD_SUPPORTED);
#else
	showStop(EDX_SLIDE,DMX_SHOW_LAMPS);
#endif
#endif	

	dltController.create(TIMER_START_REFRESH, TIME_START_REFRESH);
	
	// Prepare for START
#ifdef SENSOR_IN
	scSlide.state = STATE_SLIDE_GO;
#else
	scSlide.state = STATE_SLIDE_BLOCK;
#endif
	SC_DEBUG("@slidecontroller.cpp/stateStartEnter(): STATE START ready...\n");
}
//-----------------------------------------------------------------------------------------
void stateStartUpdate() {
//	SC_DEBUG("Update...\n");
	// This is where it all happens (not going to stateSlide anymore...)
	
	// @@@ Also implement all other states
	
	// Get the time
	struct timeval tv;
	gettimeofday(&tv, NULL);
	double now = (double) (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#ifdef FEATURE_EMERGENCY_BUTTON
//	if (scSensors[SENSOR_EMERGENCY].state(true)) {
//		SC_DEBUG("slidecontroller.cpp/stateStartUpdate(): EMERGENCY MODE\n");
//		fsmController.transitionTo(steEmergency);
//	}
	if(!getEmergencyInput(SENSOR_EMERGENCY))
	{
		SC_DEBUG("slidecontroller.cpp/stateStartUpdate(): EMERGENCY MODE\n");
		fsmController.transitionTo(steEmergency);
	}
#endif
#ifdef FEATURE_EMERGENCY_BUTTON
	if (scSensors[SENSOR_PANIC].state(false) && getEmergencyInput(SENSOR_EMERGENCY)) {
		SC_DEBUG("slidecontroller.cpp/stateStartUpdate(): SENSOR_PANIC\n");
		fsmController.transitionTo(stePanic);
	}
#else
		if (scSensors[SENSOR_PANIC].state(false)) {
		SC_DEBUG("slidecontroller.cpp/stateStartUpdate(): SENSOR_PANIC\n");
		fsmController.transitionTo(stePanic);
	}
#endif
	
#ifdef FEATURE_BUTTON_SCORERESET
if(scSensors[SENSOR_SCORERESET].state(true)) {
    slideScoreReset();
}
#endif

#ifdef FEATURE_GUI_BUTTONS
	devButtons.getButtons(); // modifies intShowSelected
#endif
		
	// Someone enters the slide (S1)
#ifdef SENSOR_IN	
	if (scSensors[SENSOR_IN].state(true)) {
#else
	if (scSlide.state == STATE_SLIDE_GO) {
#endif
//----------------------
//		intSliderIndexLast++;
            
                
		intSliderIndexLast = 1;
		scSliders[intSliderIndexLast].state = STATE_SLIDER_SLIDE;
		scSliders[intSliderIndexLast].starttimems = now;
#ifdef FEATURE_RANDOMSHOW
		bool blnRandomGeneratedShow = false;
		if (intShowSelected == 0) {
#ifdef FEATURE_GUI_BUTTONS
			intShowSelected = (10 * (rand() % 3 + 1)) + (2 * (rand() % 2 + 2)); // random show (6) without photo or aquasmash
#else
			//intShowSelected = (10 * (rand() % 4 + 1)) + (2 * (rand() % 3 + 2)); // random show (4x3) without photo or aquasmash
			intShowSelected = (10 * (rand() % 4 + 1)) + 2;//+1 // random show (4) always first subshow no photo
			while (intShowSelected == intShowLast)
			{
				intShowSelected = (10 * (rand() % 4 + 1)) + 2;//+1 // random show (4) always first subshow no photo
			}
			while ((intShowSelected) == 0 || (intShowSelected == intShowLast)) {
				int arrShow[12] = {12,13,14, 22,23,24, 32,33,34, 42,43,44}; // select between no photo shows
				int intRandomIndex = rand() % 12; //random number between zero and three because arrays use zero index
				intShowSelected = arrShow[intRandomIndex];
			}
                        //intShowSelected = 52;// steady: no moving light - no sound -no moving head
#endif
			blnRandomGeneratedShow = true;
			SC_HIGHLIGHT("slidecontroller.cpp/stateStartUpdate(): Random show (none selected) = %d\n", intShowSelected);
		}
#endif // FEATURE_RANDOMSHOW, else: default show in scSetShow()
		// random from touchscreen:
		if (intShowSelected == 99) {
			intShowSelected = (10 * (rand() % 4 + 1)) + (2 * (rand() % 3 + 2)); // random show without photo or aquasmash
			SC_HIGHLIGHT("slidecontroller.cpp/stateStartUpdate(): Random show (TouchScreen random) = %d\n", intShowSelected);
		}
		scSliders[intSliderIndexLast].show = intShowSelected; // scSlide.selectedshow;
		intShowLast = intShowSelected;
		scSetShow(&scSliders[intSliderIndexLast]);
#ifdef FEATURE_RANDOMSHOW
#ifndef FEATURE_ALWAYSSOUND
		if (blnRandomGeneratedShow) {
			scSliders[intSliderIndexLast].show_so = SOUND_NO;
			SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): sound disabled during automatic show\n");
		}
#endif
#endif // otherwise default show in scSetShow()	
#ifdef FEATURE_API
		char *value;
		value = (char *) malloc(100);
		if (apiGetValue("rfid", value, true)) {
			//[intSliderIndexLast].rfid = atoi(value);
			scSliders[intSliderIndexLast].photo = true;
                        scSliders[intSliderIndexLast].rfid = value;
			SC_HIGHLIGHT("slidecontroller.cpp/stateStartUpdate(): Scanned RFID = %lu\n", scSliders[intSliderIndexLast].rfid);
		} else {
			scSliders[intSliderIndexLast].rfid = 0;
			scSliders[intSliderIndexLast].photo = false;
			SC_HIGHLIGHT("slidecontroller.cpp/stateStartUpdate(): No scanned RFID\n");
		}
                
#endif
#ifdef FEATURE_CUECORE
                setCueCore(CUECORE_OFF);
#endif

//---------------------- next state:
		scSlide.state = STATE_SLIDE_WAIT;
		scSlide.statetimems = now;
		intShowSelected = 0; // scSlide.selectedshow

		SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): SENSOR_IN / Slider %d enters the slide\n", intSliderIndexLast);
#ifdef SENSOR_IN	
		lightSetRed();
#else
		SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): No SENSOR_IN: starting timer...\n");
		dltController.create(TIMER_GREEN, TIME_GREEN);

#endif
		scSensors[SENSOR_OUT].state(false); // Enable SENSOR_OUT
#ifdef FEATURE_CONESENSOR
                scSensors[SENSOR_CONE_IN].state(false);
                scSensors[SENSOR_CONE_OUT].state(false);
#endif
#ifdef SLIDE_MODE_AQUASMASH	
		if (scSliders[intSliderIndexLast].aquasmash) {
			nCtdColorSetSelected = rand() % 5;
			SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): intCtdColorSet %u\n", nCtdColorSetSelected);
			asLoadCtd(nCtdColorSets[nCtdColorSetSelected]); // from DMX_SHOW_LAMPS
			SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): Starting Whitemagic show %d\n", scSliders[intSliderIndexLast].show_wm);
			showStart(EDX_SLIDE, scSliders[intSliderIndexLast].show_wm, DMX_SHOW_LAMPS + CTD_SUPPORTED); //include CTD in ShowStart
		} else {
			SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): Starting Whitemagic show %d\n", scSliders[intSliderIndexLast].show_wm);
			showStart(EDX_SLIDE, scSliders[intSliderIndexLast].show_wm, DMX_SHOW_LAMPS+CTD_SUPPORTED);
		}
#else
		SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): Starting Whitemagic show %d\n", scSliders[intSliderIndexLast].show_wm);
//                if(scSliders[intSliderIndexLast].show_pr == 52) {
//                    setlampsGreen();
//                }
//                else {
                    showStart(EDX_SLIDE, scSliders[intSliderIndexLast].show_wm, DMX_SHOW_LAMPS);
//                }
				
#endif
#ifdef FEATURE_MP3
		soundStart(MPD_SLIDE, scSliders[intSliderIndexLast].show_so);
#endif
#ifdef FEATURE_LASER
		//laserStart(scSliders[intSliderIndexLast].show_ls);
#endif
#ifdef FEATURE_MOVINGHEAD
		//movingheadStart(scSliders[intSliderIndexLast].show_mh);
#endif
		
#ifdef FEATURE_ECUE
		//play ecue show for LED rings
		//LedRingSetShow(scSliders[intSliderIndexLast].show);
		LedRingSetShow(scSliders[intSliderIndexLast].show_ecue);
#endif
#ifdef FEATURE_PROJECTION
//beamers on
#ifdef IP_PROJ_BEAMER_1
	pjlSetPower(IP_PROJ_BEAMER_1,true);
#endif
#ifdef IP_PROJ_BEAMER_2
	pjlSetPower(IP_PROJ_BEAMER_2,true);
#endif
#ifdef IP_PROJ_BEAMER_3
	pjlSetPower(IP_PROJ_BEAMER_3,true);
#endif
#ifdef IP_PROJ_BEAMER_4
	pjlSetPower(IP_PROJ_BEAMER_4,true);
#endif
#ifdef IP_PROJ_BEAMER_5
	pjlSetPower(IP_PROJ_BEAMER_5,true);
#endif

#if SOURCEBRAND == BRAND_BRIGHTSIGN
	//char* showsignal = (char*)malloc(sizeof("00"));
	char showsignal1[] = "S00",showsignal2[] = "S00",showsignal3[] = "S00";
	int intSelectedShow = scSliders[intSliderIndexLast].show_pr;
	
#ifdef IP_PROJ_MD_1
	SC_DEBUG("show: %d\n",intSelectedShow);
	sprintf(showsignal1,"%s%d","S",intSelectedShow);
	brsSetShow(IP_PROJ_MD_1,IP_PROJ_MD_PORT,showsignal1);
	intSelectedShow++;
#endif
#ifdef IP_PROJ_MD_2
	SC_DEBUG("show: %d\n",intSelectedShow);
	sprintf(showsignal2,"%s%d","S",intSelectedShow);
	brsSetShow(IP_PROJ_MD_2,IP_PROJ_MD_PORT,showsignal2);
	intSelectedShow++;
#endif
#ifdef IP_PROJ_MD_3
	SC_DEBUG("show: %d\n",intSelectedShow);
	sprintf(showsignal3,"%s%d","S",intSelectedShow);
	brsSetShow(IP_PROJ_MD_3,IP_PROJ_MD_PORT,showsignal3);
	intSelectedShow++;
#endif
#endif
#endif
		
//#ifdef FEATURE_TOUCHSCREEN_SPINETIX
//		SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): Resetting GUI\n");
//		guiResetTouchscreen(GUI_SLIDE);
//#endif
		
#ifdef FEATURE_GUI
		SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): Resetting GUI\n");
		guiResetTouchscreen(GUI_SLIDE,BRAND_BRIGHTSIGN);
#endif
		
#ifdef FEATURE_GUI_BUTTONS
		devButtons.setButton(0);
#endif
		
#ifdef SLIDE_MODE_AQUASMASH
		intSlideScore = 0;
		intCtdRedHit = 0;
		intCtdGreenHit = 0;
		intCtdBlueHit = 0;
		intCtdWhiteHit = 0;
		intCtdTotalHit = 0;
		for (int i = 0; i < CTD_SUPPORTED; i++) {
			blnCtdHit[i] = false;
		}
#endif
		
 	} // end STATE_SLIDE_GO
//our timer code for lazers
        
#ifdef FEATURE_CONESENSOR
        
        
        if((scSensors[SENSOR_CONE_IN].state(true))) {
            showStart(EDX_SLIDE_2, scSliders[intSliderIndexLast].show_wm, DMX_SHOW_LAMPS);
#ifdef FEATURE_CUECORE
		SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): SENSOR_CONE_IN / starting CueCore\n");
                setCueCore(scSliders[intSliderIndexLast].show_cuecore);
#endif

	}
        if((scSensors[SENSOR_CONE_OUT].state(true))) {
#ifdef FEATURE_CUECORE
		SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): SENSOR_CONE_OUT / setting CueCore back to IDLE\n");
                setCueCore(CUECORE_OFF);
#endif
                showStop(EDX_SLIDE_2,DMX_SHOW_LAMPS); 
                setLampOff();
	}  
#endif        
        
#ifdef SENSOR_PHOTO
	if((scSensors[SENSOR_PHOTO].state(true))) {
		SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): SENSOR_PHOTO / Taking a photo\n");
		//fltSlideSpeedMS = 0; // ulnSlideTime is 0, so this will not work: (SLIDE_LENGTH / ((float) ulnSlideTime / 1000)); // m/s
		phototime = (unsigned) time(NULL);
		scTriggerPhotoDownload(phototime);
		
	}
#endif

#ifndef SENSOR_IN 
	if (dltController.finished(TIMER_GREEN)) {
		dltController.create(TIMER_GREEN, 0);
		//lightSetRed();		
	}
#endif
	
//----------------------
	// Someone leaves the slide
#ifndef FEATURE_NOMAX
	if ((scSensors[SENSOR_OUT].state(true)) ||
		((scSlide.state == STATE_SLIDE_WAIT) && ((now - scSlide.statetimems) > TIME_SLIDE_MAX))) {
#else
		if ((scSensors[SENSOR_OUT].state(true))) {
		
#endif
//		SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): SENSOR_OUT / TIME_SLIDE_MAX: leave the slide\n")

		for (int i = 1; i <= intSliderIndexLast; i++) { // @@@ Change this, because above message is outputted too much 
			if ((scSliders[i].state == (STATE_SLIDER_LEAVE - 1)) && (now > (scSliders[i].starttimems + TIME_SLIDE_MIN))) {
				scSliders[i].state = STATE_SLIDER_LEAVE;
				scSliders[i].statetimems = now;
				
#ifdef SENSOR_EXIT
				// Another slider must wait
				scSlide.state = STATE_SLIDE_BLOCK;
				scSlide.statetimems = now;
				lightSetBlue();
#else
				// Another slider can enter
				scSlide.state = STATE_SLIDE_PREPARE;
				scSlide.statetimems = now;
				lightSetOrange();
                                showStop(EDX_SLIDE,DMX_SHOW_LAMPS); //green light stop s show because of sensor location
			
#endif	
				
				SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): SENSOR_OUT / Slider %d leaves the slide\n", i);

				ulnSlideTime = now - scSliders[i].starttimems;
				if(ulnSlideTime < TIME_SLIDE_MAX)
				{
					SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): END Sensor Triggered by a slider\n");
				}
				else
				{
					SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): TIME_SLIDE_MAX reached\n");
				}
				if (ulnSlideTime > 0) {
					fltSlideSpeedMS = (SLIDE_LENGTH / ((float) ulnSlideTime / 1000)); // m/s
					fltSlideSpeedKmH = (SLIDE_LENGTH / ((float) ulnSlideTime / 1000)) * 3.6; // km/h
					fltSlideSpeedMph = fltSlideSpeedMS * 2.2369;
#ifdef FEATURE_DISPLAYBOARD
					lngSlideSpeedKmHDisp = (long)(fltSlideSpeedKmH * 100);
					ulnSlideSpeedMphDisp = (long) (fltSlideSpeedMph * 100);
#endif
					intSlideScore = ((int) fltSlideSpeedKmH * SPEED_WEIGHT);
					SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): ulnSlideTime = %lu\n", ulnSlideTime);
					SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): fltSlideSpeedMS = %.2f\n", fltSlideSpeedMS);
					SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): fltSlideSpeedKmH = %.2f\n", fltSlideSpeedKmH);
					SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): intSlideScore = %d\n", intSlideScore);
					if (intSlideScore > intBestScore) {
						intBestScore = intSlideScore;
						SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): New best time\n");
					}
					
					if(ulnSlideTime < ulnBestSlideTime && ulnSlideTime > TIME_SLIDE_MIN) {
						ulnBestSlideTime = ulnSlideTime;
					}
					ulnSlideTimeDisp = (long) (((float)ulnSlideTime / 10));
					ulnBestSlideTimeDisp = (long) (((float)ulnBestSlideTime / 10));
#ifdef SLIDE_MODE_AQUASMASH
					if (scSliders[i].aquasmash) {
						SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): RED %d\n", intCtdRedHit);
						SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): GREEN %d\n", intCtdGreenHit);
						SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): BLUE %d\n", intCtdBlueHit);
						SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): WHITE %d\n", intCtdWhiteHit);
						SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): TOTAL %d\n", intCtdTotalHit);

						intSlideScore += intCtdTotalHit;
						SC_DEBUG("@slidecontroller.cpp/stateStartUpdate(): SCORE %d\n", intSlideScore);

						if (intSlideScore > intBestScore) {
							intBestScore = intSlideScore;
							SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): New best time\n");
						}
						asStop();
					}
                                        scSliders[i].aquasmash = false; // set to false to prevent trigger in IDLE
#endif
#ifdef FEATURE_DISPLAYBOARD
	number_to_display(SC_DISPLAY_TIME, ulnSlideTimeDisp, SC_DISPLAY_TIME_L, SC_DISPLAY_TIME_D, true);
	
	number_to_display(SC_DISPLAY_SPEED, lngSlideSpeedKmHDisp, SC_DISPLAY_SPEED_L, SC_DISPLAY_SPEED_D, true);
	
	number_to_display(SC_DISPLAY_BEST, ulnBestSlideTimeDisp, SC_DISPLAY_BEST_L, SC_DISPLAY_BEST_D, true);
#endif		
#ifdef SLIDE_MODE_WHITEMAGIC
#ifdef SLIDE_MODE_AQUASMASH
					showStop(EDX_SLIDE,DMX_SHOW_LAMPS+CTD_SUPPORTED);
#else
					showStop(EDX_SLIDE,DMX_SHOW_LAMPS); //green light stop s show because of sensor location
                                        //setLampOff();
#endif
#endif					
				}
#ifdef FEATURE_ECUE
	//Rings off
	LedRingSetStandby();
#endif
#ifdef FEATURE_PROJECTION		
#if SOURCEBRAND == BRAND_SPINETIX
		prjSetShow(PRJ_SLIDE, 0);
#endif
#if SOURCEBRAND == BRAND_BRIGHTSIGN
#ifdef IP_PROJ_MD_1
	//brsSetShow(IP_PROJ_MD_1,IP_PROJ_MD_PORT,reinterpret_cast<char*>(scSliders[intSliderIndexLast].show_pr));
	brsSetShow(IP_PROJ_MD_1,IP_PROJ_MD_PORT,(char*)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_2
	brsSetShow(IP_PROJ_MD_2,IP_PROJ_MD_PORT,(char*)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_3
	brsSetShow(IP_PROJ_MD_3,IP_PROJ_MD_PORT,(char*)IP_PROJ_MD_STATE_IDLE);
#endif
#endif
#endif
			
#ifdef FEATURE_MP3
				soundStart(MPD_SLIDE, 0);
#endif
#ifdef FEATURE_LASER
				laserStart(0,1);
                                laserStart(0,2);
#endif
#ifdef FEATURE_MOVINGHEAD
				movingheadStart(0);
#endif
#ifdef FEATURE_CUECORE
#ifdef FEATURE_CUECORE
                setCueCore(CUECORE_OFF);
#endif            
#endif

				// Do this LAST:
				unsigned int now = (unsigned) time(NULL);
#ifdef SENSOR_IN
				if (ulnSlideTime > 0) {
#else
				if (ulnSlideTime > 0 && ulnSlideTime < TIME_SLIDE_MAX) {
//					SC_DEBUG("scSliders[%d].photo = %d\n",i,scSliders[i].photo);
#endif
#ifdef SLIDE_MODE_AQUASMASH
					scProcessSliderDataThread(now, scSliders[i].show, ulnSlideTime, fltSlideSpeedMS, intCtdRedHit, intCtdGreenHit, intCtdBlueHit, intCtdWhiteHit, intCtdTotalHit, intSlideScore, scSliders[i].photo,scSliders[i].rfid);
#else
					scProcessSliderDataThread(now, scSliders[i].show, ulnSlideTime, fltSlideSpeedMS, 0, 0, 0, 0, 0, intSlideScore, scSliders[i].photo,scSliders[i].rfid);
#endif
				}
				
#ifdef FEATURE_SCORESCREEN
	SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): Sending trigger to scorescreen\n");
	triggerScoreScreen();			
#endif
#ifdef FEATURE_HOLD_ONEXIT
				sleep(HOLD_TIME_SEC);
				//lightSetOrange();
		
#endif
	
				break;
			} else {
				scSensors[SENSOR_OUT].state(false); // unblock
			}
		}
	}
	
#ifdef SENSOR_EXIT
	if ((scSlide.state == STATE_SLIDE_BLOCK) && (scSensors[SENSOR_EXIT].state(false))) {
		SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): SENSOR_EXIT on slide\n");
		// Another slider can enter
		scSlide.state = STATE_SLIDE_PREPARE;
		scSlide.statetimems = now;
		//lightSetOrange();
	}
#endif

	// TIME_SLIDE_EXIT running out
	if ((scSlide.state == STATE_SLIDE_PREPARE) && ((now - scSlide.statetimems) > TIME_SLIDE_EXIT)) {

		SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): TIME_SLIDE_EXIT on slide\n");
		scSlide.state = STATE_SLIDE_GO;
#ifdef SENSOR_IN
		scSensors[SENSOR_IN].state(false); // Remove block from sensor
#endif                
		lightSetGreen();
		showStop(EDX_SLIDE,DMX_SHOW_LAMPS);
	}


/*
	// TIME_SLIDE_MAX running out
	if ((scSlide.state == STATE_SLIDE_WAIT) && ((now - scSlide.statetimems) > TIME_SLIDE_MAX)) {
		SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): TIME_SLIDE_MAX on slide\n");

		// Shift the sliders array
		for (int i = 1; i <= intSliderIndexLast; i++) {
			scSliders[i-1] = scSliders[i];
		}
		intSliderIndexLast--;
		
		scSlide.state = STATE_SLIDE_PREPARE;
		scSlide.statetimems = now;
		lightSetOrange();
	}
*/
	
	
#ifdef SLIDE_MODE_AQUASMASH	
	int ctd;
	int color;

	if (scSliders[intSliderIndexLast].aquasmash) {
		for (ctd = 1; ctd <= CTD_COUNT; ctd++) {
			color = nCtdColorSets[nCtdColorSetSelected][ctd - 1];
			if (scSensors[SENSOR_OFFSET_CTD + (ctd - 1)].state(false)) {
				// @@@ flash on each touch only for demo
//				asFlashCtd(ctd);

				if (!blnCtdHit[ctd - 1]) {
					blnCtdHit[ctd - 1] = true;
					asFlashCtd(ctd);
					SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): DOT %d\n", ctd);
//					soundStart(MPD_SLIDE, scSliders[intSliderIndexLast].show_hit);
					switch (color) {
						case CTD_COLOR_RED:
							intCtdRedHit++;
							break;
						case CTD_COLOR_GRN:
							intCtdGreenHit++;
							break;
						case CTD_COLOR_BLU:
							intCtdBlueHit++;
							break;
						case CTD_COLOR_WHT:
							intCtdWhiteHit++;
							break;
					}
					intCtdTotalHit += intCtdColorValues[color];
					// @@@ Remove debug below later
//					printf("DOT %d %d %d\n", ctd, nCtdColorSets[nCtdColorSetSelected][ctd - 1], intCtdColorValues[nCtdColorSets[nCtdColorSetSelected][ctd - 1]]);
				}
			}
		}
	}

#endif
	
	if (dltController.finished(TIMER_START_REFRESH)) {
		// Clean up photos
		systemCleanup();
		dltController.create(TIMER_START_REFRESH, TIME_START_REFRESH);
	}	
}
//-----------------------------------------------------------------------------------------

void stateStartExit() {
}





void statePanicEnter() {
	SC_DEBUG("\n");
	SC_DEBUG("@slidecontroller.cpp/statePanicEnter(): STATE PANIC\n");
	systemSetSecure();
#ifdef FEATURE_PHOTO
	blnTakePhoto = false;
#endif
#ifdef FEATURE_TIME
	ulnSlideTime = 0;
#endif
	dltController.create(TIMER_PANIC_REFRESH, TIME_PANIC_REFRESH);
}

void statePanicUpdate() {
//	if (scSensors[SENSOR_RESET].state(false)) {
//		fsmController.transitionTo(steInit);
//	}
	if (!scSensors[SENSOR_PANIC].state(false)) {
		fsmController.transitionTo(steInit);
	}
	if (dltController.finished(TIMER_PANIC_REFRESH)) {
		systemSetSecure();
		dltController.create(TIMER_PANIC_REFRESH, TIME_PANIC_REFRESH);
	}
#ifdef FEATURE_EMERGENCY_BUTTON
//	if(scSensors[SENSOR_EMERGENCY].state(true)) {
//		fsmController.transitionTo(steEmergency);
//	}
	if(!getEmergencyInput(SENSOR_EMERGENCY))
	{
		fsmController.transitionTo(steEmergency);
	}
#endif
}

void statePanicExit() {
	systemSetNormal();
}

void intTimer1() {
	intIntTimer++;
}

void intTimer3() {
}

#ifdef FEATURE_EMERGENCY_BUTTON
void stateEmergencyEnter() {
	SC_DEBUG("\n");
	SC_DEBUG("@slidecontroller.cpp/stateEmergency(): STATE EMERGENCY\n");
	systemSetEmergency();
	dltController.create(TIMER_EMERGENCY_REFRESH, TIME_EMERGENCY_REFRESH);
}
		
void stateEmergencyUpdate() {
//	if (scSensors[SENSOR_EMERGENCY].state(false)) {
//		fsmController.transitionTo(steInit);
//	}
	if(getEmergencyInput(SENSOR_EMERGENCY ) && dltController.finished(TIMER_EMERGENCY_REFRESH))
	{
		blnFirstRun = true;
		fsmController.transitionTo(steInit);
	}
	if (dltController.finished(TIMER_EMERGENCY_REFRESH) && !getEmergencyInput(SENSOR_EMERGENCY ))
	{
		systemSetEmergency();
		dltController.create(TIMER_EMERGENCY_REFRESH, TIME_EMERGENCY_REFRESH);
	}
}
void stateEmergencyExit() {
	systemClearEmergency();
}
#endif

#ifdef FEATURE_BUTTON_SCORERESET
bool slideScoreReset() {
        SC_HIGHLIGHT("@slidecontroller.cpp/stateStartUpdate(): Resetting SCORE\n");
	number_to_display(SC_DISPLAY_SMASH, 0, SC_DISPLAY_SMASH_L, SC_DISPLAY_SMASH_D, true);
	number_to_display(SC_DISPLAY_SCORE, 0, SC_DISPLAY_SCORE_L, SC_DISPLAY_SCORE_D, true);
	number_to_display(SC_DISPLAY_SPEED, 0, SC_DISPLAY_SPEED_L, SC_DISPLAY_SPEED_D, true);
	number_to_display(SC_DISPLAY_BEST, 0, SC_DISPLAY_BEST_L, SC_DISPLAY_BEST_D, true);
	intSlideScore = 0;
	intCtdRedHit = 0;
	intCtdGreenHit = 0;
	intCtdBlueHit = 0;
	intCtdWhiteHit = 0;
	intCtdTotalHit = 0;
	ulnSlideTime = 0;
	fltSlideSpeedMS = 0;
	fltSlideSpeedKmH = 0;
	fltSlideSpeedMph = 0;
	ulnSlideTimeDispOld = 0;
	ulnSlideTimeDisp = 0;
	lngSlideSpeedKmHDisp = 0;
	ulnSlideSpeedMphDisp = 0;
	ulnBestSlideTime = TIME_SLIDE_MAX;
	ulnBestSlideTimeDisp = 0;
}
#endif