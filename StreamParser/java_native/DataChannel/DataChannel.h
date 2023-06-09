/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class DataChannel */

#ifndef _Included_DataChannel
#define _Included_DataChannel
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     DataChannel
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_example_clientlibrary_DataChannel_init
  (JNIEnv *, jclass);

/*
 * Class:     DataChannel
 * Method:    deinit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_example_clientlibrary_DataChannel_deinit
  (JNIEnv *, jclass);

/*
 * Class:     DataChannel
 * Method:    write_int
 * Signature: (Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1int
  (JNIEnv *, jclass, jstring, jint);

/*
 * Class:     DataChannel
 * Method:    read_int
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_example_clientlibrary_DataChannel_read_1int
  (JNIEnv *, jclass, jstring);

/*
 * Class:     DataChannel
 * Method:    write_float
 * Signature: (Ljava/lang/String;F)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1float
  (JNIEnv *, jclass, jstring, jfloat);

/*
 * Class:     DataChannel
 * Method:    read_float
 * Signature: (Ljava/lang/String;)F
 */
JNIEXPORT jfloat JNICALL Java_com_example_clientlibrary_DataChannel_read_1float
  (JNIEnv *, jclass, jstring);

/*
 * Class:     DataChannel
 * Method:    write_AlarmHorn
 * Signature: (LDataChannel/AlarmHorn;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1AlarmHorn
  (JNIEnv *, jclass, jobject);

/*
 * Class:     DataChannel
 * Method:    read_AlarmHorn
 * Signature: (LDataChannel/AlarmHorn;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_read_1AlarmHorn
  (JNIEnv *, jclass, jobject);

/*
 * Class:     DataChannel
 * Method:    write_RvParam
 * Signature: (LDataChannel/RvParam;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_write_1RvParam
  (JNIEnv *, jclass, jobject);

/*
 * Class:     DataChannel
 * Method:    read_RvParam
 * Signature: (LDataChannel/RvParam;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_read_1RvParam
  (JNIEnv *, jclass, jobject);

/*
 * Class:     DataChannel
 * Method:    read_RadarData
 * Signature: (LDataChannel/RadarData;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_clientlibrary_DataChannel_1RadarData
  (JNIEnv *, jclass, jobject);

#ifdef __cplusplus
}
#endif
#endif
