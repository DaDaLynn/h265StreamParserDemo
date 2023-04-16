#include "StreamParser.h"
#include "../../h265StreamParser.h"


JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamParser_initStream
  (JNIEnv *env, jclass obj, jstring fileDir)
  {
	  const char* folder = env->GetStringUTFChars(fileDir, NULL);
	  char pStreamFile[200];
	  if(strlen(folder) <= 199)
		  strcpy(pStreamFile, folder);
	  else
		  return -1;	  
	  env->ReleaseStringUTFChars(fileDir, folder);
	  
	  jint streamLen = 0;
	  streamLen = initStream(pStreamFile);
	  return streamLen;
  }
  
JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamParser_nextNalLen
  (JNIEnv *env, jclass obj, jint Pos)
  {
	  jint sliceLen = nextNalLength(Pos);
	  return sliceLen;
  }
  
JNIEXPORT void JNICALL Java_com_example_hevcdeocderlibrary_StreamParser_readOneNal
  (JNIEnv *env, jclass obj, jobject jbuffer, jint Pos, jint sliceLen)
  {
	  void *address = env->GetDirectBufferAddress(jbuffer);
	  readOneNal((unsigned char*)address, Pos, sliceLen);
  }
  
  JNIEXPORT jint JNICALL Java_com_example_hevcdeocderlibrary_StreamParser_add
  (JNIEnv *env, jclass obj, jint a, jint b)
  {
	  return a + b;
  }