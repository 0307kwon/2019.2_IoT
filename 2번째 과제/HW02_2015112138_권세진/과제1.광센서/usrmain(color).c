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

// 색깔을 변수로 define합니다.
#define NoColor	0
#define	Black	1
#define	Blue	2
#define	Green	3
#define Yellow	4
#define	Red		5
#define	White	6
#define	Brown	7

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
	ev3_sensor_init(0, COL_COLOR);
	//lcd clear
	glcd_clear();

	int color;

	for (;;)
	{
		glcdGotoChar(0, 0);
		/*
		// 컬러센서로 들어오는 컬러 값을 color 변수에 저장하고
		color 변수의 값에 따라 모터의 속도를 변화시킵니다.
		*/
		color = ev3_sensor_get(0);
		switch (color)
		{
		case NoColor:
			glcd_printf("No color  VAL : %3d              ", ev3_sensor_get(0));
			motor_set(0, 100);
			break;
		case Black:
			glcd_printf("Black VAL :  %3d                 ", ev3_sensor_get(0));
			motor_set(0, 200);
			break;
		case Blue:
			glcd_printf("Blue VAL :  %3d                    ", ev3_sensor_get(0));
			motor_set(0, 300);
			break;
		case Green:
			glcd_printf("Green VAL :  %3d                    ", ev3_sensor_get(0));
			motor_set(0, 400);
			break;
		case Yellow:
			glcd_printf("Yellow VAL :  %3d                    ", ev3_sensor_get(0));
			motor_set(0, 500);
			break;
		case Red:
			glcd_printf("Red VAL :  %3d                    ", ev3_sensor_get(0));
			motor_set(0, 600);
			break;
		case White:
			glcd_printf("White VAL :  %3d                    ", ev3_sensor_get(0));
			motor_set(0, 700);
			break;
		case Brown:
			glcd_printf("Brown VAL :  %3d                    ", ev3_sensor_get(0));
			motor_set(0, 800);
			break;
		default:
			glcd_printf("UNDEFINE      ");
			break;
		}
		task_sleep(50);

	}

}
