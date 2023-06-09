#include "DataChannel.h"

#include <android/log.h>
#include <atomic>

#define TAG "njbas_0604"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
using namespace std;


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

JNIEXPORT jint JNICALL Java_com_example_clientlibrary_DataChannel_init
  (JNIEnv *env, jclass obj)
  {
	  LOGD("init....");
	  return true;
  }
  
JNIEXPORT jint JNICALL Java_com_example_clientlibrary_DataChannel_deinit
  (JNIEnv *env, jclass obj)
  {
	  LOGD("deinit....");
	  return true;
  }

rv_param local_test;

JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1RvParam(JNIEnv *env, jclass obj, jobject param)
{
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
		
		local_test.lane_width = lane_width;
		local_test.near_field_bound = near_field_bound;
		local_test.near_field_speed_limit = near_field_speed_limit;
		local_test.far_field_bound = far_field_bound;
		local_test.far_field_speed_limit = far_field_speed_limit;
		
		LOGD("After write : Local lane_width: %f", local_test.lane_width);
		LOGD("After write : Local near_field_bound: %f", local_test.near_field_bound);
		LOGD("After write : Local near_field_speed_limit: %f", local_test.near_field_speed_limit);
		LOGD("After write : Local far_field_bound: %f", local_test.far_field_bound);
		LOGD("After write : Local far_field_speed_limit: %f", local_test.far_field_speed_limit);
		
		return true;
	}
}

JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_read_1RvParam(JNIEnv *env, jclass obj, jobject param)
{
	{
		LOGD("Before read: Local lane_width: %f", local_test.lane_width);
		LOGD("Before read: Local near_field_bound: %f", local_test.near_field_bound);
		LOGD("Before read: Local near_field_speed_limit: %f", local_test.near_field_speed_limit);
		LOGD("Before read: Local far_field_bound: %f", local_test.far_field_bound);
		LOGD("Before read: Local far_field_speed_limit: %f", local_test.far_field_speed_limit);
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
		
		env->SetFloatField(obj_rvp, fid_lane_width, local_test.lane_width);
		env->SetFloatField(obj_rvp, fid_near_field_bound, local_test.near_field_bound);
		env->SetFloatField(obj_rvp, fid_near_field_speed_limit, local_test.near_field_speed_limit);
		env->SetFloatField(obj_rvp, fid_far_field_bound, local_test.far_field_bound);
		env->SetFloatField(obj_rvp, fid_far_field_speed_limit, local_test.far_field_speed_limit);
		
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
		return true;
	}
}