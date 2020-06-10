#include <stdio.h> // printf
#include <time.h> // time_t

#define VERSION_NUMBER "2.1"
#define VERSION_YEAR 2017
#define LIB_VERSION "1.8"
#define INCLUDE_PATH "~/git/islidecontroller/common/"LIB_VERSION"/var/engine" // @@@ Is this used?
#define SLIDE_CONFIG_NIEUWVLIETBAD
// ----- Edit slide parameters below ----- 

#ifdef SLIDE_CONFIG_NIEUWVLIETBAD
// Debug
#define DO_DEBUG
//#define TEST_ALWAYS_PHOTO // @@@

//*** Generic and timing. ***
#define VERSION_NAME "Nieuwvlietbad"

//#define FEATURE_CUECORE
#ifdef FEATURE_CUECORE
#define CUECORE_DEVICE "10.124.64.115"
#define CUECORE_PORT 7000
#define CUECORE_IDLE "SI"
#define CUECORE_EMERGENCY "SE"
#define CUECORE_OFF "OFF"
#endif

//#define FEATURE_BUTTON_SCORERESET

//#define FEATURE_CONESENSOR
//#define FEATURE_LASER
//#define FEATURE_MOVINGHEAD
//value depend on starting address and DMX range not known now
#define DMX_MOVINGHEAD_1 120 // checked
#define DMX_MOVINGHEAD_2 131 // checked
#define DMX_MOVINGHEAD_3 142 // checked

#define DMX_LASER_1 210 // show control channel, this is the first channel of the laser that we set, in this case its the lasers first adress
#define DMX_LASER_2 222

//#define FEATURE_RFID
//#define FEATURE_CAMERASENSOR
#define SLIDE_MODE_AQUASMASH
#define SLIDE_MODE_WHITEMAGIC
//#define FEATURE_RANDOMSHOW
//#define SLIDE_MODE_SLIDECHAMP
#define SLIDE_MODE_SENSOR // @@@ detect from inputs
#define FEATURE_TIME
#define TIME_SLIDE_INIT 10000 // Time needed to start pumps etc.
#define TIME_SLIDE_AVG 10000 // Set only average slide time in most cases
#define TIME_SLIDE_MIN 5000
#define TIME_SLIDE_MAX 150000
#define TIME_PHOTODELAY 750 //500 //ms // @@@
#define TIME_SLIDE_EXIT 3000
#define SLIDE_LENGTH 29		// Length of the slide (m) to calculate speed
#define SPEED_WEIGHT 10
#define FEATURE_EMERGENCY_BUTTON
#define FEATURE_RANDOMSHOW
#define FEATURE_ALWAYSSOUND

//#define FEATURE_API

//#define FEATURE_HOLD_ONEXIT
#ifdef FEATURE_HOLD_ONEXIT
#define HOLD_TIME_SEC 10 //sleep in seconds
#endif

#define SCR_SLIDE 1
//*** IO ***
#define FEATURE_SOLLAE
#define FEATURE_ETHDMX // for DMX
#define FEATURE_ETHDMXIN // for the 2 or 5 inputs
#define EDX_SLIDE 1 // DMX Universe
#define EDX_ID_1 1 // id used for init

//*** GUI ***
#define FEATURE_GUI // touchscreen

#ifdef FEATURE_GUI
#define FEATURE_TOUCHSCREEN
#ifndef BRAND_SPINETIX
#define BRAND_SPINETIX 0
#endif
#ifndef BRAND_BRIGHTSIGN
#define BRAND_BRIGHTSIGN 1
#endif
#ifndef TOUCHSCREENBRAND
#define TOUCHSCREENBRAND BRAND_BRIGHTSIGN
#endif
#define IP_TS "10.124.64.97"
#define GUI_SLIDE 1 // id Touchscreen
#endif

