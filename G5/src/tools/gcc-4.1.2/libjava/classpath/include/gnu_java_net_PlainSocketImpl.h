/* DO NOT EDIT THIS FILE - it is machine generated */

#ifndef __gnu_java_net_PlainSocketImpl__
#define __gnu_java_net_PlainSocketImpl__

#include <jni.h>

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_setOption (JNIEnv *env, jobject, jint, jobject);
JNIEXPORT jobject JNICALL Java_gnu_java_net_PlainSocketImpl_getOption (JNIEnv *env, jobject, jint);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_shutdownInput (JNIEnv *env, jobject);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_shutdownOutput (JNIEnv *env, jobject);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_create (JNIEnv *env, jobject, jboolean);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_connect (JNIEnv *env, jobject, jobject, jint);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_bind (JNIEnv *env, jobject, jobject, jint);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_listen (JNIEnv *env, jobject, jint);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_accept (JNIEnv *env, jobject, jobject);
JNIEXPORT jint JNICALL Java_gnu_java_net_PlainSocketImpl_available (JNIEnv *env, jobject);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_close (JNIEnv *env, jobject);
JNIEXPORT jint JNICALL Java_gnu_java_net_PlainSocketImpl_read (JNIEnv *env, jobject, jbyteArray, jint, jint);
JNIEXPORT void JNICALL Java_gnu_java_net_PlainSocketImpl_write (JNIEnv *env, jobject, jbyteArray, jint, jint);

#ifdef __cplusplus
}
#endif

#endif /* __gnu_java_net_PlainSocketImpl__ */
