/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class StreamReceiver */

#ifndef _Included_StreamReceiver
#define _Included_StreamReceiver
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     StreamReceiver
 * Method:    init
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_init
  (JNIEnv *, jclass);
  
/*
 * Class:     StreamReceiver
 * Method:    videostart
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_videostart
  (JNIEnv *, jclass);
  
/*
 * Class:     StreamReceiver
 * Method:    videostop
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_videostop
  (JNIEnv *, jclass);
  
/*
 * Class:     StreamReceiver
 * Method:    NextOnePicLen
 * Signature: (Ljava/lang/Object;II)V
 */
JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_NextOnePicLen
  (JNIEnv *, jclass);

/*
 * Class:     StreamReceiver
 * Method:    readOnePic
 * Signature: (Ljava/lang/Object;II)V
 */
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_readOnePic
  (JNIEnv *, jclass, jobject);
  
/*
 * Class:     StreamReceiver
 * Method:    deinit
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_deinit
  (JNIEnv *, jclass);
  
/*
 * Class:     StreamReceiver
 * Method:    setParam
 * Signature: (Ljava/lang/String;)I
 */  
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_setParam
  (JNIEnv *env, jobject, jobject);

/*
 * Class:     StreamReceiver
 * Method:    write_AlarmHorn
 * Signature: (Ljava/lang/String;)I
 */  
JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_write_1AlarmHorn
  (JNIEnv *env, jobject, jobject);
  
/*
 * Class:     StreamReceiver
 * Method:    read_AlarmHorn
 * Signature: (Ljava/lang/String;)I
 */  
JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_read_1AlarmHorn
  (JNIEnv *env, jobject, jobject);
  
/*
 * Class:     StreamReceiver
 * Method:    write_int
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_write_1int
  (JNIEnv *, jclass, jstring, jint);
  
/*
 * Class:     StreamReceiver
 * Method:    read_int
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_read_1int
  (JNIEnv *, jclass, jstring);
  
/*
 * Class:     StreamReceiver
 * Method:    write_float
 * Signature: (Ljava/lang/String;F)Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_write_1float
  (JNIEnv *, jclass, jstring, jfloat);

/*
 * Class:     StreamReceiver
 * Method:    read_float
 * Signature: (Ljava/lang/String;)F
 */
JNIEXPORT jfloat JNICALL Java_com_example_hevcdeocderlibrary_StreamReceiver_read_1float
  (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif
