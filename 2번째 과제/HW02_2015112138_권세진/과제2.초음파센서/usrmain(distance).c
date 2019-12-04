/*
Copyright (C) 2009 Sung Ho Park
Contact: ubinos.org@gmail.com

This file is part of the exe_helloworld component of the Ubinos.

GNU General Public License Usage
This file may be used under the terms of the GNU
General Public License version 3.0 as published by the Free Software
Foundation and appearing in the file license_gpl3.txt included in the
packaging of this file. Please review the following information to
ensure the GNU General Public License version 3.0 requirements will be
met: http://www.gnu.org/copyleft/gpl.html.

GNU Lesser General Public License Usage
Alternatively, this file may be used under the terms of the GNU Lesser
General Public License version 2.1 as published by the Free Software
Foundation and appearing in the file license_lgpl.txt included in the
packaging of this file. Please review the following information to
ensure the GNU Lesser General Public License version 2.1 requirements
will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.

Commercial Usage
Alternatively, licensees holding valid commercial licenses may
use this file in accordance with the commercial license agreement
provided with the software or, alternatively, in accordance with the
terms contained in a written agreement between you and rightful owner.
*/

/* -------------------------------------------------------------------------
Include
------------------------------------------------------------------------- */
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
	int dis;

	//lcd 초기화
	glcd_init();
	//초음파센서 초기화
	sensor_init(NXT_DIGITAL_SENSOR_SONA, 0, 0, 0);

	//모터 초기화
	motor_init();

	//distance를 4단계로 calibration
	calibSensor(0, MAX_DIS_LEVEL, dis_value);
	/*헤더파일에 DIS_VERYCLOSE, DIS_CLOSE, DIS_FAR, DIS_VERYFAR
	4단계로 미리 정의
		*/			 
				
	glcd_clear();

	for (;;)
	{
		dis = sensor_get(0); // dis 변수에 0포트에서 들어오는 distance 값을 저장합니다.
		glcdGotoChar(0, 0);
		/*check sensor level */
		// dis변수를 바탕으로 level을 체크하여 level에 따라 출력값과 모터속도를 다르게 출력
		//DIS_VERYCLOSE,
		//DIS_CLOSE,
		//DIS_FAR 
		//DIS_VERYFAR 4가지 종류로 거리를 구분합니다.
		switch (get_level(dis, MAX_DIS_LEVEL, dis_value))
		{
		case DIS_VERYCLOSE:
			glcd_printf("very close  VAL : %3d              ", dis);
			motor_set(0, 250);
			break;
		case DIS_CLOSE:
			glcd_printf("close VAL :  %3d           		", dis);
			motor_set(0, 500);
			break;
		case DIS_FAR:
			glcd_printf("far VAL :  %3d              		", dis);
			motor_set(0, 750);
			break;
		case DIS_VERYFAR:
			glcd_printf("very far VAL :  %3d                  ", dis);
			motor_set(0, 1000);
			break;
		default:
			glcd_printf("UNDEFINE		");
			break;
		}
		task_sleep(30); // sensing 주기를 30ms로 합니다.
	}


}




