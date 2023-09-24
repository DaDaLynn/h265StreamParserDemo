#include "DataChannel.h"
#include "datatrans/Release2023.5.21/includes/client.hpp"
#include "circular_buffer.hpp"

#include <android/log.h>
#include <atomic>

#define TAG "njbas_0624"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
using namespace std;

class LatestRadarData{
	
public:
	void write(const RadarData & _in_data)
	{
		unique_lock<mutex> m(m_mtx);
		data.obj_id  = _in_data.obj_id;
		data.dislong = _in_data.dislong;
		data.dislat  = _in_data.dislat;
		data.vrelong = _in_data.vrelong;
		data.status  = _in_data.status;
		LOGD("write obj_id=%d dislong=%.3f dislat=%.3f vrelong=%.3f status=%d", data.obj_id, data.dislong, data.dislat, data.vrelong, data.status);
		is_updated = true;
	}
	
	bool read(RadarData& _io_data)
	{
		unique_lock<mutex> m(m_mtx);
		if(is_updated){
			_io_data.obj_id  = data.obj_id;
			_io_data.dislong = data.dislong;
			_io_data.dislat  = data.dislat;
			_io_data.vrelong = data.vrelong;
			_io_data.status  = data.status;
			return true;
		}
		else
			return false;
	}
	
private:
	RadarData data{0};
	mutex m_mtx;
	bool is_updated{false};
};

class RadarDataWareHouse{
	
public:
	RadarDataWareHouse(size_t element_num=1280, int32_t pkg_max_num=1280)
	{
		data.init(element_num, pkg_max_num);
	}
	
	void write(const RadarData & _in_data)
	{
		RadarData m_data{_in_data.obj_id, _in_data.dislong, _in_data.dislat, _in_data.vrelong, _in_data.status};
		data.push(m_data, 0, true);
		LOGD("write obj_id=%d dislong=%.3f dislat=%.3f vrelong=%.3f status=%d", m_data.obj_id, m_data.dislong, m_data.dislat, m_data.vrelong, m_data.status);
	}
	
	bool read(vector<RadarData>& _io_data)
	{
		LOGD("read RadarData size = %d", data.size());
		if(!data.empty()){
			RadarData m_data{-1, -1, -1, -1, -1};
			int num = 1;
			while(data.pop(&m_data, num) == 0)
			{
				_io_data.push_back(m_data);
			}
			return true;
		}
		else
			return false;
	}
	
private:
	CircularBuffer_s<RadarData> data;
};

class BitsQueue {
public:
    atomic<int> frm{ 0 };
    atomic<int> video_receiving{ 0 };
    atomic<int> idr_received{ 0 };

    long long lastUpdateTime{0};

    int buffer_size = 0;
    long long cur{ 0 }, pre{0};
    int err_time{ 0 };

    void push(void* pdata, int len) {
        unique_lock<mutex> m(m_mtx);
        vector<char> t((char*)pdata, (char*)pdata + len);
        bits.push(t);
        if (bits.size() >= buffer_size)
            ready = true;
    }

    vector<char> pop() {
        update(30, 3); // 根据目标帧率和buffer帧数控制获取数据的时延
        vector<char> out;
        if(ready)
        { 
            //ctrl(30);
            unique_lock<mutex> m(m_mtx);
            if (bits.size() > 0 ) {
                out = move(bits.front());
                bits.pop();
            }
        }
        return out;
    }

