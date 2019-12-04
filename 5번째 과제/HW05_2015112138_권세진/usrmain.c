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


#include "lib_new_estk_api/itf/new_estk_led.h"
#include "lib_new_estk_api/itf/new_estk_glcd.h"
#include "lib_motor_driver/itf/lib_motor_driver.h"
#include "lib_switch/itf/lib_switch.h"
#include "itf_ubinos/itf/bsp_intr.h"


//chipset driver include
#include "ioport.h"
#include "pio/pio.h"

/*global variables */
sem_pt _g_sem; //api용 세마포어 포인터 형 정의
int count = 0;
#define TIMER_STOP 0 //sw1이  눌러졌을때
#define TIMER_RUN 1 //sw1이눌러져있지 않을때

static char g_state = TIMER_RUN;

/* ----------------
* PROTOTYPES
* */
static void rootfunc(void *arg);
static void switch_isr(void);
static void switch_isr2(void);
static void timer_isr(void);
//static void print_lcd(void *arg);


/*function Definitions */
int usrmain(int argc, char * argv[]) {
	int r;
	printf("\n\n\n\r");
	printf("==========");
	printf("exe_intr_test(build time: %s%s) \n\r", __TIME__, __DATE__);

	//r = task_create(NULL, print_lcd, NULL, task_getmiddlepriority(), 256, "print_lcd");
	r = task_create(NULL, rootfunc, NULL, task_getmiddlepriority(), 256, "root");
	if (0 != r) {
		logme("fail at task_create\r\n");
	}

	PMC->PMC_PCER0 = 1 << ID_TC3;

	/*Timer Configuration */
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS; //counter clock disable command
	TC1->TC_CHANNEL[0].TC_IDR = 0xFFFFFFFF;   //timer interrupt disable command

											  /*Channel Mode (TCLK1 Select(0) and RC Compare Trigger Enable */
	TC1->TC_CHANNEL[0].TC_CMR = (TC_CMR_TCCLKS_TIMER_CLOCK5 | TC_CMR_CPCTRG);
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN;   //counter clock enable command
	TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;   //RC compare interrupt enable

											   /*interrupt handler */
	intr_connectisr(TC3_IRQn, timer_isr, 0x40, INTR_OPT__LEVEL);

	/*enable interrupt*/
	intr_enable(TC3_IRQn);   // NVIC interrupt enable command register

							 /*Timer Interrupt period configuration */
	TC1->TC_CHANNEL[0].TC_RC = 32768;   //set timer for about 1s interval

										/*Start timer*/
	TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG; //software trigger is performed

	ubik_comp_start();

	return 0;
}

static void rootfunc(void *arg) { // 항상 돌고 있는 함수

	switch_init(switch_isr, switch_isr2);
	glcd_init();

	//기본 lcd 표시
	glcdGotoChar(0, 0);
	glcd_printf("SEC : %3d", count);

	for (;;) {
		sem_take(_g_sem); // 세마포 가져감(기다림), 1초마다 glcd_printf 합니다.

		glcdGotoChar(0, 0);
		glcd_printf("SEC : %3d", count);

		task_sleep(100);
	}

}


static void timer_isr(void) { // 1초마다 한번씩 인터럽트 함
	unsigned int reg;

	sem_give(_g_sem); //세마포 자원을 줌
	if (g_state == TIMER_STOP) { // 초기값이 TIMER_RUN이기 때문에 초기 상태는 타이머가 동작하지않음
		count++;

		reg = TC1->TC_CHANNEL[0].TC_SR;
		printf("HW_TIMER [TC:%d] \r\n", reg);

	}

}

static void switch_isr(void) { // SW1 스위치 눌렀을때 인터럽트 함수
	if (g_state == TIMER_STOP) //처음 시작 및 스위치가 홀수로 눌러졌을 때
	{
		g_state = TIMER_RUN; //다음 스위치가 눌러지면 인터럽트가 발생하여 switch_isr함수로 들어오게 되고 미리 변수를 바꿔줌
	}
	else if (g_state == TIMER_RUN)
	{
		g_state = TIMER_STOP;
	}
}

static void switch_isr2(void) { // SW2 스위치 눌렀을때 인터럽트 함수
	count = 0; //SW2가 눌러지면 초기화
	glcdGotoChar(0, 0);
	glcd_printf("SEC : %3d", count);
}


