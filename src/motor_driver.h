// motor_driver.h

#ifdef __cplusplus
extern "C"{
#endif

// void Drive_Motor_Driver_Init(void);  // ドライブモータのドライバに関するピンの設定
void Steering_Motor_Driver_Init(void);  // ステアリングモータのドライバに関するピンの設定
void Motor_Driver_Init();  // ステアリングモータとドライブモータのドライバの設定

#ifdef __cplusplus
}
#endif
