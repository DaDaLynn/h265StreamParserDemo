#include "StreamReceiver.h"
#include "h265_transport/Release/includes/client.hpp"
//#include "../../h265StreamParser.h"

#include <android/log.h>
#include <atomic>

#define TAG "njbas"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
using namespace std;


//FILE * pRecFile = NULL;

class BitsQueue{
	public:
	
	void push(void* pdata, int len){
		unique_lock<mutex> m(m_mtx);
		vector<char> t((char*)pdata, (char*)pdata+len);
		bits.push(t);
	}
	
	vector<char> pop(){
		vector<char> out;
		unique_lock<mutex> m(m_mtx);
		if (bits.size() > 0) {
			out = move(bits.front());
			bits.pop();
		}
		return out;
	}
	
	int size(){
		unique_lock<mutex> m(m_mtx);
		return bits.size();
	}
	
	int frontBitLen()
	{
		unique_lock<mutex> m(m_mtx);
		if (bits.size() > 0) {
			return bits.front().size();
		}
		return -1;
	}
	
	void clear()
	{		
		unique_lock<mutex> m(m_mtx);
		queue<vector<char>> temp;
		swap(temp, bits);
	}
	
	private:
	
	queue<vector<char>> bits;
	mutex m_mtx;
};

BitsQueue bitqueue;

struct alarm_horn{
    int ah_no{0}; //1~3报警喇叭继电器序号，4~5：LED1/手环触发LED2
    int ah_v{0};  //0:断开；1：闭合
	
	alarm_horn(int _no = 0, int _v = 0) : ah_no(_no), ah_v(_v){}
};

struct rv_param{
    float lane_width{0};  //车道宽
    float near_field_bound{0}; //近场边界
    float near_field_speed_limit{0}; //近场限速
    float far_field_bound{0}; //远场边界
    float far_field_speed_limit{0}; //远场限速
	
	rv_param(float _w = 0, float _nb = 0, float _nl = 0, float _fb = 0, float _fl = 0) {
        lane_width = _w;
        near_field_bound = _nb;
        near_field_speed_limit = _nl;
        far_field_bound = _fb;
        far_field_speed_limit = _fl;
    }
};

atomic<int> frm(0);
atomic<int> video_receiving(0);
atomic<int> idr_received(0);
void DataProcCb(void* pData, int data_len, bool bIdr, void* pUserData){
    //LOGD("send data len:%d", data_len);
	/*if(pRecFile)
	{
		LOGD("write len:%d", fwrite(pData, 1, data_len, pRecFile));
	}*/
	
	if(video_receiving)
	{
		if(bIdr)
			idr_received = 1;
		
		if(idr_received){
			//LOGD("receive frm: %d len:%d", ++frm, data_len);
			bitqueue.push(pData, data_len);
		}
	}
		
	//bitqueue.push(pData, data_len);
}

/*class FakeClient
{
public:

	~FakeClient()
	{
		if(mydo)
			stop();
	}
    int init()
    {
		int streamLen = initStream("/data/local/tmp/1920x1080_yuv420p_test0.265");
		if (streamLen == -1)
		{
			LOGD("initStream file[/data/local/tmp/1920x1080_yuv420p_test0.265] error!");
			return -1;
		}
		LOGD("initStream len:%d", streamLen);
        mydo = true;
        mT = new thread(&FakeClient::doT,this,15);
		LOGD("线程函数被启动");
		
		return 0;
    }
    void stop ()
    {
        mydo = false;
        mT->join();
		LOGD("线程函数被停止");        
    }
    void doT(int a)
    {
        while(mydo)
        {
			onePicLen = nextNalLength(Pos);
			LOGD("nextNalLength Pos:%d onePicLen:%d", Pos, onePicLen);
			vector<unsigned char> pTmpPic;
			pTmpPic.resize(onePicLen);
			readOneNal(&pTmpPic[0], Pos, onePicLen);
			if(onePicLen <= 0)
				break;
			
            DataProcCb((void*)&pTmpPic[0], onePicLen, false, nullptr);
			Pos += onePicLen;
            this_thread::sleep_for(chrono::milliseconds(30));
        }
        LOGD("线程exit");
    }
    
private:
	thread* mT;   
	bool mydo = true;
	
	int Pos = 0;
    int onePicLen = 0;

};

FakeClient* client = nullptr;*/