//#define FEATURE_PROJECTION
#ifdef FEATURE_PROJECTION
#define SOURCEBRAND BRAND_BRIGHTSIGN
#define IP_PROJ_BEAMER_1 "10.124.64.35"
#define IP_PROJ_MD_1 "10.124.64.82"
#define IP_PROJ_MD_PORT 5000
#define IP_PROJ_MD_STATE_EMERGENCY "SE"
#define IP_PROJ_MD_STATE_IDLE "SI"
#define PRJ_SLIDE 1
#endif

#ifdef FEATURE_GUI_BUTTONS
#define IP_IO_BUTTONS_1 "10.124.64.52"
#define IP_IO_BUTTONS_2 "10.124.64.53"
#define IO_BUTTONS_TYPE IO_TYPE_SOLLAE
#undef IO_BUTTON1_RANDOM
#endif
#define FEATURE_COUNTDOWNLIGHT
#define IP_CDL "10.124.64.65"
#define CDL_SLIDE 1 // id CountDownLight

//#define FEATURE_SCORESCREEN
#ifdef FEATURE_SCORESCREEN
#define SCR_SLIDE 1 // id ScoreScreen
#define BRAND_BRIGHTSIGN 1


#define SCORESCREENBRAND BRAND_BRIGHTSIGN
#define IP_SCREEN "10.124.59.17"
#endif

// Sensors
// Sensors
#define IP_IO_SENSORS_1 "10.124.64.113"
#define IP_ETHDMX_1 IP_IO_SENSORS_1
#define SENSOR_MAX 40
#define EDX_SLIDE 1

#define IO_SENSOR_TYPE IO_TYPE_EDX //IO_TYPE_SOLLAE
#define SENSOR_IN 1
#define SENSOR_OUT 2

#define SENSOR_PANIC 3

#ifdef FEATURE_EMERGENCY_BUTTON
#define SENSOR_EMERGENCY 4
#else
//#define SENSOR_RESET 4
#endif
//#define SENSOR_SCORERESET 5 // @@@ remove this functionality for now

#ifdef FEATURE_CONESENSOR
#define SENSOR_CONE_IN 6
#define SENSOR_CONE_OUT 7
#endif

#ifdef SLIDE_MODE_AQUASMASH
#define SENSOR_OFFSET_CTD 6
#define IO_CTD_TYPE IO_TYPE_SOLLAE
#define IP_IO_CTD_1 "10.124.64.50" // CTD 1-5
//#define IP_IO_CTD_2 "10.124.40.51" // CTD 6-10
// after DMX_SHOW_LAMPS: dmx address 121 (40 x 3 ch)
#define CTD_COUNT 8
#endif

// camera & photonode
//#define FEATURE_PHOTO
#ifdef FEATURE_PHOTO
#define FEATURE_UPLOAD
#define IP_CAM "10.124.59.33"
#define CAM_SLIDE 1 // id
#define WEB_SLIDE 1 // id
#define URL_API_SSL "https://hofvansaksen.islidephoto.nl/api/add_photo/"
#define UUID "719c33ec88354ac2d3a7866e28777788b9fc776d" //UUID Photonode
#endif

// Soundboxes
#define FEATURE_MP3
#ifdef FEATURE_MP3
#define IP_SB_1 "10.124.64.81"
#define MPD_SLIDE 1 // id
#define MPD_VOLUME 90
#define MPD_INDEX 1 //set database index to 0 or one
//#define FEATURE_ALWAYSSOUND
#define FEATURE_MP3_RANDOMSET	
#define FEATURE_MP3_RANDOMSET_COUNT 3
#endif

#define DMX_SHOW_LAMPS 40 // dmx range = 40 x 3 ch = 1 - 120

#define FEATURE_DISPLAYBOARD
#ifdef FEATURE_DISPLAYBOARD
#define DMX_BASE_DISPLAY 193

#define SC_DISPLAY_TIME 1
#define SC_DISPLAY_TIME_D 2
#define SC_DISPLAY_TIME_L 4

