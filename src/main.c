/********************************************************************************
   　ファイルの場所   Project/STM32F4xx_StdPeriph_Templates/main.c
  作者  Teruya, Suebe, Ono, Uno, Yamano
  編集　Kawada
  最終更新日    27-October-2016
  目的　frontierにおいてhighspeedで走行する単輪試験装置のモーター制御


     DIO1-3 PB10 TX
 	 DIO1-4 PB11 RX
	 DIO1-5 GND

  �g����   �ڂ����͎d�l�����Q�Ƃ���D
                   �W���C�X�e�B�b�N�̓��͂ɉ����ĉ��L�̓�������s����D
                  �Z�F����ł����ԂɃX�e�A��؂�
       ���F������Ԃɖ߂�
       L�P + ���X�e�B�b�N�i���E�����j�F�X�e�A���}90deg�͈̔͂Ő؂�
       �ʏ탂�[�h�FR1 + �E�X�e�B�b�N�i�㉺�����j�F�ԗւ��}5cm/s�͈̔͂ŉ�
       ���񃂁[�h�FR1 + �E�X�e�B�b�N�i�㉺�����j�F�ԗւ��}5cm/s�͈̔͂ŉ񂵁C���񂷂�

*******************************************************************************/

#include "main.h"
#include "stm32f4xx.h"
#include "led.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "usart.h"
#include "common.h"
#include "motor_driver.h"
#include "encoder.h"
#include "pi_controller.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
__IO uint16_t CCR1_Val = 333;
__IO uint16_t CCR2_Val = 249;

/* Private variables for timer*/
static __IO uint32_t nTimeDelay;
static __IO uint32_t cnt_ms = 0;
uint32_t uCountFreq;
uint32_t mCountFreq;
RCC_ClocksTypeDef RCC_Clocks;

void SysTickTimer_Config()
{
    RCC_GetClocksFreq(&RCC_Clocks);
    mCountFreq = RCC_Clocks.HCLK_Frequency/1000;
    uCountFreq = RCC_Clocks.HCLK_Frequency/1000000;
    //SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    if(SysTick_Config(mCountFreq)) //core_cm4.h
    {
      ;//  GPIO_SetBits(GPIOD,GPIO_Pin_14);//ERROR
    }
}

GPIO_InitTypeDef  GPIO_InitStructure;

uint16_t ReadADC3(uint8_t channel);
void ADC3_CH9_Config(void);

#define ENC_CNT_1R 500 // モーター1周のエンコーダカウント数
#define STEER_GEAR_RATIO 83 // ギア比
#define STEER_ENC_CNT 41500 // 車輪1回転のエンコーダカウント数
#define PI 3.141592

