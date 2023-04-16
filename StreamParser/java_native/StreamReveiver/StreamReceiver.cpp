#include "StreamReceiver.h"
#include "h265_transport/Release/includes/client.hpp"

#include <android/log.h>
#include <atomic>

#define TAG "njbas"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

#define BUFFER_SIZE 1024*1024*10

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
using namespace std;

Client client;


FILE * pRecFile = NULL;

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
	
	private:
	
	queue<vector<char>> bits;
	mutex m_mtx;
};

BitsQueue bitqueue;

void DataProcCb(void* pData, int data_len, bool bIdr, void* pUserData){
    LOGD("receive len:%d", data_len);
	if(pRecFile)
	{
		//LOGD("write len:%d", fwrite(pData, 1, data_len, pRecFile));
	}
	bitqueue.push(pData, data_len);
}

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
	
	if (!client.init(DataProcCb, nullptr)){
        LOGD("client init success...........\n");
		return 0;
    }else{
        LOGD("client init fail...........\n");
		return -1;
    }
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