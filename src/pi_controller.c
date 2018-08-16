/*
 * 作者：Teruya, Suebe, Ono, Uno, Yamano
 * 最終更新日：27-October-2016
 * 目的：PI制御の計算及びPWMの設定を行う
 */
#include "pi_controller.h"
#include "stm32f4xx.h"
#include <math.h>



// ステアリングモータのPI制御の計算
double Calc_Steer_PIcontrol(double now_value, double ref_value, int onoff)
{
	double mv = 0.0; // 操作量
	static double error_sum = 0.0; // 偏差の和
	double error = 0.0; // 偏差

// onoff:1はenable, onoff:0はunable
	if (onoff == 1)
	{
		// 目標速度が0.0なら偏差の積を0にする
		if (-0.1 < ref_value && ref_value < 0.1)
		{
			error_sum = 0.0;
		}
		else
		{
			error = ref_value - now_value;
			error_sum += error;
			mv = Kp_STEER * error + Ki_STEER * error_sum;

			/* モータの公称電圧よりも大きい電圧が掛からないように，操作量の上限値を設けた．
			* 上限値3800は4095*32.0/33.6で算出した値をもとに設定した．
			* 4095はデューティ比100%に相当し，33.6はバッテリー満充電時の電圧，32.0はドライブモータの公称電圧である*/
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

// ステアリングモータのPWMを操作量mvに応じて設定する
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

// ステアリングモータ：PI制御計算及びPWMの設定を行う
void Set_Steer_Speed(double now_speed, double ref_speed, int onoff)
{
	double mv;

	if(onoff == 1)
		mv = FEED_FORWARD * ref_speed + Calc_Steer_PIcontrol(now_speed, ref_speed, onoff);
	else if(onoff == 0)
		mv = Calc_Steer_PIcontrol(now_speed, ref_speed, onoff);

	Set_Drive_PWM(mv);
}