#define SC_DISPLAY_SPEED 5
#define SC_DISPLAY_SPEED_L 4
#define SC_DISPLAY_SPEED_D 2

#define SC_DISPLAY_BEST 9
#define SC_DISPLAY_BEST_L 4
#define SC_DISPLAY_BEST_D 2

#endif

#endif

#ifndef TIME_SLIDE_MAX
	// Maximum slide time (ms), light is set to green again in case exit sensor has not detected the sliding person (0 = infinite)
	#define TIME_SLIDE_MAX (TIME_SLIDE_AVG * 2)

#endif

#ifndef TIME_SLIDE_MIN
	// Minimum slide time (ms)
	#define TIME_SLIDE_MIN (unsigned int) ((TIME_SLIDE_AVG / 4) * 3)
#endif

#ifndef TIME_SLIDE_EXIT
	// Time to wait after exit sensor is passed (ms), this is also the time the light is orange
	#define TIME_SLIDE_EXIT (unsigned int) (TIME_SLIDE_AVG / 4)
#endif

// TIME-MODE (no sensors installed)

#ifndef TIME_RED
// Time to set red light on (ms)
#define TIME_RED 20000
#endif

#ifndef TIME_GREEN
// Time to set green light on (ms)
#define TIME_GREEN 2000
#endif

#ifndef TIME_ORANGE
// Time to set green light on (ms)
#define TIME_ORANGE 2000
#endif

#define TIME_SLIDE_NEXT 21000

// defines
#define DO_DEBUG
// Enable / disable test features
// #define DISABLE_PANIC
// #define TEST_ALWAYS_PHOTO


// ----- End of slide parameters -----

#define PRINTF_RESET	"\033[0m"
#define PRINTF_BLACK	"\033[30m"      /* Black */
#define PRINTF_RED		"\033[31m"      /* Red */
#define PRINTF_GREEN	"\033[32m"      /* Green */
#define PRINTF_YELLOW	"\033[33m"      /* Yellow */
#define PRINTF_BLUE		"\033[34m"      /* Blue */
#define PRINTF_MAGENTA	"\033[35m"      /* Magenta */
#define PRINTF_CYAN		"\033[36m"      /* Cyan */
#define PRINTF_WHITE	"\033[37m"      /* White */
#define PRINTF_BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define PRINTF_BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define PRINTF_BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define PRINTF_BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define PRINTF_BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define PRINTF_BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define PRINTF_BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define PRINTF_BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
#define SC_ERROR(args...) fprintf(stderr, PRINTF_BOLDWHITE); {time_t rawtime; struct tm * timeinfo; char buffer [80]; time(&rawtime); timeinfo = localtime(&rawtime); strftime(buffer, 80, "%H:%M:%S ", timeinfo); fprintf(stderr, buffer);} fprintf(stderr, PRINTF_RESET); fprintf(stderr, PRINTF_BOLDRED); fprintf(stderr, args); fprintf(stderr, PRINTF_RESET); fflush(stderr); // Will now print everything in the stderr buffer immediately

#ifdef DO_DEBUG
#define SC_PRINTTIME {time_t rawtime; struct tm * timeinfo; char buffer [80]; time(&rawtime); timeinfo = localtime(&rawtime); strftime(buffer, 80, "%H:%M:%S ", timeinfo); printf(buffer);}
#define SC_DEBUG(args...) printf(PRINTF_BOLDWHITE); SC_PRINTTIME printf(PRINTF_RESET); printf(args); fflush(stdout); // Will now print everything in the stdout buffer immediately
#define SC_HIGHLIGHT(args...) printf(PRINTF_BOLDWHITE); SC_PRINTTIME printf(PRINTF_RESET); printf(PRINTF_YELLOW); printf(args); printf(PRINTF_RESET); fflush(stdout); // Will now print everything in the stdout buffer immediately
#else
#define SC_DEBUG(args...) ;
#endif

