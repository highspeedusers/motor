/*
 * pi_controller.h
 *
 *  Created on: 2016/07/04
 *      Author: user
 */

#ifndef SRC_PI_CONTROLLER_H_
#define SRC_PI_CONTROLLER_H_

#ifdef __cplusplus
 extern "C" {
#endif

// �X�e�A�����O���[�^�̃Q�C��
#define FEED_FORWARD 52.0 // ���[�^�̌��̓d���C�����׉�]�����Q�l�Ɍ��߂�
#define Kp_STEER 2.4
#define Ki_STEER 1.8


double Calc_Steer_PIcontrol(double now_value, double ref_value, int onoff);
void Set_Steer_PWM(double mv);
void Set_Steer_Speed(double now_speed, double ref_speed, int onoff);

#ifdef __cplusplus
}
#endif

#endif /* SRC_PI_CONTROLLER_H_ */
