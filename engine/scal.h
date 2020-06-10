#include "slidecontroller.h"
#include <bitset>

// Classes
class scSensor {
public:
    scSensor(void);
    bool init(int intType, char const * chrIP, int intChannel, int intId, bool blnInverted);
    bool state(bool blnBlock);
private:
    int intType;
    int intId;
    bool blnInverted;
    bool blnBlocked;
};



bool scalInit();

bool systemSetSecure();
bool systemSetNormal();
bool systemCleanup();
#ifdef FEATURE_EMERGENCY_BUTTON
bool systemSetEmergency();
bool systemClearEmergency();
bool getEmergencyInput(int DeviceId);
#endif

bool lightSetRed();
bool lightSetGreen();
bool lightSetOrange();
bool lightSetBlue();
bool setLampOff();
//bool inputGetFoggerLow();
//bool inputGetFoggerHigh();
//bool inputGetFoggerMainLow();

#ifdef SLIDE_MODE_AQUASMASH
bool asLoadCtd(unsigned char nCtdColorSet[CTD_SUPPORTED]);
bool asFlashCtd(int ctd);
bool asStop();
#endif
void scProcessSliderDataThread(unsigned int ts, unsigned int show, unsigned long duration_ms, float speed_ms, unsigned int red, unsigned int green, unsigned int blue, unsigned int white, unsigned int total, unsigned int score, bool photo);
void scProcessSliderDataThread(unsigned int ts, unsigned int show, unsigned long duration_ms, float speed_ms, unsigned int red, unsigned int green, unsigned int blue, unsigned int white, unsigned int total, unsigned int score, bool photo, char* rfid);
void scProcessSliderData(unsigned int ts, unsigned int show, unsigned long duration_ms, float speed_ms, unsigned int red, unsigned int green, unsigned int blue, unsigned int white, unsigned int total, unsigned int score, bool photo, char* rfid);

#ifdef SLIDE_MODE_WHITEMAGIC
bool wmStartShow(int intShowSelected);
bool wmStopShow();
#endif

#ifdef FEATURE_SCORESCREEN
void triggerScoreScreen();
#endif

bool soundStart(unsigned char playerid, int soundid);

bool scSetShow(scSlider * scSliderIn);

bool showStart(unsigned int universe, unsigned int showid, unsigned char nLampMax);
bool showStop(unsigned int universe, unsigned char nLampMax);
bool number_to_display(int bteDisplayBase, long lngNumber, int bteDigitCount, int bteDot, bool blnShow0);
int dec_to_segm(int dec, bool dot, bool show0);

#ifdef FEATURE_LASER
bool laserStart(unsigned int value,int lazer);
#endif

#ifdef FEATURE_MOVINGHEAD
bool movingheadStart(unsigned int value);
#endif
bool setlampsGreen();

#ifdef FEATURE_CUECORE
bool setCueCore(char *command);
#endif