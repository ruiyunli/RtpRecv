
/*
 *****************************************
 * RTP传输流程：                                                                    *
 * 1、配置信息							 *
 * 	     包括本地端口，目标端口，目标ip，             *
 * 	     以及发送的数据包次数				 *
 * 2、ip数据结构转换						 *
 * 3、时间戳设置							 *
 * 4、创建会话							 *
 * 5、发送及接受数据包					 *
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
#include "rtpsession.h"				//定义了rtpsession的一些实现
#include "rtpudpv4transmitter.h"	//定义了RTPSession的第二个参数类
#include "rtpipv4address.h"			//定义了rtpipv4address
#include "rtpsessionparams.h"		//定义了rtpsession的第一个参数类
#include "rtperrors.h"				//定义了RTP中的错误信息
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

/**** 控制台输出 ****/
#include <android/log.h>
#define  LOG_TAG "RTP_JNI"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)


//配置信息
#define LOCAL_PORT 12000
#define DEST_PORT 10000
#define DEST_IP "192.168.0.30"
#define PACKET_NUM 10
#define TIME_STAMP 1.0/10.0


int Java_com_example_rtprecv_MainActivity_RtpTest(JNIEnv* env, jobject thiz)
{
 	RTPSession sess;	//RTPSession类来实例化此次的RTP会话
	uint16_t portbase;
	int status,i,num,recvCount;

    //1、配置信息
	//包括本地端口
	portbase=LOCAL_PORT;
	num=PACKET_NUM;
	
	//下面创建一个RTP会话，发送传入的数据包

	//这是rtpsession的第二个参数类，他的成员函数可以设定监听端口
	RTPUDPv4TransmissionParams transparams;

	//这是rtpsession的第一个参数类，他的成员函数可以设置恰当的时戳单元
	RTPSessionParams sessparams;
	
	//3、时间戳设置
	//设置恰当的时戳单元，每秒我们需要发送10次，故参数为1.0/10
	sessparams.SetOwnTimestampUnit(TIME_STAMP);
	//下面设置是不是接收我们自定义的数据包，这里选是
	sessparams.SetAcceptOwnPackets(true);
	//设置本机端口
	transparams.SetPortbase(portbase);

	//4、创建会话
	status = sess.Create(sessparams,&transparams);

	LOGD("LocalPort:%d",portbase);
	LOGD("ready");

	//5、发送及接受数据包
	recvCount = 0;
	for (i = 1 ; i <= num ; i++)
	{
		//为了保证source table（数据源列表）不会改动，类似GotoFirstSourceWithData等函数
		//的执行必须要在BeginDataAccess和EndDataAccess之间，详情见库函数声明
		sess.BeginDataAccess();

		//收到的报文，遍历所有携带数据的源（因为一个rtp会话允许有多个参与者（源））
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


