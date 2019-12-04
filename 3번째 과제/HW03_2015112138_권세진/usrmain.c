
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

#define IDLE_PRIORITY 0

// ������ ������ define�մϴ�.
#define NoColor	0
#define	Black	1
#define	Blue	2
#define	Green	3
#define Yellow	4
#define	Red		5
#define	White	6
#define	Brown	7



task_pt task_handle01;
task_pt task_handle02;

int color; // �÷��� �Է¹޾� �����ϴ� ����

/* -------------------------------------------------------------------------
Global variables
------------------------------------------------------------------------- */

static void rootfunc(void * arg);

static void taskfunc01(void * arg) { // ������ üũ�ؼ� color�� �����մϴ�.
	while (1) {
		color = ev3_sensor_get(0);
		task_sleep(50);
	}
}

static void taskfunc02(void * arg) { // ���� ���� ���͸� �����մϴ�.
	glcd_clear();
	while (1) {
		glcdGotoChar(0, 0);

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


/* -------------------------------------------------------------------------
Prototypes
------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------
Function Definitions
------------------------------------------------------------------------- */
int usrmain(int argc, char * argv[]) {
	int r;
	printf("\n\n\n\r");
	printf("\n\n\n\r");
	printf("================================\n\r");
	printf("exe_ubinos_test(build time: %s %s)\n\r", __TIME__, __DATE__);

	r = task_create(NULL, rootfunc, NULL, task_getmiddlepriority(), 256, "root");

	if (0 != r) {
		logme("fail at task_create\r\n");
	}


	ubik_comp_start();

	/* code below ubik_comp_start function never be executed */

	return 0;
}





static void rootfunc(void * arg) {
	//lcd�� ���� �ʱ�ȭ
	glcd_init();
	motor_init();

	//lcd clear
	glcd_clear();

	//�÷����� �ʱ�ȭ
	ev3_sensor_init(0, COL_COLOR);

	//�׽�ũ 2�� ����
	if (task_create(&task_handle01, taskfunc01, NULL, IDLE_PRIORITY + 2, 256, "task_01") != 0) { // ����üũ �׽�ũ
		printf("�׽�ũ ��������");
	}
	if (task_create(&task_handle02, taskfunc02, NULL, IDLE_PRIORITY + 1, 256, "task_02") != 0) { // ���� ���� �������� �׽�ũ
		printf("�׽�ũ ��������");
	}
}