#define MYSQL_SERVER "127.0.0.1"
#define MYSQL_USER "web"
#define MYSQL_PASSWORD "w3b"
#define MYSQL_DATABASE "slide"


// Set features
#ifdef SLIDE_MODE_AQUASMASH
#define FEATURE_REMOTEIO
#define FEATURE_TIME
//#define FEATURE_DMX
//#define FEATURE_MP3
//#define FEATURE_PHOTO
#endif

#ifdef SLIDE_MODE_WHITEMAGIC
//#define FEATURE_NUDAM
//#define FEATURE_DMX
#endif

#ifdef SLIDE_MODE_SLIDECHAMP
#define FEATURE_REMOTEIO
#define FEATURE_TIME
#endif

#ifdef SLIDE_MODE_FACEBOOK
#define FEATURE_PHOTO
#define FEATURE_TIME
#endif

// Disable features in demo mode
#ifdef DEMOMODE
#undef FEATURE_NUDAM
#endif



// defines

#define DIR_TMP "/var/www/slide/tmp/"
#define DIR_PHOTO "/var/www/slide/snapshots/"
#define PHOTO_RETRIES 10

#define RESOLUTIONMS_TIMER 10 //100 //ms
#define RESOLUTIONMS_DMX 50

#define TIMER_RED 1
#define TIMER_ORANGE 2
#define TIMER_GREEN 3
#define TIMER_SLIDE_MAX 4
#define TIMER_SLIDE_MIN 5
#define TIMER_EXIT 6
#define TIMER_DEMO 7
#define TIMER_SLIDE_INIT 8
#define TIMER_PANIC_REFRESH 9
#define TIMER_START_REFRESH 10
#define TIMER_FAZER_PUMP 11
#ifdef FEATURE_EMERGENCY_BUTTON
#define TIMER_EMERGENCY_REFRESH 12
#endif
#define TIMER_LAZER1_START 13
#define TIMER_LAZER2_START 14

#define TIME_PANIC_REFRESH 30000 //ms
#define TIME_START_REFRESH 10000 //ms
#define TIME_FAZER_ACTIVE 5000 //ms 
#define TIME_FAZER_PUMP 60000
#define TIME_EMERGENCY_REFRESH 10000 //ms

#ifdef SLIDE_MODE_AQUASMASH
#define INPUT_OFFSET_SENSOR_CTD 0
#define INPUT_OFFSET_BUTTON_AS 6
#define CTD_SUPPORTED 16
#define CTD_COLOR_RED 0
#define CTD_COLOR_GRN 1
#define CTD_COLOR_BLU 2
#define CTD_COLOR_WHT 3
#define CTD_POINTS_RED 50
#define CTD_POINTS_GRN 50
#define CTD_POINTS_BLU 50
#define CTD_POINTS_WHT 50
#endif

#ifdef SLIDE_MODE_WHITEMAGIC
#define INPUT_OFFSET_BUTTON_WM 20
#endif

#define INPUT_GROUP_SENSOR 1

// NuDAM outputs
#define OUTPUT_LAMP_GO 1
#define OUTPUT_CONTACT_RECORD 2
#ifdef SLIDE_MODE_WHITEMAGIC
#define OUTPUT_OFFSET_LAMP_WM 3
#endif
#ifdef SLIDE_MODE_WHITEMAGIC
#define OUTPUT_OFFSET_LAMP_AS 11
#endif

#define OUTPUT_GROUP_LAMP 1

// IO
#define PIN_SENSOR_IN PIN_DEF_IN1
#define PIN_SENSOR_OUT PIN_DEF_IN2
#define PIN_BUTTON_PANIC PIN_DEF_IN3
#define PIN_BUTTON_RESET PIN_DEF_IN4
#define PIN_BUTTON_PHOTO PIN_DEF_IN5
#define PIN_BUTTON_TAMPER PIN_DEF_IN6
#define PIN_LED_STATE PIN_DEF_LED1 // HIGH = green
#define PIN_LED_ERROR PIN_DEF_LED2 // HIGH = red
#define PIN_RELAY_LIGHT PIN_DEF_OUT1 // HIGH = Red / LOW = Green // @@@ ???