    int size() {
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

    // 获取当前时间（毫秒）
    long long getCurrentTime() {
        return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    }

    bool update(int targetFrameRate, int bufferFrameSize) {
        if (!(targetFrameRate > 0 && bufferFrameSize > 0))
            return false;

        if(lastUpdateTime != 0 && bits.size() < bufferFrameSize) // 当队列中的数据少于设定的buffer帧数时需要调整速率
        {
            long long now = getCurrentTime();
            int delta = (int)(now - lastUpdateTime); // 连续两帧的时间差 需要从targetUpdateTime中减去
            int targetUpdateTime = 1000 / targetFrameRate;
            int updatetimedelay = (targetUpdateTime - delta) + (bufferFrameSize - bits.size()) * (targetUpdateTime / bufferFrameSize);
            if(updatetimedelay > 0){
                this_thread::sleep_for(chrono::milliseconds(updatetimedelay));
            }
        }

        if(bits.size())
            lastUpdateTime = getCurrentTime();
        return true;
    }

    void ctrl(int targetFrameRate) {
        if (bits.size() > buffer_size || bits.size() == 0)
        {
            pre = cur = err_time = 0;
            return;
        }

        // 当队列中的数据少于设定的buffer帧数时需要调整速率
        float ratio = bits.size() * 1.0 / buffer_size;
        int target_gap_ms = 1000 / (targetFrameRate * ratio);

        if (0 == pre) {
            pre = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        }
        else
        {
            cur = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
            err_time += target_gap_ms - (cur - pre);			
            pre = cur;
            if(err_time > 0)
                this_thread::sleep_for(chrono::milliseconds(err_time));

        }
    }

private:
    bool ready{false};
    queue<vector<char>> bits;
    mutex m_mtx;
};


class BaseClient{	
public:
	Client* pC{nullptr};
	RadarDataWareHouse* pR{nullptr};	//LatestRadarData* pR{nullptr};
	BitsQueue* pV{nullptr};
};

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

void VideoDataCb(void* pData, int data_len, bool bIdr, void* pUserData){
	
	BitsQueue* pBitsQueue = ((BaseClient*)pUserData)->pV;
	LOGD("VideoDataCb: len:%d pBitsQueue->size:%d", data_len, pBitsQueue->size());
	if(pBitsQueue->video_receiving)
	{
		if(bIdr)
			pBitsQueue->idr_received = 1;
		
		if(pBitsQueue->idr_received){
			//LOGD("receive frm: %d len:%d", ++pBitsQueue->frm, data_len);
			pBitsQueue->push(pData, data_len);
		}
		
		if(pBitsQueue->size() > 100)
		{
			LOGD("VideoDataCb pBitsQueue overflow...............");
		}
	}
}

int64_t get_ts_us() //us
{
    using namespace std::chrono;
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

void RadarDataCb(void* pData, int data_len, void* pUserData){
	LOGD("RadarDataCb, pData: %p pUserData: %p .........", pData, pUserData);
    static int64_t ts = get_ts_us();
    auto interval = get_ts_us() - ts;
    ts = get_ts_us();
    //RadarData radar_data = *(RadarData*)pData; //需要将radar_data送给界面展示
	RadarData radar_data;
	memcpy(&radar_data, pData, sizeof(radar_data));
    LOGD("RadarData[%d]==>data_len: %d, %.2fms\n", radar_data.obj_id, data_len, interval/1000.0f);
	BaseClient* pBase = (BaseClient*)pUserData;
	if(pBase)
	{
		RadarDataWareHouse* radardataWH = pBase->pR;// LatestRadarData* latestdata = pBase->pR;
		if(radardataWH/*latestdata*/){
			radardataWH->write(radar_data);//latestdata->write(radar_data);
		}
		else
			LOGD("latestdata is null xxxxxxxxxx");
	}
	else
		LOGD("pBase is null xxxxxxxxxx");
}
  
JNIEXPORT jlong JNICALL Java_com_example_clientlibrary_DataChannel_init
  (JNIEnv *env, jclass obj)
  {
	  LOGD("init....");
	  BaseClient* pBClient = new (std::nothrow) BaseClient();
	  if(pBClient == nullptr)
	  	return reinterpret_cast<jlong>(pBClient);
	
	  pBClient->pC = new (std::nothrow) Client();
	  pBClient->pR = new (std::nothrow) RadarDataWareHouse();//pBClient->pR = new (std::nothrow) LatestRadarData();
	  pBClient->pV = new (std::nothrow) BitsQueue();
	  if(pBClient->pC == nullptr || pBClient->pR == nullptr || pBClient->pV == nullptr )
	  {
		  if(pBClient->pC != nullptr) 
		  {
			  delete pBClient->pC;
			  pBClient->pC = nullptr;
		  }
		  if(pBClient->pR != nullptr)
		  {		
			  delete pBClient->pR;
			  pBClient->pR = nullptr;
		  }
		  if(pBClient->pV != nullptr)
		  {			  
			  delete pBClient->pV;
			  pBClient->pV = nullptr;
		  }
		  delete pBClient;
		  pBClient = nullptr;
		  return reinterpret_cast<jlong>(pBClient);
	  }	
	  
	  if (!pBClient->pC->init(VideoDataCb, pBClient, RadarDataCb, pBClient)){
	  	LOGD("pBClient->pC init success, pBClient: %p...........", pBClient);
	  	return reinterpret_cast<jlong>(pBClient);
	  }else{
		  if(pBClient->pC != nullptr) 
		  {
			  delete pBClient->pC;
			  pBClient->pC = nullptr;
		  }
		  if(pBClient->pR != nullptr)
		  {		
			  delete pBClient->pR;
			  pBClient->pR = nullptr;
		  }
		  if(pBClient->pV != nullptr)
		  {			  
			  delete pBClient->pV;
			  pBClient->pV = nullptr;
		  }
		  delete pBClient;
		  pBClient = nullptr;
		  return reinterpret_cast<jlong>(pBClient);
	  }
  }

JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_status
  (JNIEnv *env, jclass, jlong ptrVal)
  {
	  LOGD("status....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {
		char device_info[256];
		return pBClient->pC->read_data("device_info", device_info, sizeof(device_info));
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	  	return false;
	  }
  }
  
JNIEXPORT jint JNICALL Java_com_example_clientlibrary_DataChannel_deinit
  (JNIEnv *env, jclass obj, jlong ptrVal)
  {
	  LOGD("deinit....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient)
	  {
		  if(pBClient->pC != nullptr) 
		  {
			  delete pBClient->pC;
			  pBClient->pC = nullptr;
		  }
		  if(pBClient->pR != nullptr)
		  {		
			  delete pBClient->pR;
			  pBClient->pR = nullptr;
		  }
		  if(pBClient->pV != nullptr)
		  {			  
			  delete pBClient->pV;
			  pBClient->pV = nullptr;
		  }
		  delete pBClient;
		  pBClient = nullptr;
		  LOGD("pBClient delete success.......");
		  return 0;
	  }
	  return -1;
  }
  
  
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1RvParam(JNIEnv *env, jclass obj, jlong ptrVal, jobject param)
{
	LOGD("write_1RvParam1....");
	BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	if(pBClient && pBClient->pC)
	{
		// 获取类
		jclass cls_DataChannel = env->FindClass("com/example/clientlibrary/DataChannel");	
		if(cls_DataChannel == NULL)
			LOGD("cls_DataChannel is NULL");
		jclass cls_RvParam = env->FindClass("com/example/clientlibrary/DataChannel$RvParam");
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
		float lane_width = env->GetFloatField(obj_rvp, fid_lane_width);
		LOGD("lane_width: %f", lane_width);
		float near_field_bound = env->GetFloatField(obj_rvp, fid_near_field_bound);
		LOGD("near_field_bound: %f", near_field_bound);
		float near_field_speed_limit = env->GetFloatField(obj_rvp, fid_near_field_speed_limit);
		LOGD("near_field_speed_limit: %f", near_field_speed_limit);
		float far_field_bound = env->GetFloatField(obj_rvp, fid_far_field_bound);
		LOGD("far_field_bound: %f", far_field_bound);
		float far_field_speed_limit = env->GetFloatField(obj_rvp, fid_far_field_speed_limit);
		LOGD("far_field_speed_limit: %f", far_field_speed_limit);
		
		rv_param val(lane_width, near_field_bound, near_field_speed_limit, far_field_bound, far_field_speed_limit);	
		return pBClient->pC->write_data("rv_param", &val, sizeof(val));
	}
	else		
	{
		LOGD("pBClient or pBClient->pC is null....");
		return false;
	}
}

JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_read_1RvParam(JNIEnv *env, jclass obj, jlong ptrVal, jobject param)
{
	LOGD("read_1RvParam1....");
	BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	if(pBClient && pBClient->pC)
	{
		// 获取类
		jclass cls_DataChannel = env->FindClass("com/example/clientlibrary/DataChannel");	
		if(cls_DataChannel == NULL)
			LOGD("cls_DataChannel is NULL");
		jclass cls_RvParam = env->FindClass("com/example/clientlibrary/DataChannel$RvParam");
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
		bool ret = pBClient->pC->read_data("rv_param", &val, sizeof(val));
		if(ret)
		{
			LOGD("read_data success");
		}
		else{
			LOGD("read_data fail");
			return false;
		}
		
		env->SetFloatField(obj_rvp, fid_lane_width, val.lane_width);
		env->SetFloatField(obj_rvp, fid_near_field_bound, val.near_field_bound);
		env->SetFloatField(obj_rvp, fid_near_field_speed_limit, val.near_field_speed_limit);
		env->SetFloatField(obj_rvp, fid_far_field_bound, val.far_field_bound);
		env->SetFloatField(obj_rvp, fid_far_field_speed_limit, val.far_field_speed_limit);
		return true;
	}
	else		
	{
		LOGD("pBClient or pBClient->pC is null....");
		return false;
	}
}

/*JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_read_1RadarData(JNIEnv *env, jclass obj, jlong ptrVal, jobject param)
{
	LOGD("read_1RadarData....");
	BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	if(pBClient && pBClient->pC)
	{
		// 获取类
		jclass cls_DataChannel = env->FindClass("com/example/clientlibrary/DataChannel");	
		if(cls_DataChannel == NULL)
			LOGD("cls_DataChannel is NULL");
		jclass cls_RadarData = env->FindClass("com/example/clientlibrary/DataChannel$RadarData");
		if(cls_RadarData == NULL)
		{
			LOGD("cls_RadarData is NULL");
			return false;
		}
		// 获取成员变量的ID
		jfieldID fid_obj_id = env->GetFieldID(cls_RadarData, "obj_id", "I");
		jfieldID fid_dislong = env->GetFieldID(cls_RadarData, "dislong", "F");
		jfieldID fid_dislat = env->GetFieldID(cls_RadarData, "dislat", "F");
		jfieldID fid_vrelong = env->GetFieldID(cls_RadarData, "vrelong", "F");
		jfieldID fid_status = env->GetFieldID(cls_RadarData, "status", "I");

		// 获取Java对象
		jobject obj_rdp = param;

		RadarData val{0};			
		bool ret = pBClient->pR->read(val);
		if(ret){
			// 设置成员变量的值
			env->SetIntField(obj_rdp, fid_obj_id, val.obj_id);
			env->SetFloatField(obj_rdp, fid_dislong, val.dislong);
			env->SetFloatField(obj_rdp, fid_dislat, val.dislat);
			env->SetFloatField(obj_rdp, fid_vrelong, val.vrelong);
			env->SetIntField(obj_rdp, fid_status, val.status);
			return true;
		}
		else
		{
			LOGD("radar_data read fail");
			return false;
		}
	}
	else		
	{
		LOGD("pBClient or pBClient->pC is null....");
		return false;
	}
}*/

// 实现对应的函数
JNIEXPORT jobjectArray JNICALL Java_com_example_clientlibrary_DataChannel_read_1RadarDataArray(JNIEnv *env, jclass obj, jlong ptrVal)
{
	LOGD("1RadarDataArray....");
	BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	if(pBClient && pBClient->pC)
	{
		vector<RadarData> val_vec;			
		bool ret = pBClient->pR->read(val_vec);
		int receive_num = val_vec.size();
		LOGD("pBClient->pR->read %d radar data....", receive_num);
		
		// 获取类
		jclass cls_DataChannel = env->FindClass("com/example/clientlibrary/DataChannel");	
		if(cls_DataChannel == NULL)
			LOGD("cls_DataChannel is NULL");
		jclass cls_RadarData = env->FindClass("com/example/clientlibrary/DataChannel$RadarData");
		if(cls_RadarData == NULL)
		{
			LOGD("cls_RadarData is NULL");
			return NULL;
		}

		// 获取RadarData类的构造方法ID
		jmethodID radarData_mid = env->GetMethodID(cls_RadarData, "<init>", "(IFFFI)V");
		if(radarData_mid == NULL)
			LOGD("radarData_mid is NULL");
		
		// 创建一个jobjectArray对象，长度为receive_num
		jobjectArray radarData_jarr = env->NewObjectArray(receive_num, cls_RadarData, NULL);
		// 遍历元素
		for (int i = 0; i < receive_num; i++) {
			jobject radarData_obj = env->NewObject(cls_RadarData, radarData_mid, val_vec[i].obj_id, val_vec[i].dislong, val_vec[i].dislat, val_vec[i].vrelong, val_vec[i].status);
			// 将对象赋值给jobjectArray对象中的对应元素
			env->SetObjectArrayElement(radarData_jarr, i, radarData_obj);
			// 释放radarData_obj的引用
			env->DeleteLocalRef(radarData_obj);
		}
		
		// 创建一个jobjectArray对象的副本，用于返回给java层
		//jobjectArray result = env->NewLocalRef(radarData_jarr);
		
		// 释放radarData_jarr的引用
		//env->DeleteLocalRef(radarData_jarr);
		
		// 返回result给java层
		return radarData_jarr;		
	}
	else		
	{
		LOGD("pBClient or pBClient->pC is null....");
		return NULL;
	}
}

  
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1int
  (JNIEnv *env, jclass, jlong ptrVal, jstring data_name, jint pData)
  {
	  LOGD("write_1int....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {
		const char* str = env->GetStringUTFChars(data_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(data_name, str);
		
		uint8_t uintValue = (uint8_t) pData;
		LOGD("write %s %d....", name.c_str(), (int)uintValue);
		return pBClient->pC->write_data(name, &uintValue, sizeof(uintValue));		
		  
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	  	return false;
	  }
  }
  
JNIEXPORT jint JNICALL Java_com_example_clientlibrary_DataChannel_read_1int
  (JNIEnv *env, jclass, jlong ptrVal, jstring data_name)
  {
	  LOGD("read_1int....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {
		const char* str = env->GetStringUTFChars(data_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(data_name, str);
		
		uint8_t uintValue = 125;
		bool ret = pBClient->pC->read_data(name, &uintValue, sizeof(uintValue));
		if(ret){
			LOGD("read %s %d....", name.c_str(), (int)uintValue);
			LOGD("read_data return %d...\n", ret);
			return (int)uintValue;
		}
		else
		{
			LOGD("read_data fail");
			return -1;
		}			
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	  	return -1;
	  }
  }
  
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1name
  (JNIEnv *env, jclass, jlong ptrVal, jstring device_name)
  {
	  LOGD("write_1name....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {
		const char* str = env->GetStringUTFChars(device_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(device_name, str);
		
		char ch_name[256];
		int i;
		for( i=0;i<name.length();i++){
			ch_name[i] = name[i];
		}
		ch_name[name.length()] = 0;
		LOGD("ch_name:%s len:%d....", ch_name, sizeof(ch_name));
		return pBClient->pC->write_data("device_name", ch_name, sizeof(ch_name));	
		  
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	  	return false;
	  }
  }
  
JNIEXPORT jstring JNICALL Java_com_example_clientlibrary_DataChannel_read_1name
  (JNIEnv *env, jclass, jlong ptrVal)
  {
	  LOGD("read_1name....");
	  jstring device_name;
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {		
		char name[256];
		bool ret = pBClient->pC->read_data("device_name", name, sizeof(name));
		if(ret)
		{			
			LOGD("name:%s len:%d....", name, sizeof(name));
			LOGD("read_name return %d...\n", ret);
			return env->NewStringUTF(name);
		}
		else{
			LOGD("read_data fail");
			return device_name;
		}
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	    return device_name;
	  }	
  }
  
JNIEXPORT jstring JNICALL Java_com_example_clientlibrary_DataChannel_read_1info
  (JNIEnv *env, jclass, jlong ptrVal)
  {
	  LOGD("read_1info....");
	  jstring device_info_null;
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {		
		char device_info[256];
		bool ret = pBClient->pC->read_data("device_info", device_info, sizeof(device_info));
		if(ret)
		{
			LOGD("device_info:%s len:%d....", device_info, sizeof(device_info));
			LOGD("read_info return %d...\n", ret);
			return env->NewStringUTF((string(device_info) + "[" + string(&device_info[64]) + "]").c_str());
		}
		else
		{
			LOGD("read_data fail");
			return device_info_null;
		}
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	    return device_info_null;
	  }	
  }
    
  
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1float
  (JNIEnv *env, jclass, jlong ptrVal, jstring data_name, jfloat pData)
  {
	  LOGD("write_1float....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {
		const char* str = env->GetStringUTFChars(data_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(data_name, str);
		
		float f = (float) pData;
		LOGD("write %s %f....", name.c_str(), f);
		return pBClient->pC->write_data(name, &f, sizeof(f));		
		  
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	  	return false;
	  }
  }
  
JNIEXPORT jfloat JNICALL Java_com_example_clientlibrary_DataChannel_read_1float
  (JNIEnv *env, jclass, jlong ptrVal, jstring data_name)
  {
	  LOGD("read_1float....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {
		const char* str = env->GetStringUTFChars(data_name, NULL);
		string name(str);
		env->ReleaseStringUTFChars(data_name, str);
		
		float floatValue = -2.1;
		bool ret = pBClient->pC->read_data(name, &floatValue, sizeof(floatValue));
		if(ret)
		{
			LOGD("read %s %f....", name.c_str(), floatValue);
			return floatValue;	
		}	
		else
		{
			LOGD("read_data fail");
			return -1.0;
		}
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	  	return -1.0;
	  }
  }
  
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1AlarmHorn(JNIEnv *env, jclass obj, jlong ptrVal, jobject param)
{
	  LOGD("write_1AlarmHorn....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {
	  	// 获取类
	  	jclass cls_DataChannel = env->FindClass("com/example/clientlibrary/DataChannel");	
	  	if(cls_DataChannel == NULL)
	  		LOGD("cls_DataChannel is NULL");
	  	jclass cls_AlarmHorn = env->FindClass("com/example/clientlibrary/DataChannel$AlarmHorn");
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
	  	return pBClient->pC->write_data("alarm_horn", &val, sizeof(val));
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	  	return false;
	  }
}

JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_read_1AlarmHorn(JNIEnv *env, jclass obj, jlong ptrVal, jobject param)
{
	  LOGD("read_1AlarmHorn....");
	  BaseClient* pBClient  = reinterpret_cast<BaseClient*>(ptrVal);
	  if(pBClient && pBClient->pC)
	  {
	  	// 获取类
	  	jclass cls_DataChannel = env->FindClass("com/example/clientlibrary/DataChannel");	
	  	if(cls_DataChannel == NULL)
	  		LOGD("cls_DataChannel is NULL");
	  	jclass cls_AlarmHorn = env->FindClass("com/example/clientlibrary/DataChannel$AlarmHorn");
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
	  	bool ret = pBClient->pC->read_data("alarm_horn", &val, sizeof(val));
	  	if(ret)
	  	{
	  		LOGD("read_data success");
	  	}
	  	else{
	  		LOGD("read_data fail");		
			return false;
		}			
      
	  	// 设置成员变量的值
	  	env->SetIntField(obj_rvp, fid_ah_no, val.ah_no);
	  	env->SetIntField(obj_rvp, fid_ah_v, val.ah_v);
		return ret;
	  }
	  else		
	  {
	  	LOGD("pBClient or pBClient->pC is null....");
	  	return false;
	  }
}