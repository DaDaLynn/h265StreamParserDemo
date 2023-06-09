/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class StreamParser */

#ifndef _Included_StreamParser
#define _Included_StreamParser
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     StreamParser
 * Method:    initStream
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamParser_initStream
  (JNIEnv *, jclass, jstring);

/*
 * Class:     StreamParser
 * Method:    nextNalLen
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamParser_nextNalLen
  (JNIEnv *, jclass, jint);

/*
 * Class:     StreamParser
 * Method:    readOneNal
 * Signature: (Ljava/lang/Object;II)V
 */
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamParser_readOneNal
  (JNIEnv *, jclass, jobject, jint, jint);

/*
 * Class:     StreamParser
 * Method:    add
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamParser_add
  (JNIEnv *, jclass, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
