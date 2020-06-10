/*
 * SlideController Abstraction Layer
 */
// *************** INCLUDES ***************
#include "defines.h"
#include "scal.h"
#include "slidecontroller.h" // for class scSlider
#include <cstdlib>
#include <sys/stat.h> // for struct stat in scProcessSliderData()

#ifdef FEATURE_GUI
#include "gui.h"
#endif
#ifdef FEATURE_MP3
#include "soundbox.h"
#endif
#ifdef FEATURE_COUNTDOWNLIGHT
#include "countdownlight.h"
#endif
#ifdef FEATURE_SOLLAE
#include "remoteio.h"
#endif
#include "scorescreen.h"
#include "projection.h"
#ifdef FEATURE_ETHDMX
#include "ethdmx.h"
#endif
#ifdef FEATURE_ETHDMXIN
#include "ethdmxin.h"
#endif
#ifdef FEATURE_ETHDMXOUT
#include "ethdmxout.h"
#endif
#ifdef FEATURE_PHOTO
#include "web.h"
#include "camera.h"
#endif
#ifdef FEATURE_ECUE
#include "ecue.h"
#endif
#ifdef FEATURE_CUECORE
#include "cuecore.h"
#endif

// *************** GLOBAL VARIABLES ***************
#ifndef STANDALONE
extern int intShowSelected; // used in scSetShow()
#endif
bool blnDebugMode = false;
int intLastCueCoreRandom = 0;
#ifdef FEATURE_MP3_RANDOMSET
int intRandomSelectionOld = 0;
#endif

// *************** CLASS FUNCTIONS ***************
scSensor::scSensor(void) {	
}

bool scSensor::init(int intType, char const * chrIP, int intChannel, int intId, bool blnInverted) {
	bool ret;

//	SC_HIGHLIGHT("@scal.cpp/scSensor::init(%d, %s, %d, %d, %d): Starting...\n", intType, chrIP, intChannel, intId, blnInverted);
	
	this->intType = intType;
	this->intId = intId;
	this->blnInverted = blnInverted;
	
	SC_HIGHLIGHT("@scal.cpp/scSensor::init(%d, %s, %d, %d, %d): Starting...\n", this->intType, chrIP, intChannel, this->intId, this->blnInverted);

	if (this->intType == IO_TYPE_SOLLAE) {
		ret = rioInit(this->intType, this->intId, chrIP, intChannel, blnDebugMode); 
	} else if (this->intType == IO_TYPE_EDX) {
		ret = ediInit(this->intId, chrIP, intChannel, blnInverted);
	} 
#ifdef FEATURE_CAMERASENSOR
	else if (this->intType == IO_TYPE_CAMERA) {
		ret = camSensorInit((char*)chrIP,intChannel);
	}
#endif
	else {
		// Unknown intType
		SC_ERROR("@scal.cpp/scSensor::init(...): Unknown intType\n");
		return false;
	}
	
	return ret;
}

bool scSensor::state(bool blnBlock) {
	if (!blnBlock) this->blnBlocked = false;
	if (this->intType == IO_TYPE_SOLLAE) {
		if ((rioGetInput(this->intId)) == (!this->blnInverted)) {
			if (!this->blnBlocked) {
				if (blnBlock) this->blnBlocked = true;
				return true;
			}
		}
	} else if (this->intType == IO_TYPE_EDX) {
		if ((ediGetInput(this->intId)) == (!this->blnInverted)) {
			if (!this->blnBlocked) {
				if (blnBlock) this->blnBlocked = true;
				return true;
			}
		}
	} 
#ifdef FEATURE_CAMERASENSOR
	else if (this->intType == IO_TYPE_CAMERA) {
		if ((camSensorGetInput(this->intId)) == (!this->blnInverted)) {
			if(!this->blnBlocked) {
				if(blnBlock) this->blnBlocked = true;
				return true;
			}
		}
	}
#endif

	if (this->intId == 0) {
		SC_ERROR("scal.cpp/scSensor::state(...): intId == 0 ???\n");
	}
#ifdef FEATURE_GUI
	int cmd = guiGetCmd();
//	printf("@inputGetIn %d\n", cmd);
	if (cmd == this->intId) {
		guiResetCmd();
		printf("We got %d\n", this->intId);
		return true;
	}
#endif

	return false;
}


// *************** FUNCTIONS ***************
bool scalInit() {
#ifdef FEATURE_GUI
	guiInit(GUI_SLIDE, IP_TS);
#endif
#ifdef FEATURE_COUNTDOWNLIGHT
	cdlInit(CDL_SLIDE, IP_CDL);
#endif
#ifdef FEATURE_GPIO	
	// Init gpio
	gpioInit();
	systemSetSecure();
#endif
#ifdef FEATURE_DMX	
	dmxshowInit(RESOLUTIONMS_DMX, DMX_START, DMX_MAXCHANNEL);
#endif
#ifdef FEATURE_MP3
	sndInit(MPD_SLIDE, IP_SB_1, true, false, MPD_VOLUME);
	soundStart(MPD_SLIDE,1);
#endif
#ifdef FEATURE_ETHDMX
	edxInit(EDX_SLIDE, IP_ETHDMX_1);
#endif
#ifdef FEATURE_ETHDMXIN
//#ifdef SENSOR_IN
//	ediInit(SENSOR_IN, IP_ETHDMX_1, SENSOR_IN,false); // @@@
//#endif
//	ediInit(SENSOR_OUT, IP_ETHDMX_1, SENSOR_OUT,false); // @@@
//	ediInit(SENSOR_PANIC, IP_ETHDMX_1, SENSOR_PANIC,true);
//#ifdef FEATURE_EMERGENCY_BUTTON
//	ediInit(SENSOR_EMERGENCY, IP_ETHDMX_1, SENSOR_EMERGENCY, false);
//#endif
	//ediInit(RIO_SENSOR_5, IP_ETHDMX_4, 1);
	//ediInit(RIO_SENSOR_8, IP_ETHDMX_5, 1);
#endif
#ifdef FEATURE_ETHDMXOUT
	edoInit(RIO_RELAY_1, IP_ETHDMX_5, 1);
#endif

#ifdef FEATURE_SCORESCREEN
	scrInit(SCR_SLIDE, IP_SCREEN);
#endif
#ifdef FEATURE_PHOTO
	camInit(CAM_SLIDE, IP_CAM);
	webInit(WEB_SLIDE, URL_API_SSL, UUID, false); // no debug
#endif
#ifdef FEATURE_ECUE
	ecuInit(ECUE_DEVICE,ECUE_IP);
#endif
	return true;
}


bool systemSetSecure() {
#ifdef FEATURE_GPIO	
	gpioSetLedError(HIGH);
#endif
#ifdef FEATURE_LASER
        laserStart(0,1);
        laserStart(0,2);
#endif
#ifdef FEATURE_CUECORE
        setCueCore(CUECORE_OFF);
#endif
	lightSetRed();
#ifdef SLIDE_MODE_AQUASMASH
//	asStop();
#endif
#ifdef SLIDE_MODE_WHITEMAGIC   
//	wmStopShow();
#endif
#ifdef FEATURE_DMX
	dmxshowOff();
#endif
	
#ifdef FEATURE_PROJECTION

#if SOURCEBRAND == BRAND_SPINETIX
	prjSetPower(false); // @@@
#endif
#if SOURCEBRAND == BRAND_BRIGHTSIGN
	// all brightsigns to IDLE to be sure
#ifdef IP_PROJ_MD_1
	brsSetShow(IP_PROJ_MD_1,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_2
	brsSetShow(IP_PROJ_MD_2,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_3
	brsSetShow(IP_PROJ_MD_3,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_4
	brsSetShow(IP_PROJ_MD_4,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_5
	brsSetShow(IP_PROJ_MD_5,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_6
	brsSetShow(IP_PROJ_MD_6,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_7
	brsSetShow(IP_PROJ_MD_7,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#endif
	// all beamers off
#ifdef IP_PROJ_BEAMER_1
	pjlSetPower(IP_PROJ_BEAMER_1,false);
#endif
#ifdef IP_PROJ_BEAMER_2
	pjlSetPower(IP_PROJ_BEAMER_2,false);
#endif
#ifdef IP_PROJ_BEAMER_3
	pjlSetPower(IP_PROJ_BEAMER_3,false);
#endif
#ifdef IP_PROJ_BEAMER_4
	pjlSetPower(IP_PROJ_BEAMER_4,false);
#endif
#ifdef IP_PROJ_BEAMER_5
	pjlSetPower(IP_PROJ_BEAMER_5,false);
#endif
#ifdef IP_PROJ_BEAMER_6
	pjlSetPower(IP_PROJ_BEAMER_6,false);
#endif
#ifdef IP_PROJ_BEAMER_7
	pjlSetPower(IP_PROJ_BEAMER_7,false);
#endif
#ifdef IP_PROJ_BEAMER_8
	pjlSetPower(IP_PROJ_BEAMER_8,false);
#endif	
	
#endif
#ifdef FEATURE_SCORESCREEN
	scrSetDisplay(SCR_SLIDE, false,BRAND_BRIGHTSIGN);
#endif
#ifdef FEATURE_TOUCHSCREEN
	guiSetScreenActive(GUI_SLIDE, false,BRAND_BRIGHTSIGN);	
#else
	// Touchscreen disabled
	FILE * pFile;
	char filename[100];

	sprintf(filename, "%s/slide_disabled", DIR_TMP);

	pFile = fopen(filename, "w");
	fprintf(pFile, "This is a dummy file to disable GUI\n");
	fclose(pFile);
#endif

	edxSetLamp(EDX_SLIDE, 0, DMX_VALUE_OFF, DMX_VALUE_OFF, DMX_VALUE_OFF, 1000);
#ifdef FEATURE_ECUE
	LedRingSetShow(SHOW_OFF);
#endif
	return true;
}


bool systemSetNormal() {
#ifdef FEATURE_LASER
        laserStart(0,1);
        laserStart(0,2);
#endif
#ifdef FEATURE_CUECORE
        setCueCore(CUECORE_IDLE);
#endif
#ifdef FEATURE_GPIO	
	gpioSetLedError(LOW);
#endif
        showStop(EDX_SLIDE,DMX_SHOW_LAMPS);
	lightSetRed();
#ifdef FEATURE_SCORESCREEN
	scrSetDisplay(SCR_SLIDE, true, BRAND_BRIGHTSIGN);
#endif
#ifdef FEATURE_PROJECTION
	
#if SOURCEBRAND == BRAND_SPINETIX
	prjSetPower(true);
#endif
#if SOURCEBRAND == BRAND_BRIGHTSIGN
#ifdef IP_PROJ_MD_1
	brsSetShow(IP_PROJ_MD_1,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_2
	brsSetShow(IP_PROJ_MD_2,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_3
	brsSetShow(IP_PROJ_MD_3,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_4
	brsSetShow(IP_PROJ_MD_4,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_5
	brsSetShow(IP_PROJ_MD_5,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_6
	brsSetShow(IP_PROJ_MD_6,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#ifdef IP_PROJ_MD_7
	brsSetShow(IP_PROJ_MD_7,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_IDLE);
#endif
#endif
	
#ifdef FEATURE_PJLINK
// all beamers on
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
#ifdef IP_PROJ_BEAMER_6
	pjlSetPower(IP_PROJ_BEAMER_6,true);
#endif
#ifdef IP_PROJ_BEAMER_7
	pjlSetPower(IP_PROJ_BEAMER_7,true);
#endif
#ifdef IP_PROJ_BEAMER_8
	pjlSetPower(IP_PROJ_BEAMER_8,true);
#endif
#endif
#endif
#ifdef FEATURE_ECUE
	ecuSetStandby(ECUE_DEVICE,SLIDEID);
#endif	
	return true;
}


bool lightSetRed() {
#ifdef FEATURE_GPIO	
	gpioSetLedState(LOW);
	gpioSetLightRed();
#else
//	rioSetOutput(RIO_1_LIGHT, 0);
#endif
#ifdef FEATURE_DMX
#ifdef DMX_BASE_TRAFFICLIGHT
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_RED, DMX_VALUE_FULL, 0);
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_GREEN, DMX_VALUE_OFF, 0);
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_BLUE, DMX_VALUE_OFF, 0);
#endif
#endif
#ifdef FEATURE_COUNTDOWNLIGHT
	cdlSetRed(CDL_SLIDE);
#endif

	return true;
}


bool lightSetGreen() {
#ifdef FEATURE_GPIO	
	gpioSetLedState(HIGH);
	gpioSetLightGreen();
#else
//	rioSetOutput(RIO_1_LIGHT, 1);
#endif
#ifdef FEATURE_DMX
#ifdef DMX_BASE_TRAFFICLIGHT
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_RED, DMX_VALUE_OFF, 0);
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_GREEN, DMX_VALUE_FULL, 0);
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_BLUE, DMX_VALUE_OFF, 0);
#endif
#endif
#ifdef FEATURE_COUNTDOWNLIGHT
	cdlSetGreen(CDL_SLIDE);
//	cdlCountDown(5);
#endif

	return true;
}


bool lightSetOrange() {
#ifdef FEATURE_GPIO	
	gpioSetLedState(LOW);
	gpioSetLightRed();
#else
//	rioSetOutput(RIO_1_LIGHT, 0);	
#endif
#ifdef FEATURE_DMX
#ifdef DMX_BASE_TRAFFICLIGHT
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_RED, DMX_VALUE_FULL, 0); // TODO Or create this in dmxshow.cpp
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_GREEN, DMX_VALUE_HALF, 0);
	dmxshowDirect(DMX_BASE_TRAFFICLIGHT + DMX_OFFSET_BLUE, DMX_VALUE_OFF, 0);
#endif
#endif
#ifdef FEATURE_COUNTDOWNLIGHT
	cdlCountDown(CDL_SLIDE, TIME_SLIDE_EXIT / 1000);
#endif
	
	return true;
}

bool lightSetBlue() {
#ifdef FEATURE_COUNTDOWNLIGHT
	cdlSendCommand(CDL_SLIDE, 3, 0); // Actually it is cyan
#endif
	
	return true;
}

bool lightSetCross() {
#ifdef FEATURE_COUNTDOWNLIGHT
	cdlSetCross(CDL_SLIDE);
#endif
	return true;
}


#ifdef SLIDE_MODE_AQUASMASH

bool asLoadCtd(unsigned char nCtdColorSet[CTD_SUPPORTED]) {
	int fadetime = 500;
	for (int ctd = 1; ctd <= CTD_SUPPORTED; ctd++) {
		switch (nCtdColorSet[ctd - 1]) {
			case CTD_COLOR_RED:
				edxSetLamp(EDX_SLIDE, DMX_SHOW_LAMPS+ctd, DMX_VALUE_FULL, DMX_VALUE_OFF, DMX_VALUE_OFF, fadetime);
				break;
			case CTD_COLOR_GRN:
				edxSetLamp(EDX_SLIDE, DMX_SHOW_LAMPS+ctd, DMX_VALUE_OFF, DMX_VALUE_FULL, DMX_VALUE_OFF, fadetime);
				break;
			case CTD_COLOR_BLU:
				edxSetLamp(EDX_SLIDE, DMX_SHOW_LAMPS+ctd, DMX_VALUE_OFF, DMX_VALUE_OFF, DMX_VALUE_FULL, fadetime);
				break;
			case CTD_COLOR_WHT:
				edxSetLamp(EDX_SLIDE, DMX_SHOW_LAMPS+ctd, DMX_VALUE_FULL, DMX_VALUE_FULL, DMX_VALUE_FULL, fadetime);
				break;
		}
	}

	return true;
}


bool asFlashCtd(int ctd) {
	edxStartFlashOff(EDX_SLIDE, DMX_SHOW_LAMPS+ctd, 9, 50);

	return true;
}


bool asStop() {
	edxStartShow(EDX_SLIDE, 0, 100, DMX_SHOW_LAMPS+CTD_SUPPORTED);
	
	return true;
}
#endif

#ifdef FEATURE_SCORESCREEN
void triggerScoreScreen()
{
	scrTrigger(SCR_SLIDE,BRAND_BRIGHTSIGN); // update spinetix with or without photo
	scrTrigger(SCR_SLIDE,BRAND_BRIGHTSIGN); // update spinetix with or without photo
	scrTrigger(SCR_SLIDE,BRAND_BRIGHTSIGN); // update spinetix with or without photo
	// UDP packages can get lost, so fire them more often
}
#endif

//void scTriggerPhotoDownload(unsigned int ts) {
//	std::thread hThreadTriggerPhotoDownload(scTriggerPhotoDownloadThread, ts);
//	pthread_setname_np(hThreadTriggerPhotoDownload.native_handle(), "TriggerPhotoDownloadThread");
//	hThreadTriggerPhotoDownload.detach();
//}
//
//void scTriggerPhotoDownloadThread(unsigned int ts) {
//#ifdef FEATURE_PHOTO
//	int i = 1;
//#ifdef TIME_PHOTODELAY				
//	usleep(TIME_PHOTODELAY * 1000);
//#endif
//	bool success_photo = camGetPhoto(CAM_SLIDE, ts);
//	if (success_photo) {
//		SC_DEBUG("@scal.cpp/scProcessSliderData(): camGetPhoto done\n");		
//	} // else debug message in library
//
//	// photo should exist now, but double check:
//	char filename[100];
//	struct stat buffer;
//	sprintf(filename, "%sphoto-%u.jpg", DIR_PHOTO, ts);
//	// Be sure this photo has content. Check if the axis page is not password protected: 
//	// "Enable anonymous viewer login (no user name or password required)"
//	if ((stat(filename, &buffer) != 0) || (buffer.st_size < 200000)){ // file does not exists or smaller than 200kb
//		usleep(1000 * 1000); // 1 s
//		while (( (stat(filename, &buffer) != 0) || (buffer.st_size < 200000) ) && i<=PHOTO_RETRIES) { // after 2nd try output debug info
//			SC_DEBUG("@scal.cpp/scProcessSliderDataThread(): Waiting for file %s to exist...\n", filename);
//			usleep(700 * 1000); // 700 ms
//			if(i>=PHOTO_RETRIES){
//				SC_ERROR("@scal.cpp/scProcessSliderDataThread(): file %s not found after %u tries, stopping...\n", filename, i);
//			}
//			i++;
//		}		
//	}
//#endif
//}

void scProcessSliderDataThread(unsigned int ts, unsigned int show, unsigned long duration_ms, float speed_ms, unsigned int red, unsigned int green, unsigned int blue, unsigned int white, unsigned int total, unsigned int score, bool photo) {
    char* rfid = "0";
    std::thread(scProcessSliderData, ts, show, duration_ms, speed_ms, red, green, blue, white, total, score, photo,rfid).detach();
}

void scProcessSliderDataThread(unsigned int ts, unsigned int show, unsigned long duration_ms, float speed_ms, unsigned int red, unsigned int green, unsigned int blue, unsigned int white, unsigned int total, unsigned int score, bool photo, char* rfid) {
	std::thread(scProcessSliderData, ts, show, duration_ms, speed_ms, red, green, blue, white, total, score, photo, rfid).detach();
}

void scProcessSliderData(unsigned int ts, unsigned int show, unsigned long duration_ms, float speed_ms, unsigned int red, unsigned int green, unsigned int blue, unsigned int white, unsigned int total, unsigned int score, bool photo,char* rfid) {

	float speed_kmh = speed_ms * 3.6;
#ifdef FEATURE_PHOTO
	int i = 1;
#endif
	// save to MySQL database, debug message in scrSaveData()
#ifdef SLIDE_MODE_AQUASMASH
	scrSaveData(ts, show, duration_ms, speed_kmh, red, green, blue, white, total, score, photo, rfid);
#else
	scrSaveData(ts, show, duration_ms, speed_kmh, 0, 0, 0, 0, 0, score, photo, rfid);
#endif

#ifdef TIME_PHOTODELAY				
	usleep(TIME_PHOTODELAY * 1000);
#endif
#ifdef FEATURE_PHOTO
	if(photo){ // make photo
		bool success_photo = camGetPhoto(CAM_SLIDE, ts);
		if (success_photo) {
			SC_DEBUG("@scal.cpp/scProcessSliderData(..., %d): camGetPhoto done\n", rfid);		
		} // else debug message in library
		
		// photo should exist now, but double check:
		char filename[100];
		struct stat buffer;
		sprintf(filename, "%sphoto-%u.jpg", DIR_PHOTO, ts);
		// Be sure this photo has content. Check if the axis page is not password protected: 
		// "Enable anonymous viewer login (no user name or password required)"
		if ((stat(filename, &buffer) != 0) || (buffer.st_size < 100000)){ // file does not exists or smaller than 200kb
			usleep(1000 * 1000); // 1 s
			while (( (stat(filename, &buffer) != 0) || (buffer.st_size < 100000) ) && i<=PHOTO_RETRIES) { // after 2nd try output debug info
				SC_DEBUG("@scal.cpp/scProcessSliderDataThread(): Waiting for file %s to exist...\n", filename);
				usleep(700 * 1000); // 700 ms
				if(i>=PHOTO_RETRIES){
					SC_ERROR("@scal.cpp/scProcessSliderDataThread(): file %s not found after %u tries, stopping...\n", filename, i);
				}
				i++;
			}		
		}
	}
#endif


	
#ifdef FEATURE_UPLOAD
	if(photo && i<=PHOTO_RETRIES ){ // upload photo
		bool success_upload = webSendPhoto(WEB_SLIDE, ts, duration_ms, (int) speed_ms, red, green, blue, white, total, score,rfid);
		if (!success_upload) {
			SC_ERROR("@scal.cpp/scProcessSliderData(): webSendPhoto failed!\n");		
		} // else debug message in library
	} // if(photo)
		
#endif // FEATURE_UPLOAD
}

/*
bool asSaveScore(unsigned int ts, unsigned int show, unsigned long duration, float speed, unsigned int red, unsigned int green, unsigned int blue, unsigned int white, unsigned int total, unsigned int score, bool photo) {

	scrSaveData(SCR_SLIDE, ts, show, duration, speed, red, green, blue, white, total, score, photo);

	
//	FILE * pFile;
//	char filename[100];
	
//	sprintf(filename, "%s/photo-%u.txt", DIR_PHOTO, ts);

//	pFile = fopen(filename, "w");
//	fprintf(pFile, "duration = %.1lu\n", duration / 1000);
//	fprintf(pFile, "speed = %u\n", speed);
//	fprintf(pFile, "red = %u\n", red);
//	fprintf(pFile, "green = %u\n", green);
//	fprintf(pFile, "blue = %u\n", blue);
//	fprintf(pFile, "white = %u\n", white);
//	fprintf(pFile, "total = %u\n", total);
//	fprintf(pFile, "score = %u\n", score);
//	fclose(pFile);
 
	return true;
}
*/

bool systemCleanup() {
	bool ret = true;
	
	
#ifdef FEATURE_SCORESCREEN
	ret = scrCleanup(SCR_SLIDE);
	scrTrigger(SCR_SLIDE,BRAND_BRIGHTSIGN); // update spinetix
#else
	//scrCleanup(SCR_SLIDE);
#endif
	
	return ret;
}


#ifdef SLIDE_MODE_WHITEMAGIC
bool wmStartShow(int intShowSelected) {
	edxStartShow(EDX_SLIDE, intShowSelected, 0, 0);
	return true;
}


bool wmStopShow() {
	edxStartShow(EDX_SLIDE, 0, 0, 0);	// dmxshowStop();
	return true;
}
#endif

bool soundStart(unsigned char playerid, int soundid) {
    	#ifdef FEATURE_MP3
	bool result = false;
	if (soundid > 0) {
		result = sndPlay(playerid, soundid);
	} else {
		result = sndStop(playerid);
	}
	return result;
#else
	return false;
#endif
}


bool scSetShow(scSlider * scSliderIn) {
	int intShowSelectedWM;
	int intShowSelectedSO;
	int intShowSelectedPR;
	int intShowSelectedSOhit = 0;
	bool blnShowSelectedAS = true;
	int intShowSelectedLS;
	int intShowSelectedMH;
#ifdef FEATURE_CUECORE
        char* charShowSelectedCueCore = "0";
#endif
#ifdef FEATURE_PHOTO
	bool blnTakePhoto = false;
#endif
#ifdef TEST_ALWAYS_PHOTO
	blnTakePhoto = true;
#endif
	
	
#define MOVINGHEAD_RAINBOWMOVING 1
#define MOVINGHEAD_GREENPANNING 2
#define MOVINGHEAD_RGBWTILTING 3
#define MOVINGHEAD_DISCOMOVING 4

	

	SC_HIGHLIGHT("@scal.cpp/scSetShow(): scSliderIn->show = %d\n", scSliderIn->show);
	switch (scSliderIn->show) {
//              with photo
		case 11: // Jungle
			intShowSelectedWM = SHOW_GREENFADE;
			intShowSelectedPR = intShowSelected;
			intShowSelectedSO = SOUND_OFFSET + SOUND_1_2; //1
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_GREENPANNING;
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "1";
#endif
#ifdef FEATURE_PHOTO
			blnTakePhoto = true;
#endif
			break;
		case 21: // Disco
			intShowSelectedWM = SHOW_RAINBOW;
			intShowSelectedPR = intShowSelected;
			intShowSelectedSO = SOUND_OFFSET + SOUND_1_3; //2
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_DISCOMOVING;
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "2";
#endif
#ifdef FEATURE_PHOTO
			blnTakePhoto = true;
#endif
			break;
		case 31: // Kids
			intShowSelectedWM = SHOW_PINKFADE;
			intShowSelectedPR = intShowSelected;
			intShowSelectedSO = SOUND_OFFSET + SOUND_1_4; //3
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_RAINBOWMOVING;
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "3";
#endif                        
#ifdef FEATURE_PHOTO
			blnTakePhoto = true;
#endif
			break;
		case 41: // Movie
			intShowSelectedWM = SHOW_REDFADE;
			intShowSelectedPR = intShowSelected;
			intShowSelectedSO = SOUND_OFFSET + SOUND_2_2; //4
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_RGBWTILTING;
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "4";
#endif                        
#ifdef FEATURE_PHOTO
			blnTakePhoto = true;
#endif
			break;
//              without photo
		case 12:
			intShowSelectedWM = SHOW_GREENFADE;
			intShowSelectedPR = intShowSelected;
			intShowSelectedSO = SOUND_OFFSET + SOUND_1_2; //1
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_GREENPANNING;
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "1";
#endif                        
#ifdef FEATURE_PHOTO
			blnTakePhoto = false;
#endif
			break;
		case 22:
			intShowSelectedWM = SHOW_RAINBOW;
			intShowSelectedPR = intShowSelected;
			intShowSelectedSO = SOUND_OFFSET + SOUND_1_3; //2
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_DISCOMOVING;
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "2";
#endif                        
#ifdef FEATURE_PHOTO
			blnTakePhoto = false;
#endif
			break;
		case 32:
			intShowSelectedWM = SHOW_PINKFADE;
			intShowSelectedPR = intShowSelected;
			intShowSelectedSO = SOUND_OFFSET + SOUND_1_4; //3
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_RAINBOWMOVING;
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "3";
#endif                        
#ifdef FEATURE_PHOTO
			blnTakePhoto = false;
#endif
			break;
		case 42:
			intShowSelectedWM = SHOW_REDFADE;
			intShowSelectedPR = intShowSelected;
			intShowSelectedSO = SOUND_OFFSET + SOUND_2_2; //4
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_RGBWTILTING;
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "4";
#endif                        
#ifdef FEATURE_PHOTO
			blnTakePhoto = false;
#endif
			break;		
		default:
			intShowSelectedWM = SHOW_RAINBOW;
			intShowSelectedPR = 11;
			intShowSelectedSO = SOUND_NO; //SOUND_OFFSET + SOUND_1_4; //3
			blnShowSelectedAS = true;
			intShowSelectedLS = LASER_BURST;
			intShowSelectedMH = MOVINGHEAD_RGBWSTEADY;
#ifdef FEATURE_PHOTO
			blnTakePhoto = true;
#endif
#ifdef FEATURE_CUECORE
                        charShowSelectedCueCore = "1";
#endif 
			break;
	}
	
#ifdef FEATURE_MP3_RANDOMSET
	if (intShowSelectedSO != 0) {
		int intRandomSet = intShowSelectedSO - 1;
		int intRandomSelection;
		do {
			intRandomSelection = (rand() % FEATURE_MP3_RANDOMSET_COUNT) + 1;
		} while (intRandomSelection == intRandomSelectionOld);
		intRandomSelectionOld = intRandomSelection;
		intShowSelectedSO = SOUND_OFFSET + (intRandomSet * FEATURE_MP3_RANDOMSET_COUNT) + intRandomSelection;
		SC_HIGHLIGHT("intRandomSet = %d aquasmash offset + %d / intRandomSelection = %d > intShowSelectedSO = %d\n",SOUND_OFFSET, intRandomSet, intRandomSelection, intShowSelectedSO);
	}
#endif
	
	
	scSliderIn->show_wm = intShowSelectedWM;
#ifdef FEATURE_MP3
	scSliderIn->show_so = intShowSelectedSO + MPD_INDEX;
#endif
	scSliderIn->show_pr = intShowSelectedPR;
#ifdef FEATURE_MP3
	scSliderIn->show_hit = intShowSelectedSOhit + MPD_INDEX;
#endif
	scSliderIn->show_ls = intShowSelectedLS;
	scSliderIn->show_mh = intShowSelectedMH;
#ifdef SLIDE_MODE_AQUASMASH
	scSliderIn->aquasmash = blnShowSelectedAS;
#endif
#ifdef FEATURE_PHOTO
	scSliderIn->photo = true;
#endif
#ifdef FEATURE_CUECORE
        char* arShow[12] = {"1", "2", "3", "4", "5","6","7","8","9","10","11","12"};
        int random = 0;
        do {
            random = (rand() % 12) + 1;
        } while (random == intLastCueCoreRandom);
        SC_HIGHLIGHT("intRandomSet = int random for show = %d\n",random);
        intLastCueCoreRandom = random;
        scSliderIn->show_cuecore = arShow[random];
#endif 		
	return true;
}

bool showStart(unsigned int universe, unsigned int showid, unsigned char nLampMax) {
	bool ret = edxStartShow(universe, showid, 0, nLampMax);
	return ret;
}

bool showStop(unsigned int universe, unsigned char nLampMax) {
	bool ret = edxStartShow(universe, 0, 0, nLampMax);
	return ret;
}


#ifdef FEATURE_DISPLAYBOARD

bool number_to_display(int bteDisplayBase, long lngNumber, int bteDigitCount, int bteDot, bool blnShow0) {
	int intDigit;
	int intModulo;
	int bteDigitNum;
	int dmxvalues[255];
	int address;
	
	SC_DEBUG("@scal.cpp/number_to_display(%d, %ld, %d, %d, %d)...\n", bteDisplayBase, lngNumber, bteDigitCount, bteDot, blnShow0);
	
	for (address = DMX_BASE_DISPLAY; address <= 255; address++) {
		dmxvalues[address] = -1;
	}


	if (lngNumber < 0) {
		for (bteDigitNum = 1; bteDigitNum <= bteDigitCount; bteDigitNum++) {
			dmxvalues[(DMX_BASE_DISPLAY) + (bteDisplayBase - 1) + (bteDigitNum - 1)] = 0;
		}
		return false;
	}

	bteDigitNum = 1;
	if (bteDigitCount >= 4) {
		if (lngNumber >= 1000) {
			intDigit = lngNumber / 1000;
			intModulo = lngNumber % 1000;
			dmxvalues[(DMX_BASE_DISPLAY) + (bteDisplayBase - 1) + (bteDigitNum - 1)] = dec_to_segm(intDigit, (bteDigitNum == bteDot), blnShow0);
		} else {
			intModulo = lngNumber;
			dmxvalues[(DMX_BASE_DISPLAY) + (bteDisplayBase - 1) + (bteDigitNum - 1)] = dec_to_segm(0, (bteDigitNum == bteDot), blnShow0);
		}
		bteDigitNum++;
	} else {
		intModulo = lngNumber;
	}

	if (lngNumber >= 100) {
		intDigit = intModulo / 100;
		intModulo = intModulo % 100;
		dmxvalues[(DMX_BASE_DISPLAY) + (bteDisplayBase - 1) + (bteDigitNum - 1)] = dec_to_segm(intDigit, (bteDigitNum == bteDot), blnShow0);
	} else {
		intModulo = lngNumber;
		dmxvalues[(DMX_BASE_DISPLAY) + (bteDisplayBase - 1) + (bteDigitNum - 1)] = dec_to_segm(0, (bteDigitNum == bteDot), blnShow0);
	}
	bteDigitNum++;

	if (lngNumber >= 10) {
		intDigit = intModulo / 10;
		intModulo = intModulo % 10;
		dmxvalues[(DMX_BASE_DISPLAY) + (bteDisplayBase - 1) + (bteDigitNum - 1)] = dec_to_segm(intDigit, (bteDigitNum == bteDot), blnShow0);
	} else {
		intModulo = lngNumber;
		dmxvalues[(DMX_BASE_DISPLAY) + (bteDisplayBase - 1) + (bteDigitNum - 1)] = dec_to_segm(0, (bteDigitNum == bteDot), blnShow0);
	}
	bteDigitNum++;

	intDigit = intModulo;
	dmxvalues[(DMX_BASE_DISPLAY) + (bteDisplayBase - 1) + (bteDigitNum - 1)] = dec_to_segm(intDigit, (bteDigitNum == bteDot), blnShow0);

	// Write values
	for (address = DMX_BASE_DISPLAY; address <= 255; address++) {
		if (dmxvalues[address] != -1) {
			edxSetAddress(EDX_SLIDE, address, dmxvalues[address]);
		}
	}
	
	return true;
}


int dec_to_segm(int dec, bool dot, bool show0) {
	int ret = 0;

	switch (dec) {
		case 0:
			if (show0) {
				ret = 126;
			} else {
				ret = 0;
			}
			break;
		case 1: ret = 96;
			break;
		case 2: ret = 218;
			break;
		case 3: ret = 248;
			break;
		case 4: ret = 228;
			break;
		case 5: ret = 188;
			break;
		case 6: ret = 190;
			break;
		case 7: ret = 112;
			break;
		case 8: ret = 254;
			break;
		case 9: ret = 252;
			break;
	}
	if (dot) {
		ret += 1;
	}

	return ret;
}



#endif

#ifdef FEATURE_EMERGENCY_BUTTON

bool systemSetEmergency() {
	bool ret = true;

#ifdef FEATURE_CUECORE
        setCueCore(CUECORE_EMERGENCY);
#endif
#ifdef FEATURE_MP3
	soundStart(MPD_SLIDE,0);
#endif
#ifdef FEATURE_GUI
	//disable touchscreen
	guiSetScreenActive(GUI_SLIDE, false,BRAND_BRIGHTSIGN);	
#endif
#ifdef FEATURE_COUNTDOWNLIGHT
	//set to red
	//lightSetRed();
	lightSetCross();
#endif
        
#ifdef FEATURE_MOVINGHEAD
        //movingheadStart(MOVINGHEAD_RGBWSTEADY);
#endif
#ifdef FEATURE_SCORESCREEN
	//disable scorescreen
#endif
#ifdef SLIDE_MODE_WHITEMAGIC
#ifdef SLIDE_MODE_AQUASMASH
	showStop(EDX_SLIDE,DMX_SHOW_LAMPS+CTD_SUPPORTED);
	edxSetLamp(EDX_SLIDE, 0, DMX_VALUE_FULL, DMX_VALUE_FULL, DMX_VALUE_FULL, 1000);
#else
	showStop(EDX_SLIDE,DMX_SHOW_LAMPS);
	edxSetLamp(EDX_SLIDE, 0, DMX_VALUE_FULL, DMX_VALUE_FULL, DMX_VALUE_FULL, 1000);
#endif
#endif
#ifdef FEATURE_LASER
        laserStart(0,1);
        laserStart(0,2);
#endif
#ifdef FEATURE_PROJECTION
	//all beamers to white
#if SOURCEBRAND == BRAND_SPINETIX
	prjSetPower(false); // @@@
#endif
#if SOURCEBRAND == BRAND_BRIGHTSIGN
	// all brightsigns to IDLE to be sure
#ifdef IP_PROJ_MD_1
	brsSetShow(IP_PROJ_MD_1,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_EMERGENCY);
#endif
#ifdef IP_PROJ_MD_2
	brsSetShow(IP_PROJ_MD_2,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_EMERGENCY);
#endif
#ifdef IP_PROJ_MD_3
	brsSetShow(IP_PROJ_MD_3,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_EMERGENCY);
#endif
#ifdef IP_PROJ_MD_4
	brsSetShow(IP_PROJ_MD_4,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_EMERGENCY);
#endif
#ifdef IP_PROJ_MD_5
	brsSetShow(IP_PROJ_MD_5,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_EMERGENCY);
#endif
#ifdef IP_PROJ_MD_6
	brsSetShow(IP_PROJ_MD_6,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_EMERGENCY);
#endif
#ifdef IP_PROJ_MD_7
	brsSetShow(IP_PROJ_MD_7,IP_PROJ_MD_PORT,(char *)IP_PROJ_MD_STATE_EMERGENCY);
#endif
#endif
	// all beamers on
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
#ifdef IP_PROJ_BEAMER_6
	pjlSetPower(IP_PROJ_BEAMER_6,true);
#endif
#ifdef IP_PROJ_BEAMER_7
	pjlSetPower(IP_PROJ_BEAMER_7,true);
#endif
#ifdef IP_PROJ_BEAMER_8
	pjlSetPower(IP_PROJ_BEAMER_8,true);
#endif
#endif
	return ret;
}

bool getEmergencyInput(int DeviceId)
{
	return ediGetInput(DeviceId);
}

bool systemClearEmergency() {
	return true;
}

bool setLampOff(){
            //edxSetLamp(EDX_SLIDE_2, 0, DMX_VALUE_OFF, DMX_VALUE_OFF, DMX_VALUE_OFF, 1000);
}

#ifdef FEATURE_ECUE

bool LedRingSetShow(int nTheme) {
	ecuSetShow(ECUE_DEVICE,SLIDEID,nTheme);
	return true;
}

bool LedRingSetEmergency() {
	ecuSetEmergency(ECUE_DEVICE,SLIDEID);
	return true;
}

bool LedRingSetPowerOff() {
	ecuSetPowerOff(ECUE_DEVICE,SLIDEID);
	return true;
}

bool LedRingSetStandby() {
	ecuSetStandby(ECUE_DEVICE,SLIDEID);
	return true;
}
#endif

#endif


//#ifdef FEATURE_PROJECTION
//
//bool SetBrightSignTheme(const char * chrIP, char *theme)
//{
//	brsSetShow(chrIP, IP_PROJ_MD_PORT, theme);
//	return true;
//}
//
//#endif

#ifdef FEATURE_LASER
bool laserStart(unsigned int value, int lazer) {
	unsigned int universe = EDX_SLIDE_1;

	SC_HIGHLIGHT("@scal.cpp: laserStart(%d)\n", value);
        if(lazer == 1) {
            edxSetAddress(universe, DMX_LASER_1, value);
        }
	if (lazer == 2) { 
            edxSetAddress(universe, DMX_LASER_2, value);
        }
	return true;	
}
#endif

#ifdef FEATURE_MOVINGHEAD
bool movingheadStart(unsigned int show) {
	unsigned int universe = EDX_SLIDE_2;
	int mhpan = 0; // 0...255 = 0...540deg
	int mhtilt = 0;
	int mhcontpan = 0; // 16...135 = clockwise, slow to fast / 136...255 = counterclockwise / overrules mhpan
	int mhconttilt = 0; // 16...135 = clockwise, slow to fast / 136...255 = counterclockwise / overrules mhtilt
	int mhdimmer = 0; // 0...100%
	int mhcolorpreset = 0;
	int mhcolorrunning = 0; // overrules colorpreset
	int mhcolorrunningspeed = 0; // slow to fast
//	int mhfunction = 0; // overrule colorrunning and colorrunningspeed
	
//	show = 4;
	SC_HIGHLIGHT("@scal.cpp: movingheadStart(%d)\n", show);
	switch (show) {
		case MOVINGHEAD_RAINBOWMOVING: // rainbow moving
			mhcontpan = 195; // counterclockwise mid
			mhconttilt = 75; // clockwise mid
			mhdimmer = 255; // 100%
			mhcolorpreset = 0;
			mhcolorrunning = 200; // rainbow 
			mhcolorrunningspeed = 255; // fast
			break;

		case MOVINGHEAD_GREENPANNING: // green panning
			mhcontpan = 255; // counterclockwise fast
			mhconttilt = 0;
			mhdimmer = 255; // 100%
			mhcolorpreset = 77; // green
			mhcolorrunning = 0; 
			mhcolorrunningspeed = 0;
			break;
		
		case MOVINGHEAD_RGBWTILTING: // RGBW tilting
			mhpan = 42; // 90deg
			mhcontpan = 0;
			mhconttilt = 135; // clockwise fast
			mhdimmer = 255; // 100%
			mhcolorpreset = 231; // RGBW
			mhcolorrunning = 0; 
			mhcolorrunningspeed = 0;
			break;

		case MOVINGHEAD_DISCOMOVING: // disco moving
			mhcontpan = 255; // counterclockwise fast
			mhconttilt = 135; // clockwise fast
			mhdimmer = 255; // 100%
			mhcolorpreset = 0;
			mhcolorrunning = 136; // jump 
			mhcolorrunningspeed = 255; // fast
			break;

		case MOVINGHEAD_RGBWSTEADY: // white steady
			mhcontpan = 0;
			mhconttilt = 0;
			mhdimmer = 255; // 100%
			mhcolorpreset = 231; // RGBW
			mhcolorrunning = 0; 
			mhcolorrunningspeed = 0;
			break;
			
		default:
			// all off
			break;
	}
	
//	int mhcolor = (rand() % 256);
//	int mhgobo = (rand() % 256);
//	int mhrotation = (rand() % 256);
//	int mhprism = (rand() % 256);
//	int mhprogram = 136 + ((showid - 1) * 11);	
//	unsigned char value = 37 + ((showid - 1) * 8);
	
	if (mhcontpan == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_1 + (1 - 1), mhpan);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_1 + (3 - 1), mhcontpan);
	if (mhconttilt == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_1 + (2 - 1), mhtilt);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_1 + (4 - 1), mhconttilt);
	edxSetAddress(universe, DMX_MOVINGHEAD_1 + (5 - 1), mhdimmer);
	if (mhcolorrunning == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_1 + (7 - 1), mhcolorpreset);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_1 + (8 - 1), mhcolorrunning);
	edxSetAddress(universe, DMX_MOVINGHEAD_1 + (9 - 1), mhcolorrunningspeed);

	if (mhcontpan == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_2 + (1 - 1), mhpan);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_2 + (3 - 1), mhcontpan);
	if (mhconttilt == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_2 + (2 - 1), mhtilt);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_2 + (4 - 1), mhconttilt);
	edxSetAddress(universe, DMX_MOVINGHEAD_2 + (5 - 1), mhdimmer);
	if (mhcolorrunning == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_2 + (7 - 1), mhcolorpreset);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_2 + (8 - 1), mhcolorrunning);
	edxSetAddress(universe, DMX_MOVINGHEAD_2 + (9 - 1), mhcolorrunningspeed);

	if (mhcontpan == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_3 + (1 - 1), mhpan);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_3 + (3 - 1), mhcontpan);
	if (mhconttilt == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_3 + (2 - 1), mhtilt);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_3 + (4 - 1), mhconttilt);
	edxSetAddress(universe, DMX_MOVINGHEAD_3 + (5 - 1), mhdimmer);
	if (mhcolorrunning == 0) {
		edxSetAddress(universe, DMX_MOVINGHEAD_3 + (7 - 1), mhcolorpreset);
	}
	edxSetAddress(universe, DMX_MOVINGHEAD_3 + (8 - 1), mhcolorrunning);
	edxSetAddress(universe, DMX_MOVINGHEAD_3 + (9 - 1), mhcolorrunningspeed);
	
	return true;	
}
#endif

bool setlampsGreen() {
    edxSetLamp(EDX_SLIDE, 0, DMX_VALUE_OFF, DMX_VALUE_FULL, DMX_VALUE_OFF, 1000);
    return true;
}

#ifdef FEATURE_CUECORE
bool setCueCore(char* command) {
        cueSetShowTCP(CUECORE_DEVICE, CUECORE_PORT, command);
    return true;
}

#endif

/*		
#ifdef FEATURE_LAZER
bool setEffect(int universe,int effect,int channel, int value) {
	edxSetAddress(universe, effect + channel, value);
	return true;	
}
#endif

      //lets start the default programs for now
    //channel 10 is the start of the programs, last value is the premade program(see manual on evernote on this project)
      setEffect(EDX_SLIDE,DMX_BOL_1,10, 6);
      setEffect(EDX_SLIDE,DMX_BOL_2,10, 11);
      setEffect(EDX_SLIDE,DMX_BOL_3,10, 21);
      //unknown for lazer
*/



/*
bool fazerStart(unsigned char smokevalue, unsigned char fanvalue) {
	unsigned int universe = EDX_CONE_3;
	edxSetAddress(universe, DMX_FAZER, smokevalue);
	edxSetAddress(universe, DMX_FAZER + 1, fanvalue);
	
	return true;	
}

bool discoStart(unsigned int mhprogram) {
	unsigned int universe = EDX_CONE_3;
	// Moving Head
	int mhcolor = (rand() % 256);
	int mhgobo = (rand() % 256);
	int mhrotation = (rand() % 256);
	int mhprism = (rand() % 256);
//	int mhprogram = 136 + ((showid - 1) * 11);
	edxSetAddress(universe, DMX_MH_1 + (6 - 1), mhcolor); // colorwheel
	edxSetAddress(universe, DMX_MH_1 + (7 - 1), 255); // shutter open
	edxSetAddress(universe, DMX_MH_1 + (8 - 1), 255); // dimmer full
	edxSetAddress(universe, DMX_MH_1 + (9 - 1), mhgobo); // gobo wheel
	edxSetAddress(universe, DMX_MH_1 + (10 - 1), mhrotation); // gobo rotation
	edxSetAddress(universe, DMX_MH_1 + (11 - 1), mhprism); // prism
	edxSetAddress(universe, DMX_MH_1 + (13 - 1), mhprogram); // built in program

	edxSetAddress(universe, DMX_MH_2 + (6 - 1), mhcolor); // colorwheel
	edxSetAddress(universe, DMX_MH_2 + (7 - 1), 255); // shutter open
	edxSetAddress(universe, DMX_MH_2 + (8 - 1), 255); // dimmer full
	edxSetAddress(universe, DMX_MH_2 + (9 - 1), mhgobo); // gobo wheel
	edxSetAddress(universe, DMX_MH_2 + (10 - 1), mhrotation); // gobo rotation
	edxSetAddress(universe, DMX_MH_2 + (11 - 1), mhprism); // prism
	edxSetAddress(universe, DMX_MH_2 + (13 - 1), mhprogram + (1 * MOVINGHEAD_OFFSET)); // built in program

	edxSetAddress(universe, DMX_MH_3 + (6 - 1), mhcolor); // colorwheel
	edxSetAddress(universe, DMX_MH_3 + (7 - 1), 255); // shutter open
	edxSetAddress(universe, DMX_MH_3 + (8 - 1), 255); // dimmer full
	edxSetAddress(universe, DMX_MH_3 + (9 - 1), mhgobo); // gobo wheel
	edxSetAddress(universe, DMX_MH_3 + (10 - 1), mhrotation); // gobo rotation
	edxSetAddress(universe, DMX_MH_3 + (11 - 1), mhprism); // prism
	edxSetAddress(universe, DMX_MH_3 + (13 - 1), mhprogram + (2 * MOVINGHEAD_OFFSET)); // built in program

	edxSetAddress(universe, DMX_MH_4 + (6 - 1), mhcolor); // colorwheel
	edxSetAddress(universe, DMX_MH_4 + (7 - 1), 255); // shutter open
	edxSetAddress(universe, DMX_MH_4 + (8 - 1), 255); // dimmer full
	edxSetAddress(universe, DMX_MH_4 + (9 - 1), mhgobo); // gobo wheel
	edxSetAddress(universe, DMX_MH_4 + (10 - 1), mhrotation); // gobo rotation
	edxSetAddress(universe, DMX_MH_4 + (11 - 1), mhprism); // prism
	edxSetAddress(universe, DMX_MH_4 + (13 - 1), mhprogram + (3 * MOVINGHEAD_OFFSET)); // built in program
	
	// Pathfinder
	edxSetAddress(universe, DMX_P_1 + (1 - 1), 255); // SOUND-ACTIVITY mode
	edxSetAddress(universe, DMX_P_2 + (1 - 1), 255); // SOUND-ACTIVITY mode
	
	return true;	
}


bool fazerSetPump(bool blnOn) {
	SC_HIGHLIGHT("@scal.cpp/fazerSetPump(%d)...\n", blnOn);
//	edoSetOutput(RIO_RELAY_1, blnOn);
	
	return true;
}
*/
