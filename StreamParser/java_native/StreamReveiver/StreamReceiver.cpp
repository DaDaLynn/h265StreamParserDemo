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

atomic<int> frm(0);
atomic<int> isRunning(0);
void DataProcCb(void* pData, int data_len, bool bIdr, void* pUserData){
    LOGD("receive frm: %d len:%d", ++frm, data_len);
	/*if(pRecFile)
	{
		LOGD("write len:%d", fwrite(pData, 1, data_len, pRecFile));
	}*/
	if(isRunning)
		bitqueue.push(pData, data_len);
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
	
	isRunning = 1;
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
		return 0;
	
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
		  //LOGD("before delete");
		  //delete client;
		  //LOGD("after delete");
		  //client = nullptr;
		  //bitqueue.clear();
		  frm = 0;
		  isRunning = 0;
		  bitqueue.clear();
	  }
  }