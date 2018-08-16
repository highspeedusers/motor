/*
 * ��ҁFTeruya, Suebe, Ono, Uno, Yamano
 * �ŏI�X�V���F27-October-2016
 * �ړI�FPI����̌v�Z�y��PWM�̐ݒ���s��
 */
#include "pi_controller.h"
#include "stm32f4xx.h"
#include <math.h>



// �X�e�A�����O���[�^��PI����̌v�Z
double Calc_Steer_PIcontrol(double now_value, double ref_value, int onoff)
{
	double mv = 0.0; // �����
	static double error_sum = 0.0; // �΍��̘a
	double error = 0.0; // �΍�

// onoff:1��enable, onoff:0��unable
	if (onoff == 1)
	{
		// �ڕW���x��0.0�Ȃ�΍��̐ς�0�ɂ���
		if (-0.1 < ref_value && ref_value < 0.1)
		{
			error_sum = 0.0;
		}
		else
		{
			error = ref_value - now_value;
			error_sum += error;
			mv = Kp_STEER * error + Ki_STEER * error_sum;

			/* ���[�^�̌��̓d�������傫���d�����|����Ȃ��悤�ɁC����ʂ̏���l��݂����D
			* ����l3800��4095*32.0/33.6�ŎZ�o�����l�����Ƃɐݒ肵���D
			* 4095�̓f���[�e�B��100%�ɑ������C33.6�̓o�b�e���[���[�d���̓d���C32.0�̓h���C�u���[�^�̌��̓d���ł���*/
			if (mv > 3800)
				mv = 3800;
			else if (mv < -3800)
				mv = -3800;
		}
	}
	else if (onoff == 0)
	{
		error_sum = 0.0;
	}
	return mv;
}

// �X�e�A�����O���[�^��PWM�𑀍��mv�ɉ����Đݒ肷��
void Set_Steer_PWM(double mv)
{
	if(mv<0){
  	  	TIM_SetCompare1(TIM3,(u16)((double)(-mv *665)/0xfff));
		GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	}else if(mv>=0){
  	  	TIM_SetCompare1(TIM3,(u16)((double)(mv *665)/0xfff));
		GPIO_SetBits(GPIOD,GPIO_Pin_12);
	}
}

// �X�e�A�����O���[�^�FPI����v�Z�y��PWM�̐ݒ���s��
void Set_Steer_Speed(double now_speed, double ref_speed, int onoff)
{
	double mv;

	if(onoff == 1)
		mv = FEED_FORWARD * ref_speed + Calc_Steer_PIcontrol(now_speed, ref_speed, onoff);
	else if(onoff == 0)
		mv = Calc_Steer_PIcontrol(now_speed, ref_speed, onoff);

	Set_Drive_PWM(mv);
}