/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.10
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */

#ifndef SWIG_VC_WRAP_H_
#define SWIG_VC_WRAP_H_

class SwigDirector_VcCallback : public VcCallback, public Swig::Director {

public:
    void swig_connect_director(JNIEnv *jenv, jobject jself, jclass jcls, bool swig_mem_own, bool weak_global);
    SwigDirector_VcCallback(JNIEnv *jenv);
    virtual void onIncoming(VcCall *arg0);
    virtual void onConfirmed(VcCall *arg0);
    virtual void onEstablished(VcCall *arg0);
    virtual void onOutgoFail(VcCall *arg0);
    virtual void onTimeout(VcCall *arg0);
    virtual void onDisconnect(VcCall *arg0);
public:
    bool swig_overrides(int n) {
      return (n < 6 ? swig_override[n] : false);
    }
protected:
    Swig::BoolArray<6> swig_override;
};


#endif