int main(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	int boudrate = 115200; //ボーレート
    int bufsize  = 1024; // バッファサイズ
    USART_Config(USART_CH3, boudrate, bufsize); // USART�̐ݒ�

    // �}�C�R���̓���J�n��[����ɂ��m�点����
    char str[256];
    sprintf(str," start\r\n");
	STM32_puts(str);

	SysTickTimer_Config();
	ADC3_CH9_Config();

	RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 2);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* �J�E���^�[ */
    int cnt = 0; // tera term�Ƀ}�C�R���̃f�[�^���o�͂���^�C�~���O�����߂�J�E���^�[
    uint32_t prev_cnt = 0; // PI������s�������肷��ۂɗp����ϐ��Dcnt_ms-prev_cnt>delta_time��PI������s
    int LED_cnt = 0;

    /* ���[�^��OnOff�X�C�b�` 1:ON 0:OFF */
	int steer_enable = 1;

	/* エンコーダ、速度用変数� */
	int32_t	steer_enc_cnt = 0; // モーターエンコーダのカウント数
    int32_t	prev_steer_enc_cnt = 0; // 前のループ処理のモーターのエンコーダカウント数
	double now_speed = 0.0; // 車輪の回転速度[cm/s]
	double ref_speed = 0.0; // 車輪の目標速度[cm/s]
	double wheel_rotation = 0.0; //ホイールの回転速度[rad/s]
	int steer_speed = 0; // ステア回転速度[cnt/s]←よくわからん

	/* 電流、電圧値読み取り用の変数　 */
	int steer_voltage = 0; // モーターの電流センサーの出力電圧[mV]
	int steer_current = 0; // モーターの電流[mA]
	int steer_ADC[50] = {0}; // モータの電流値はそれに対応する電圧にAD変換される。その値を代入する配列
	int total_steer_ADC = 0; // モーターの電流値を電圧値にAD変換した値の総和、電流値の算出に用いる
	int limit_cnt_steer = 0; // モータに電流制限をかけるかどうか決定する指標値
	int i = 0; //　電流センサの電圧を読み取り、その値を格納する配列の要素番号
	//　int j=0 //電圧値の総和計算で用いるカウンタ

	/* joystickからの目標値読み取り用変数 */
	char data[28] = {0};  // �o�b�t�@������o�����f�[�^�ۑ��p
	char data2[28] = {0};
	int DATA;  // �o�b�t�@���̃f�[�^�T�C�Y
	/* ���ꂼ��̃{�^���̏����i�[����ϐ� */
	char *str_ref_speed;
	char *str_ref_angle;
	char *circle_button;
	char *square_button;
	char *L1_button;
	char *R1_button;
	char *space;
	int mode = 0; // ���񃂁[�h���ǂ����𔻒f���邽�߂̕ϐ� 0:���񃂁[�h�łȂ� 1:���񃂁[�h

	SystemInit();
	Encoder_Init();  // エンコーダ設定
	Motor_Driver_Init();  // モータードライバを使えるようにする
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);  // ドライブモータの有効化

	// マイコンの生存確認用にLEDを使う、下のwhile文で点滅する
    LED_Init(LED1);

    // PWMの初期化
	Set_Steer_PWM(0.0);

	while (1)
	{
		DATA = USART_GetReceivedDataSize(USART_CH3);

		// データが15文字以上バッファにあるならデータを読む。1文字以上としたときは上手くいかなかったのでこうしている
		if(1)   // DATA > 15)
		{
			/* どのようなデータを受信しているのかは、ros/src/dora_joy/src/joy_listen_node.cpp　を参照 */
//			USART_GetReceivedData(USART_CH3, data, 0);  // dataにバッファの文字列を入れる
//			STM32_puts(data);
			get_data(data, 1);
//			STM32_puts(data);



//			space = strtok(data, " "); // 取得した文字列は必ずしも欲しい情報の先頭とは限らないので、先頭のスペースまでデータを捨てる
			//受信したデータをそれぞれの変数に格納
			str_ref_speed = strtok(data, ",");
			str_ref_angle = strtok(NULL, ",");
			circle_button = strtok(NULL, ",");
			square_button = strtok(NULL, ",");
			L1_button = strtok(NULL, ",");
			R1_button = strtok(NULL, ",");

			// �Z�{�^���Ő��񃂁[�h�ɐ؂�ւ��
			if(*circle_button == '1')
			{
				ref_speed = 0.0;
				ref_angle = 51.94; // �ԗ�A,D 51.94 B,C -59.14
				mode = 1;

			}
			// ���{�^���ŏ�����Ԃɖ߂�
			else if(*square_button == '1')
			{
				ref_speed = 0.0;
				ref_angle = 0.0;
				mode = 0;
			}
			// ���񃂁[�h����R1�{�^���������Ȃ���E�X�e�B�b�N���㉺�ɓ������Ɛ��񂷂�
			// �ԗ�A,D 51.7 < now_angle && now_angle < 52.2
			// �ԗ�B,C -52.2 < now_angle && now_angle < -51.7
			else if(mode == 1 && *R1_button == '1' && 51.7 < now_angle && now_angle < 52.2)
			{
				ref_speed = atof(str_ref_speed);
			}
			// �ʏ탂�[�h���ɁCR1�{�^���������Ȃ���E�X�e�B�b�N���㉺�ɓ������Ǝԗւ���]����
			else if(mode == 0 && *R1_button == '1')
			{
				// �ԗ�A,B -atof(str_ref_speed) �ԗ�C,D atof(str_ref_speed)
				ref_speed = -atof(str_ref_speed);
			}
			// �ʏ탂�[�h����L1�{�^���������Ȃ���E�X�e�B�b�N�����E�ɓ������ƃX�e�A��؂�
			else if(mode == 0 && *L1_button == '1')
			{
				ref_angle = atof(str_ref_angle);
			}
		}


		// モータの電流センサの出力を読み取り、配列にその値を格納する
		total_steer_ADC-=steer_ADC[i];
		// 電流算出のために、AD変換した値の総和を求める
		steer_ADC[i] = ReadADC3(ADC_Channel_9);
		total_steer_ADC += steer_ADC[i];
		++i;
		// ADポートから得た値の総和から電流[mA]を求める
		/* ***************************************************************
		 * �Z�o�ɗp�������̓��[�^�h���C�o�̃f�[�^�V�[�g���Q�l�ɂ����C�v�L�����u���[�V����          *
		 * �ԗ�A �F (2472,121)�@and (203,196)  B : (2470,121)�@and (219,196) *
		 * �ԗ�C : (2485,121)�@and (270,196) D : (2540,121)�@and (275,196) *
		 * ***************************************************************/
		steer_voltage = (total_steer_ADC << 4) / 994; // 3300 * (total_steer_ADC / 10) / 4096
		steer_current = ((steer_voltage - 2472) * 121) >> 4; // (steer_voltage - 2.48) / 0.066 * 0.5
		drive_current = ((drive_voltage - 203) * 196) >> 4;  // (drive_voltage - 0.27) / 0.01 / 19.0 * 1.1

		if(i > 49)
			i = 0;

		// 駆動用モータの電流制限判定
		/*
		if(drive_current > 4000)
		{
			++limit_cnt_drive;
			if(limit_cnt_drive > 10)
			{
				drive_enable = 0;
			}
		}
		else
		{
			limit_cnt_drive = 0;
			drive_enable = 1;
		}
		// �X�e�A�����O���[�^�̓d����������
		if(fabs(steer_current) > 400)
		{
			++limit_cnt_steer;
			if(limit_cnt_steer > 10)
			{
				steer_enable = 0;
			}
		}
		else
		{
			limit_cnt_steer = 0;
			steer_enable = 1;
		}
		*/

		// 10[ms] ごとにPI制御を実行する
		if((cnt_ms - prev_cnt) > 10)
		{
			// エンコーダの値を取得
			prev_steer_enc_cnt = steer_enc_cnt;
			steer_enc_cnt = Get_Encoder_Count_Of_Steering_Motor();

			// 車輪速度の算出[cm/s]車輪の円周:56.2345[cm]
			wheel_rotation = 2 * PI * ((double)drive_enc_cnt - prev_drive_enc_cnt) / DRIVE_ENC_CNT / ((cnt_ms - prev_cnt) / 1000.0);
			now_speed = wheel_rotation * 9.0; // (drive_enc_cnt - prev_drive_enc_cnt) / DRIVE_ENC_CNT * 56.2345 / ((cnt_ms - prev_cnt) / 1000.0)
			

			// PI制御の計算を行い、PWMをセットする
			Set_Steer_Speed(now_speed, ref_speed);

			if(cnt > 20)
			{
				// 画面出力:車輪の目標回転速度、現在の回転速度、ステアのモーター電流、ステアのセンサ電圧
				sprintf(str,"%d %3.2f %3.1f %3.1f %d %d \r\n",mode,  wheel_rotation, ref_speed, now_speed, steer_current, steer_voltage,);
				STM32_puts(str);

				cnt = 0;

				// マイコンの動作確認のため、LEDを常時点滅させる
				if(LED_cnt < 5)
				{
					LED_On(LED1);
				}
				else if(LED_cnt < 10)
				{
					LED_Off(LED1);
				}
				else
				{
					LED_cnt = -1;
				}
				++LED_cnt;
			}
			++cnt;
		}
	}
}

