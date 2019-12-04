

/* -------------------------------------------------------------------------
Include
------------------------------------------------------------------------- */



//필요한 라이브러리들을 include 합니다.
#include "../ubiconfig.h"

// standard c library include
#include <stdio.h>
#include <sam4e.h>

// ubinos library include
#include "itf_ubinos/itf/bsp.h"
#include "itf_ubinos/itf/ubinos.h"
#include "itf_ubinos/itf/bsp_fpu.h"

// chipset driver include
#include "ioport.h"
#include "pio/pio.h"

// new estk driver include
#include "lib_new_estk_api/itf/new_estk_led.h"
#include "lib_new_estk_api/itf/new_estk_glcd.h"
#include "lib_switch/itf/lib_switch.h"
#include "lib_sensor/itf/lib_sensor.h"
#include "lib_EV3_sensor/itf/lib_EV3_sensor.h"
#include "lib_sensorcalib/itf/lib_sensorcalib.h"
#include "lib_motor_driver/itf/lib_motor_driver.h"

// custom library header file include
//#include "../../lib_default/itf/lib_default.h"
#include "../../lib_new/itf/lib_new.h"
// user header file include

/* -------------------------------------------------------------------------
Global variables
------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
Prototypes
------------------------------------------------------------------------- */
static void rootfunc(void * arg);


/* -------------------------------------------------------------------------
Function Definitions
------------------------------------------------------------------------- */
int usrmain(int argc, char * argv[]) {
	int r;

	printf("\n\n\n\r");
	printf("================================================================================\n\r");
	printf("exe_ubinos_test (build time: %s %s)\n\r", __TIME__, __DATE__);
	printf("================================================================================\n\r");

	r = task_create(NULL, rootfunc, NULL, task_getmiddlepriority(), 256, "root");
	if (0 != r) {
		logme("fail at task_create\r\n");
	}

	ubik_comp_start();

	return 0;
}

static void rootfunc(void * arg) {
	//lcd 초기화
	glcd_init();

	//모터 초기화
	motor_init();

	//밝기센서 값 초기화
	ev3_sensor_init(0, COL_REFLECT);

	//컬러센서를 보정하기 위한 밝기 센서 값을 먼저 calibration해야합니다.
	calibEV3Sensor(0, MAX_LIGHT_LEVEL, light_value);

	//lcd clear
	glcd_clear();
	//첫줄부터 둘째줄까지 DARK값 표시
	glcdGotoChar(0, 1);
	glcd_printf("STATE[LIGHT_DARK] \nVAL : %d   ", light_value[0]);
	//셋째줄부터 넷째줄까지 DIM값 표시
	glcdGotoChar(0, 3);
	glcd_printf("STATE[LIGHT_DIM] \nVAL : %d   ", light_value[1]);
	//다섯줄부터 여섯줄까지 BRIGHT값 표시
	glcdGotoChar(0, 5);
	glcd_printf("STATE[LIGHT_BRIGHT] \nVAL : %d   ", light_value[2]);


}