// DMX-shield
#define PIN_DMX_DIR 4
#define PIN_DMX_TX 2
#define PIN_DMX_S1 A1
#define PIN_DMX_LED2 8

#define DMX_BASE_TRAFFICLIGHT 47
#define DMX_BASE_LAMPS 1 //17
#define DMX_OFFSET_LAMP 3 // 6
#define DMX_OFFSET_RED 0
#define DMX_OFFSET_GREEN 1
#define DMX_OFFSET_BLUE 2
#define DMX_LAMP_COUNT 15
#define DMX_START 1
#define DMX_MAXCHANNEL (DMX_BASE_LAMPS + (DMX_LAMP_COUNT * (DMX_OFFSET_LAMP))) + 10 // + 10 for trafficlight etc.
#define DMX_VALUE_OFF 0
#define DMX_VALUE_LOW 20
#define DMX_VALUE_HALF 127
#define DMX_VALUE_FULL 255

#define DMX_BASE_MP3 1 //9
#define DMX_OFFSET_COMMAND 0
#define DMX_OFFSET_TRACK 1
#define DMX_OFFSET_VOLUME 2
#define DMX_OFFSET_BALANCE 3
#define MP3_COMMAND_PLAY 1
#define MP3_COMMAND_STOP 2
#define MP3_BALANCE_MID 127
#define MP3_VOLUME 255 //200

#define MP3_OFFSET_WM 200
#ifndef DMX_SHOW_LAMPS
#define DMX_SHOW_LAMPS 42 // dmx show end address = 42 x 3 ch = 126
#endif

// Effect DMX channels
//#define DMX_LASER_1 129
//#define DMX_LASER_2 148
#define DMX_MH_1 167
#define DMX_MH_2 180
#define DMX_MH_3 193
#define DMX_MH_4 206
#define DMX_P_1 219
#define DMX_P_2 222
#define DMX_FAZER 225

// RS485-breakout
#define PIN_RXTXCONTROL 3

// Shows
#define SHOW_STANDBY 0
#define SHOW_RAINBOW 1
#define SHOW_CIRCLELIGHT 2
#define SHOW_DISCO 3
#define SHOW_FLASHLIGHT 4
#define SHOW_REDFADE 5
#define SHOW_GREENFADE 6
#define SHOW_BLUEFADE 7
#define SHOW_YELLOWFADE 8
#define SHOW_PINKFADE 9
#define SHOW_CYANFADE 10
#define SHOW_WHITEFADE 11
#define SHOW_RANDOMFADE 12

#define SHOW_AS_PICTURE 9
#define SHOW_AS_NOPIC 10

#define LASER_3D 39
#define LASER_SCANNEDBEAM 59
#define LASER_LUMIA 78
#define LASER_BURST 127
#define LASER_UNIVERSAL 111

#define MOVINGHEAD_RAINBOWMOVING 1
#define MOVINGHEAD_GREENPANNING 2
#define MOVINGHEAD_RGBWTILTING 3
#define MOVINGHEAD_DISCOMOVING 4
#define MOVINGHEAD_RGBWSTEADY 5
#define MOVINGHEAD_OFFSET 10

#define PROJECTION_SHOW1 12
#define PROJECTION_SHOW2 22
#define PROJECTION_SHOW3 32
#define PROJECTION_SHOW4 42

// Board Olimex
#define PIN_DEF_IN1 28
#define PIN_DEF_IN2 29
#define PIN_DEF_IN3 30
#define PIN_DEF_IN4 31
#define PIN_DEF_IN5 32
#define PIN_DEF_IN6 33

#define PIN_DEF_OUT1 34
#define PIN_DEF_OUT2 35
#define PIN_DEF_OUT3 36
#define PIN_DEF_OUT4 37

