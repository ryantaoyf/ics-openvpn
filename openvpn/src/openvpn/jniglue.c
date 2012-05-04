#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <setjmp.h>

#include "jniglue.h"

JNIEXPORT jint Java_de_blinkt_OpenVPN_startOpenVPNThread(JNIEnv* env, jclass jc);


extern int main (int argc, char *argv[]);

static jmp_buf jump_buffer;

int callmain (int argc, char *argv[]) {
    if(!setjmp(jump_buffer))
        main(argc,argv);
}


void android_openvpn_exit(int status) {
    longjmp(jump_buffer,status+1);
}


void testmsg(char* m1, ...) {
	va_list arglist;
	va_start(arglist,m1);
	//	vsnprintf(m1, 100, "foo bbaz",arglist);
	__android_log_vprint(ANDROID_LOG_DEBUG, "openvpn",m1, arglist);
		va_end(arglist);
}

// Store env and class, we allow only one instance
// so make these variables global for now
jclass openvpnclass;
JNIEnv* openvpnjenv;

//Lde/blinkt/openvpn/OpenVPN startOpenVPNThread startOpenVPNThread
 jint Java_de_blinkt_openvpn_OpenVPN_startOpenVPNThread(JNIEnv* env, jclass jc){
    char* argv[] = {"openvpn", "--client",
                    "--dev","tun",
                    "--comp-lzo",
//                    "--redirect-gateway","def1",
//                    "--pkcs12","/mnt/sdcard/Network_Certificate.p12",
                    "--remote-cert-eku", "TLS Web Server Authentication",
                    "--remote","openvpn.uni-paderborn.de",
                    "--ca","/mnt/sdcard/ca.pem",
                    "--key","/mnt/sdcard/schwabe.key",
                    "--cert","/mnt/sdcard/schwabe.pem",
                    "--verb","4"
                };
     
     openvpnclass = jc;
     openvpnjenv= env;
     int argc=17;

    return callmain(argc,argv);
 }

void Java_de_blinkt_openvpn_OpenVPN_startOpenVPNThreadArgs(JNIEnv *env,jclass jc, jobjectArray stringArray) {
    openvpnclass = jc;
    openvpnjenv= env;
    
    int stringCount = (*env)->GetArrayLength(env, stringArray);
    
    
    const char** argv = calloc(stringCount,sizeof(const char*));
    
    int i;
    for (i=0; i<stringCount; i++) {
        jstring string = (jstring) (*env)->GetObjectArrayElement(env, stringArray, i);
        jboolean isCopy;
        const char* rawString = (*env)->GetStringUTFChars(env, string, &isCopy);
        
        // Copy the string to able to release it
        argv[i] = rawString;

    }
    
    // Call main
    callmain(stringCount,argv);
    
    // Release the Strings
    for(i=0; i<stringCount;i++){
        jstring string = (jstring) (*env)->GetObjectArrayElement(env, stringArray, i);
        (*env)->ReleaseStringUTFChars(env,string,argv[i]);
    }
    free(argv);
}
    



jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    __android_log_write(ANDROID_LOG_DEBUG,"openvpn", "Loading openvpn native library $id$ compiled on "   __DATE__ " " __TIME__ );
    return JNI_VERSION_1_2;
}

void addInterfaceInformation(int mtu,const char* ifconfig_local, const char* ifconfig_remote)
{
    jstring jlocal = (*openvpnjenv)->NewStringUTF(openvpnjenv, ifconfig_local);
    jstring jremote = (*openvpnjenv)->NewStringUTF(openvpnjenv, ifconfig_remote);

    jmethodID aMethodID = (*openvpnjenv)->GetStaticMethodID(openvpnjenv, openvpnclass, "addInterfaceInfo", 
                                                            "(ILjava/lang/String;Ljava/lang/String;)V");
    (*openvpnjenv)->CallStaticVoidMethod(openvpnjenv,openvpnclass,aMethodID,mtu,jlocal,jremote);
    
    (*openvpnjenv)->DeleteLocalRef(openvpnjenv,jlocal);
    (*openvpnjenv)->DeleteLocalRef(openvpnjenv,jremote);

    
}

void android_openvpn_log(int level,const char* prefix,const char* prefix_sep,const char* m1)
{
    __android_log_print(ANDROID_LOG_DEBUG,"openvpn","%s%s%s",prefix,prefix_sep,m1);

    jstring jprefix = (*openvpnjenv)->NewStringUTF(openvpnjenv, prefix);
    jstring jmessage = (*openvpnjenv)->NewStringUTF(openvpnjenv, m1);
    
    jmethodID aMethodID = (*openvpnjenv)->GetStaticMethodID(openvpnjenv, openvpnclass, "logMessage", 
                                                            "(ILjava/lang/String;Ljava/lang/String;)V");
    
    (*openvpnjenv)->CallStaticVoidMethod(openvpnjenv,openvpnclass,aMethodID,level,jprefix,jmessage);
    
    (*openvpnjenv)->DeleteLocalRef(openvpnjenv,jprefix);
    (*openvpnjenv)->DeleteLocalRef(openvpnjenv,jmessage);
    
}

int android_open_tun () {
    jmethodID aMethodID = (*openvpnjenv)->GetStaticMethodID(openvpnjenv, openvpnclass, "openTunDevice", 
                                                            "()I");
    return (*openvpnjenv)->CallStaticIntMethod(openvpnjenv,openvpnclass,aMethodID);

}


unsigned char android_protect_socket(int sockfd) {
    jmethodID aMethodID = (*openvpnjenv)->GetStaticMethodID(openvpnjenv, openvpnclass, "protectSocket", 
                                                            "(I)Z");
    return (*openvpnjenv)->CallStaticBooleanMethod(openvpnjenv,openvpnclass,aMethodID,sockfd);
    
}


void android_set_dns(const char* dns) {
    jmethodID aMethodID = (*openvpnjenv)->GetStaticMethodID(openvpnjenv, openvpnclass, "addDns", 
                                                            "(Ljava/lang/String;)V");
    jstring jdns = (*openvpnjenv)->NewStringUTF(openvpnjenv,dns);
    (*openvpnjenv)->CallStaticVoidMethod(openvpnjenv,openvpnclass,aMethodID,jdns);
    
    
}

void android_set_domain(const char* domain) {
    jmethodID aMethodID = (*openvpnjenv)->GetStaticMethodID(openvpnjenv, openvpnclass, "addDomain", 
                                                            "(Ljava/lang/String;)V");
    jstring jdomain = (*openvpnjenv)->NewStringUTF(openvpnjenv,domain);
    (*openvpnjenv)->CallStaticVoidMethod(openvpnjenv,openvpnclass,aMethodID,jdomain);
    
    
}

void addRouteInformation(const char* dest, const char* mask, const char* gw) {
    
    jstring jmask =  (*openvpnjenv)->NewStringUTF(openvpnjenv, mask);
    jstring jdest =  (*openvpnjenv)->NewStringUTF(openvpnjenv, dest);
    jstring jgw =    (*openvpnjenv)->NewStringUTF(openvpnjenv, gw);
    jmethodID aMethodID = (*openvpnjenv)->GetStaticMethodID(openvpnjenv, openvpnclass, "addRoute", 
                                            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    (*openvpnjenv)->CallStaticVoidMethod(openvpnjenv,openvpnclass,aMethodID,jdest,jmask,jgw);

    (*openvpnjenv)->DeleteLocalRef(openvpnjenv,jmask);
    (*openvpnjenv)->DeleteLocalRef(openvpnjenv,jdest);
    (*openvpnjenv)->DeleteLocalRef(openvpnjenv,jgw);


}