Client* client = nullptr;

JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_init
  (JNIEnv *env, jclass obj)
  {
	/*pRecFile = fopen("/data/data/com.android.UniPlugin/readOnePic_0416.265", "rb");
	if(pRecFile == NULL){
		LOGD("receive file open fail...........\n");
		return -2;
	}
	else{
		LOGD("receive file open success...........\n");
		fread(first_frm, 1, 67234, pRecFile);
	}*/		
	
	if(client == nullptr)
	{
		client = new (std::nothrow) Client();
		if(client == nullptr)
			return -1;
		
		if (!client->init(DataProcCb, nullptr)){
			LOGD("client init success...........\n");
			return 0;
		}else{
			LOGD("client init fail...........\n");
			return -2;
		}
	}
	else
		return -3;
	
	/*if(client == nullptr){
		client = new FakeClient();
		int ret = client->init();
		if (!ret){
			LOGD("client init success...........\n");
		}else{
			LOGD("client init fail...........\n");
		}
		return ret;
	}
	return -1;*/	
  }
  
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_videostart
  (JNIEnv *, jclass)
  {
	  LOGD("client videostart...........\n");
	  bitqueue.clear();
	  video_receiving = 1;	
  }
  
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_videostop
  (JNIEnv *, jclass)
  {	  
	  LOGD("client videostop...........\n");
	  video_receiving = 0;
	  idr_received = 0;	
  }
  
JNIEXPORT int JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_NextOnePicLen
  (JNIEnv *env, jclass obj)
  {  
	  return bitqueue.frontBitLen();
  }
  
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_readOnePic
  (JNIEnv *env, jclass obj, jobject jbuffer)
  {  
	  
	  auto rec_bit = bitqueue.pop();
	  if(rec_bit.size() > 0)
	  {
		  LOGD("In readOnePic len:%d", rec_bit.size());
		  void *address = env->GetDirectBufferAddress(jbuffer);
		  memcpy(address, &rec_bit[0], rec_bit.size());
		  /*if(pRecFile)
			{
				LOGD("write len:%d", fwrite(address, 1, rec_bit.size(), pRecFile));
			}*/
	  }
	  else
		  this_thread::sleep_for(chrono::milliseconds(2));
	  
  }
  
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_deinit
  (JNIEnv *env, jclass obj)
  {
	  if(client)
	  {
		  LOGD("before delete");
		  delete client;
		  LOGD("after delete");
		  client = nullptr;
		  bitqueue.clear();
		  
		  /*frm = 0;
		  video_receiving = 0;
		  bitqueue.clear();*/
	  }
  }


JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_write_1AlarmHorn(JNIEnv *env, jobject obj, jobject param)
{
	if(client)
	{
		// 获取类
		jclass cls_StreamReceiver = env->FindClass("com/example/hevcdeocderlibrary/StreamReceiver");	
		if(cls_StreamReceiver == NULL)
			LOGD("cls_StreamReceiver is NULL");
		jclass cls_AlarmHorn = env->FindClass("com/example/hevcdeocderlibrary/StreamReceiver$AlarmHorn");
		if(cls_AlarmHorn == NULL)
		{
			LOGD("cls_AlarmHorn is NULL");
			return false;
		}
		// 获取成员变量的ID
		jfieldID fid_ah_no = env->GetFieldID(cls_AlarmHorn, "ah_no", "I");
		jfieldID fid_ah_v = env->GetFieldID(cls_AlarmHorn, "ah_v", "I");

		// 获取Java对象
		jobject obj_rvp = param;

		// 获取成员变量的值
		int ah_no = env->GetIntField(obj_rvp, fid_ah_no);
		LOGD("ah_no: %d", ah_no);
		int ah_v = env->GetIntField(obj_rvp, fid_ah_v);
		LOGD("ah_v: %d", ah_v);
		
		alarm_horn val(ah_no, ah_v);		
		return client->write_data("alarm_horn", &val, sizeof(val));
	}
	else
		return false;
}

JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_read_1AlarmHorn(JNIEnv *env, jobject obj, jobject param)
{
	if(client)
	{
		// 获取类
		jclass cls_StreamReceiver = env->FindClass("com/example/hevcdeocderlibrary/StreamReceiver");	
		if(cls_StreamReceiver == NULL)
			LOGD("cls_StreamReceiver is NULL");
		jclass cls_AlarmHorn = env->FindClass("com/example/hevcdeocderlibrary/StreamReceiver$AlarmHorn");
		if(cls_AlarmHorn == NULL)
		{
			LOGD("cls_AlarmHorn is NULL");
			return false;
		}
		// 获取成员变量的ID
		jfieldID fid_ah_no = env->GetFieldID(cls_AlarmHorn, "ah_no", "I");
		jfieldID fid_ah_v = env->GetFieldID(cls_AlarmHorn, "ah_v", "I");

		// 获取Java对象
		jobject obj_rvp = param;

		alarm_horn val;	
		val.ah_no = -1;
		val.ah_v = -2;
		bool ret = client->read_data("alarm_horn", &val, sizeof(val));
		if(ret)
		{
			LOGD("read_data success");
		}
		else
			LOGD("read_data fail");
		
		

		// 设置成员变量的值
		env->SetIntField(obj_rvp, fid_ah_no, val.ah_no);
		env->SetIntField(obj_rvp, fid_ah_v, val.ah_v);
	}
	else
		return false;
}


JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_write_1RvParam(JNIEnv *env, jobject obj, jobject param)
{
	if(client)
	{
		// 获取类
		jclass cls_StreamReceiver = env->FindClass("com/example/hevcdeocderlibrary/StreamReceiver");	
		if(cls_StreamReceiver == NULL)
			LOGD("cls_StreamReceiver is NULL");
		jclass cls_RvParam = env->FindClass("com/example/hevcdeocderlibrary/StreamReceiver$RvParam");
		if(cls_RvParam == NULL)
		{
			LOGD("cls_RvParam is NULL");
			return false;
		}
		// 获取成员变量的ID
		jfieldID fid_lane_width = env->GetFieldID(cls_RvParam, "lane_width", "F");
		jfieldID fid_near_field_bound = env->GetFieldID(cls_RvParam, "near_field_bound", "F");
		jfieldID fid_near_field_speed_limit = env->GetFieldID(cls_RvParam, "near_field_speed_limit", "F");
		jfieldID fid_far_field_bound = env->GetFieldID(cls_RvParam, "far_field_bound", "F");
		jfieldID fid_far_field_speed_limit = env->GetFieldID(cls_RvParam, "far_field_speed_limit", "F");

		// 获取Java对象
		jobject obj_rvp = param;

		// 获取成员变量的值
		float lane_width = env->GetIntField(obj_rvp, fid_lane_width);
		LOGD("lane_width: %f", lane_width);
		float near_field_bound = env->GetIntField(obj_rvp, fid_near_field_bound);
		LOGD("near_field_bound: %f", near_field_bound);
		float near_field_speed_limit = env->GetIntField(obj_rvp, fid_near_field_speed_limit);
		LOGD("near_field_speed_limit: %f", near_field_speed_limit);
		float far_field_bound = env->GetIntField(obj_rvp, fid_far_field_bound);
		LOGD("far_field_bound: %f", far_field_bound);
		float far_field_speed_limit = env->GetIntField(obj_rvp, fid_far_field_speed_limit);
		LOGD("far_field_speed_limit: %f", far_field_speed_limit);
		
		rv_param val(lane_width, near_field_bound, near_field_speed_limit, far_field_bound, far_field_speed_limit);		
		return client->write_data("rv_param", &val, sizeof(val));
	}
	else
		return false;
}

JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_read_1RvParam(JNIEnv *env, jobject obj, jobject param)
{
	if(client)
	{
		// 获取类
		jclass cls_StreamReceiver = env->FindClass("com/example/hevcdeocderlibrary/StreamReceiver");	
		if(cls_StreamReceiver == NULL)
			LOGD("cls_StreamReceiver is NULL");
		jclass cls_RvParam = env->FindClass("com/example/hevcdeocderlibrary/StreamReceiver$RvParam");
		if(cls_RvParam == NULL)
		{
			LOGD("cls_RvParam is NULL");
			return false;
		}
		// 获取成员变量的ID
		jfieldID fid_lane_width = env->GetFieldID(cls_RvParam, "lane_width", "F");
		jfieldID fid_near_field_bound = env->GetFieldID(cls_RvParam, "near_field_bound", "F");
		jfieldID fid_near_field_speed_limit = env->GetFieldID(cls_RvParam, "near_field_speed_limit", "F");
		jfieldID fid_far_field_bound = env->GetFieldID(cls_RvParam, "far_field_bound", "F");
		jfieldID fid_far_field_speed_limit = env->GetFieldID(cls_RvParam, "far_field_speed_limit", "F");

		// 获取Java对象
		jobject obj_rvp = param;

		rv_param val(-1, -2, -3, -4, -5);	
		bool ret = client->read_data("rv_param", &val, sizeof(val));
		if(ret)
		{
			LOGD("read_data success");
		}
		else
			LOGD("read_data fail");
		
		

		// 设置成员变量的值
		env->SetIntField(obj_rvp, fid_lane_width, val.lane_width);
		env->SetIntField(obj_rvp, fid_near_field_bound, val.near_field_bound);
		env->SetIntField(obj_rvp, fid_near_field_speed_limit, val.near_field_speed_limit);
		env->SetIntField(obj_rvp, fid_far_field_bound, val.far_field_bound);
		env->SetIntField(obj_rvp, fid_far_field_speed_limit, val.far_field_speed_limit);
	}
	else
		return false;
}
 

JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_setParam(JNIEnv *env, jobject obj, jobject param)
{
    // 获取类
    jclass cls_RadarVideoParam = env->FindClass("com/example/hevcdeocderlibrary/RadarVideoParam");

    // 获取成员变量的ID
    jfieldID fid_lane_offset = env->GetFieldID(cls_RadarVideoParam, "lane_offset", "F");
    jfieldID fid_alarm_horn = env->GetFieldID(cls_RadarVideoParam, "alarm_horn", "Lcom/example/hevcdeocderlibrary/AlarmHorn;");
    jfieldID fid_work_mode = env->GetFieldID(cls_RadarVideoParam, "work_mode", "I");
    jfieldID fid_rv_param = env->GetFieldID(cls_RadarVideoParam, "rv_param", "Lcom/example/hevcdeocderlibrary/RvParam;");
    jfieldID fid_extension = env->GetFieldID(cls_RadarVideoParam, "extension", "[B");

    jclass cls_AlarmHorn = env->FindClass("com/example/hevcdeocderlibrary/AlarmHorn");
    jfieldID fid_ah_no = env->GetFieldID(cls_AlarmHorn, "ah_no", "I");
    jfieldID fid_ah_v = env->GetFieldID(cls_AlarmHorn, "ah_v", "I");

    jclass cls_RvParam = env->FindClass("com/example/hevcdeocderlibrary/RvParam");
    jfieldID fid_lane_width = env->GetFieldID(cls_RvParam, "lane_width", "F");
    jfieldID fid_near_field_bound = env->GetFieldID(cls_RvParam, "near_field_bound", "F");
    jfieldID fid_near_field_speed_limit = env->GetFieldID(cls_RvParam, "near_field_speed_limit", "F");
    jfieldID fid_far_field_bound = env->GetFieldID(cls_RvParam, "far_field_bound", "F");
    jfieldID fid_far_field_speed_limit = env->GetFieldID(cls_RvParam, "far_field_speed_limit", "F");

    // 获取Java对象
    jobject obj_rvp = param;

    // 获取成员变量的值
    float lane_offset = env->GetFloatField(obj_rvp, fid_lane_offset);
	LOGD("lane_offset: %.3f", lane_offset);
    int work_mode = env->GetIntField(obj_rvp, fid_work_mode);
	LOGD("work_mode: %d", work_mode);
    //byte* extension = (byte*)env->GetByteArrayElements(env->GetObjectField(obj_rvp, fid_extension), NULL);

    // 设置成员变量的值
    env->SetFloatField(obj_rvp, fid_lane_offset, 1.0f);
    env->SetIntField(env->GetObjectField(obj_rvp, fid_alarm_horn), fid_ah_no, 1);
    env->SetIntField(env->GetObjectField(obj_rvp, fid_alarm_horn), fid_ah_v, 1);
}
  
JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_write_1int
  (JNIEnv *env, jclass, jstring data_name, jint pData)
  {
	  if(client)
	  {
		LOGD("1111111111");
		const char* str = env->GetStringUTFChars(data_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(data_name, str);
		LOGD("22222");
		
		int intValue = (int) pData;
		return client->write_data(name, &intValue, sizeof(intValue));		
		  
	  }
	  return false;
  }
  
JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_read_1int
  (JNIEnv *env, jclass, jstring data_name)
  {
	  if(client)
	  {
		LOGD("1111111111");
		const char* str = env->GetStringUTFChars(data_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(data_name, str);
		LOGD("22222");
		
		int intValue = -2;
		bool ret = client->read_data(name, &intValue, sizeof(intValue));
		LOGD("read_data return %d...\n", ret);

		return intValue;		
		  
	  }
	  return -1;
  }
  
  JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_write_1float
  (JNIEnv *env, jclass, jstring data_name, jfloat pData)
  {
	  if(client)
	  {
		LOGD("1111111111");
		const char* str = env->GetStringUTFChars(data_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(data_name, str);
		LOGD("22222");
		
		float f = (float) pData;
		return client->write_data(name, &f, sizeof(f));		
		  
	  }
	  return false;
  }
  
JNIEXPORT jfloat JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_read_1float
  (JNIEnv *env, jclass, jstring data_name)
  {
	  if(client)
	  {
		LOGD("1111111111");
		const char* str = env->GetStringUTFChars(data_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(data_name, str);
		LOGD("22222");
		
		float floatValue = -2.1;
		bool ret = client->read_data(name, &floatValue, sizeof(floatValue));
		LOGD("read_data return %d...\n", ret);
		
		return floatValue;		  
	  }
	  return -1.0;
  }
  
  
//JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_write_data
//  (JNIEnv *env, jclass, jstring data_name, jobject pData, jint data_len)
//  {
//	  //if(client)
//	  {
//		LOGD("1111111111");
//		const char* str = env->GetStringUTFChars(data_name, NULL);
//		string name(str);
//		env->ReleaseStringUTFChars(data_name, str);
//		LOGD("22222");
//		
//		jclass cls = env->FindClass("java/lang/Float");
//		jmethodID mid = env->GetMethodID(cls, "floatValue", "()F");
//		LOGD("33333");
//		if (env->IsInstanceOf(pData, cls)) {	
//			LOGD("44444");
//			jfloat f = env->CallFloatMethod(pData, mid);
//			LOGD("pData:%.3f", f);
//			return client->write_data(name, &f, sizeof(f));			
//		}
//		else
//			return -1;
//		  
//	  }
//  }
//  
//JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_read_data
//  (JNIEnv *env, jclass, jstring data_name, jobject pData, jint data_len)
//  {
//	  if(client)
//	  {
//		const char* str = env->GetStringUTFChars(data_name, NULL);
//		string name(str);
//		env->ReleaseStringUTFChars(data_name, str);
//		
//		return client->read_data(name, &pData, data_len);	
//		
//	  }
//  }