#define PIN_DEF_LED1 38
#define PIN_DEF_LED2 39
#define PIN_DEF_LED3 40
#define PIN_DEF_LED4 41


// Gui Commands
#define GUI_CMD_IN 1
#define GUI_CMD_OUT 2
#define GUI_CMD_PANIC 3
#define GUI_CMD_RESET 4
#define GUI_CMD_PHOTO 5
#define GUI_CMD_FOGGERLOW 6
#define GUI_CMD_FOGGERHIGH 7
#define GUI_CMD_FOGGERMAINLOW 8


// MP3's
#define SOUND_NO 0
#ifdef FEATURE_GUI_BUTTONS
#define SOUND_1_2 1
#define SOUND_1_3 2
#define SOUND_1_4 0
#define SOUND_2_2 3
#define SOUND_2_3 4
#define SOUND_2_4 0
#define SOUND_3_2 5
#define SOUND_3_3 6
#define SOUND_3_4 0
#define SOUND_4_2 7
#define SOUND_4_3 8
#define SOUND_4_4 0
#else
#define SOUND_1_2 1
#define SOUND_1_3 2
#define SOUND_1_4 3
#define SOUND_2_2 4
#define SOUND_2_3 5
#define SOUND_2_4 6
#define SOUND_3_2 7
#define SOUND_3_3 8
#define SOUND_3_4 9
#define SOUND_4_2 10
#define SOUND_4_3 11
#define SOUND_4_4 12
#endif
#ifdef SLIDE_MODE_AQUASMASH
#define SOUND_OFFSET 0 //4
#else
#define SOUND_OFFSET 0
#endif

// RIO
#define RIO_TYPE_INPUT 1
#define RIO_TYPE_OUTPUT 2
#define RIO_1_IN 1
#define RIO_1_OUT 2
#define RIO_1_PANIC 3
#define RIO_1_PHOTO 4 // @@@
#define RIO_1_DISABLED 8

#define RIO_1_LIGHT 1

// IO op RIO / ethDMX
// input
#define RIO_PUMP 1
#define RIO_SENSOR_1 2 // Sensor IN
#define RIO_SENSOR_2 3 // Sensor Cone2
#define RIO_SENSOR_3 4 // Cone 2
#define RIO_SENSOR_4 5 // Cone 3
#define RIO_SENSOR_5 6 // Sensor OUT
#define RIO_SENSOR_6 7 // Fogger liquid level LOW
#define RIO_SENSOR_7 8 // Fogger liquid level HIGH
#define RIO_SENSOR_8 9 // Main fogger liquid level low
// output
#define RIO_RELAY_1 1 // Fogger liquid low



// Beamer
#define BEAMER_NO 0
/*
#define BEAMER_AQUASMASH 1
#define BEAMER_SPROOKJESBOS 2
#define BEAMER_HELIKOPTER 3
#define BEAMER_HOKIEPOKIE 4
#define BEAMER_ANIMALS 5
#define BEAMER_EPIC 6
#define BEAMER_BOOYAH 7
#define BEAMER_DRAKEN1 8
#define BEAMER_DRAKEN2 9
#define BEAMER_DRAKEN3 10
#define BEAMER_PIRATES 11
#define BEAMER_JAMESBOND 12
#define BEAMER_SPONGEBOB 13
*/


#define STATE_SLIDER_START 1
#define STATE_SLIDER_SLIDE 2
#define STATE_SLIDER_LEAVE 3

#define STATE_SLIDE_WAIT 1
#define STATE_SLIDE_BLOCK 2
#define STATE_SLIDE_PREPARE 3
#define STATE_SLIDE_GO 4

#define IO_TYPE_SOLLAE 1
#define IO_TYPE_EDX 2
#define IO_TYPE_CAMERA 3
#ifndef SENSOR_TYPE
#define SENSOR_TYPE IO_TYPE_SOLLAE
#endif