/* SysTickTimerがオーバーフローしたときに呼ばれる割込みハンドラ　*/
//extern __IO uint32_t nTimeDelay;

void SysTick_Handler(void)
{
    if(nTimeDelay != 0x00)
    {
    	nTimeDelay--;
    }
    cnt_ms ++;
}

//　以下はADポートの設定
void ADC3_CH9_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  /* Enable ADC3 and GPIO clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  /* Configure ADC3 Channel9 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC3 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
//  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
//  ADC_InitStructure.ADC_ContinuousConvMode =ENABLE ;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode =DISABLE ;
  //Specifies whether the conversion is performed in Continuous(ENABLE) or Single(DISABLE) mode.
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC3, &ADC_InitStructure);
//Initializes the ADCx peripheral according to the specified parameters in the ADC_InitStruct

  /* ADC3 regular channel9 configuration *************************************/
//  ADC_RegularChannelConfig(ADC3, ADC_Channel_9, 1, ADC_SampleTime_3Cycles);

  /* Enable ADC3 */
  ADC_Cmd(ADC3, ENABLE);

}

void ADC1_CH0_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  /* Enable ADC1 and GPIO clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Configure ADC1 Channel0 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  ADC_DeInit();

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC3 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode =ENABLE ;
  //Specifies whether the conversion is performed in Continuous(ENABLE) or Single(DISABLE) mode.
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
//Initializes the ADCx peripheral according to the specified parameters in the ADC_InitStruct

  /* ADC1 regular channel9 configuration *************************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);


  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

}

uint16_t ReadADC3(uint8_t channel)
{
  ADC_RegularChannelConfig(ADC3, channel, 1, ADC_SampleTime_3Cycles);
  // Start the conversion
  //ADC_SoftwareStartConvCmd(ADC3, ENABLE);
  ADC_SoftwareStartConv(ADC3);//Enables the selected ADC software start conversion of the regular channels.
  // Wait until conversion completion
  while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  return ADC_GetConversionValue(ADC3);
}
