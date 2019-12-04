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
sem_pt _g_sem; //api�� �������� ������ �� ����
int count = 0;
#define TIMER_STOP 0 //sw1��  ����������
#define TIMER_RUN 1 //sw1�̴��������� ������

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

static void rootfunc(void *arg) { // �׻� ���� �ִ� �Լ�

	switch_init(switch_isr, switch_isr2);
	glcd_init();

	//�⺻ lcd ǥ��
	glcdGotoChar(0, 0);
	glcd_printf("SEC : %3d", count);

	for (;;) {
		sem_take(_g_sem); // ������ ������(��ٸ�), 1�ʸ��� glcd_printf �մϴ�.

		glcdGotoChar(0, 0);
		glcd_printf("SEC : %3d", count);

		task_sleep(100);
	}

}


static void timer_isr(void) { // 1�ʸ��� �ѹ��� ���ͷ�Ʈ ��
	unsigned int reg;

	sem_give(_g_sem); //������ �ڿ��� ��
	if (g_state == TIMER_STOP) { // �ʱⰪ�� TIMER_RUN�̱� ������ �ʱ� ���´� Ÿ�̸Ӱ� ������������
		count++;

		reg = TC1->TC_CHANNEL[0].TC_SR;
		printf("HW_TIMER [TC:%d] \r\n", reg);

	}

}

static void switch_isr(void) { // SW1 ����ġ �������� ���ͷ�Ʈ �Լ�
	if (g_state == TIMER_STOP) //ó�� ���� �� ����ġ�� Ȧ���� �������� ��
	{
		g_state = TIMER_RUN; //���� ����ġ�� �������� ���ͷ�Ʈ�� �߻��Ͽ� switch_isr�Լ��� ������ �ǰ� �̸� ������ �ٲ���
	}
	else if (g_state == TIMER_RUN)
	{
		g_state = TIMER_STOP;
	}
}

static void switch_isr2(void) { // SW2 ����ġ �������� ���ͷ�Ʈ �Լ�
	count = 0; //SW2�� �������� �ʱ�ȭ
	glcdGotoChar(0, 0);
	glcd_printf("SEC : %3d", count);
}


