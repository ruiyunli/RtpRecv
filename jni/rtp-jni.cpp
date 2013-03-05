
/*
 * RTP传输流程：
 * 1、配置信息
 * 	     包括本地端口，目标端口，目标ip，发送的数据包次数
 * 2、ip数据结构转换
 * 3、时间戳设置
 * 4、创建会话
 * 5、发送及接受数据包
 *
 */

#include <jni.h>
#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include <netinet/in.h>	//not win32
#include <arpa/inet.h>	//not win32
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

//
#ifdef __cplusplus
extern "C" {
void Java_com_example_rtprecv_MainActivity_RtpTest(JNIEnv *, jobject);
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


void Java_com_example_rtprecv_MainActivity_RtpTest(JNIEnv* env, jobject thiz)
{
 	RTPSession sess;
	uint16_t portbase,destport;
	uint32_t destip;
	std::string ipstr;
	int status,i,num;

    //1、配置信息
	//包括本地端口，目标端口，目标ip，发送的数据包次数
	portbase=LOCAL_PORT;
    ipstr=DEST_IP;
    destport=DEST_PORT;
	num=PACKET_NUM;

	//2、ip数据结构转换
	destip = inet_addr(ipstr.c_str());
	if (destip == INADDR_NONE)
	{
		LOGE("BAD IP ADDR");
		return ;
	}
	//BYTE ORDER到host byte order转换
	destip = ntohl(destip);
	
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	
	//3、时间戳设置
	sessparams.SetOwnTimestampUnit(TIME_STAMP);
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);

	//4、创建会话
	status = sess.Create(sessparams,&transparams);	
//	checkerror(status);
	RTPIPv4Address addr(destip,destport);
	status = sess.AddDestination(addr);
//	checkerror(status);

	LOGD("LocalPort:%d",portbase);
	LOGD("DestPort:%d",destport);
	LOGD(DEST_IP);
	LOGD("ready!");

	//5、发送及接受数据包
	for (i = 1 ; i <= num ; i++)
	{
		// send the packet
		status = sess.SendPacket((void *)"1234567890",10,0,false,10);

		LOGD("send packet:%d",i);

		sess.BeginDataAccess();

		// check incoming packets
		if (sess.GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket *pack;
				
				while ((pack = sess.GetNextPacket()) != NULL)
				{
					// You can examine the data here
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


	return;
}

