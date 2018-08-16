/*********************************************************
/ encoder.h
/ 
/
/*********************************************************/
#ifndef ENCODER_H
#define ENCODER_H

#ifdef __cplusplus
 extern "C" {
#endif

void Steering_Motor_Encoder_Init(void);
void Drive_Motor_Encoder_Init(void);
void Clear_Encoder_Count_Of_Steering_Motor(void);
void Clear_Encoder_Count_Of_Drive_Motor(void);
int32_t Get_Encoder_Count_Of_Steering_Motor(void);
int32_t Get_Encoder_Count_Of_Drive_Motor(void);
void Encoder_Init(void);


#ifdef __cplusplus
}
#endif

#endif
