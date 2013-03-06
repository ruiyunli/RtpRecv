
/*
 *****************************************
 * RTP�������̣�                                                                    *
 * 1��������Ϣ							 *
 * 	     �������ض˿ڣ�Ŀ��˿ڣ�Ŀ��ip��             *
 * 	     �Լ����͵����ݰ�����				 *
 * 2��ip���ݽṹת��						 *
 * 3��ʱ�������							 *
 * 4�������Ự							 *
 * 5�����ͼ��������ݰ�					 *
 *****************************************
 *
 ******************************************
 * Send:  	192.168.0.30--->192.168.0.20  *
 * 			10000--->12000				  *
 * Recv:	192.168.0.20--->192.168.0.30  *
 * 			12000--->10000				  *
 ******************************************
 *
 */

#include <jni.h>
#include "rtpsession.h"				//������rtpsession��һЩʵ��
#include "rtpudpv4transmitter.h"	//������RTPSession�ĵڶ���������
#include "rtpipv4address.h"			//������rtpipv4address
#include "rtpsessionparams.h"		//������rtpsession�ĵ�һ��������
#include "rtperrors.h"				//������RTP�еĴ�����Ϣ
#include <netinet/in.h>	//not win32
#include <arpa/inet.h>	//not win32
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

//
#ifdef __cplusplus
extern "C" {
int Java_com_example_rtprecv_MainActivity_RtpTest(JNIEnv *, jobject);
}
#endif

/**** ����̨��� ****/
#include <android/log.h>
#define  LOG_TAG "RTP_JNI"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)


//������Ϣ
#define LOCAL_PORT 12000
#define DEST_PORT 10000
#define DEST_IP "192.168.0.30"
#define PACKET_NUM 10
#define TIME_STAMP 1.0/10.0


int Java_com_example_rtprecv_MainActivity_RtpTest(JNIEnv* env, jobject thiz)
{
 	RTPSession sess;	//RTPSession����ʵ�����˴ε�RTP�Ự
	uint16_t portbase;
	int status,i,num,recvCount;

    //1��������Ϣ
	//�������ض˿�
	portbase=LOCAL_PORT;
	num=PACKET_NUM;
	
	//���洴��һ��RTP�Ự�����ʹ�������ݰ�

	//����rtpsession�ĵڶ��������࣬���ĳ�Ա���������趨�����˿�
	RTPUDPv4TransmissionParams transparams;

	//����rtpsession�ĵ�һ�������࣬���ĳ�Ա������������ǡ����ʱ����Ԫ
	RTPSessionParams sessparams;
	
	//3��ʱ�������
	//����ǡ����ʱ����Ԫ��ÿ��������Ҫ����10�Σ��ʲ���Ϊ1.0/10
	sessparams.SetOwnTimestampUnit(TIME_STAMP);
	//���������ǲ��ǽ��������Զ�������ݰ�������ѡ��
	sessparams.SetAcceptOwnPackets(true);
	//���ñ����˿�
	transparams.SetPortbase(portbase);

	//4�������Ự
	status = sess.Create(sessparams,&transparams);

	LOGD("LocalPort:%d",portbase);
	LOGD("ready");

	//5�����ͼ��������ݰ�
	recvCount = 0;
	for (i = 1 ; i <= num ; i++)
	{
		//Ϊ�˱�֤source table������Դ�б�����Ķ�������GotoFirstSourceWithData�Ⱥ���
		//��ִ�б���Ҫ��BeginDataAccess��EndDataAccess֮�䣬������⺯������
		sess.BeginDataAccess();

		//�յ��ı��ģ���������Я�����ݵ�Դ����Ϊһ��rtp�Ự�����ж�������ߣ�Դ����
		if (sess.GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket *pack;
				
				while ((pack = sess.GetNextPacket()) != NULL)
				{
					// You can examine the data here
					recvCount++;
					LOGD("!!!!!Got packet!!!!!");
					// we don't longer need the packet, so we'll delete it
					sess.DeletePacket(pack);
				}
			} while (sess.GotoNextSourceWithData());
		}
		
		sess.EndDataAccess();

#ifndef RTP_SUPPORT_THREAD
		status = sess.Poll();
//		checkerror(status);
#endif // RTP_SUPPORT_THREAD
		
		RTPTime::Wait(RTPTime(1,0));
	}
	
	sess.BYEDestroy(RTPTime(10,0),0,0);


	return recvCount;
}


