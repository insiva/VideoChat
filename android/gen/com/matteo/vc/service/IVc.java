/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/zzh/workspace/VideoChat/android/src/com/matteo/vc/service/IVc.aidl
 */
package com.matteo.vc.service;
public interface IVc extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.matteo.vc.service.IVc
{
private static final java.lang.String DESCRIPTOR = "com.matteo.vc.service.IVc";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.matteo.vc.service.IVc interface,
 * generating a proxy if needed.
 */
public static com.matteo.vc.service.IVc asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.matteo.vc.service.IVc))) {
return ((com.matteo.vc.service.IVc)iin);
}
return new com.matteo.vc.service.IVc.Stub.Proxy(obj);
}
@Override public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_makeCall:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
int _result = this.makeCall(_arg0);
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_handleCall:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
int _result = this.handleCall(_arg0);
reply.writeNoException();
reply.writeInt(_result);
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.matteo.vc.service.IVc
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
@Override public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
@Override public int makeCall(int ssrc) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(ssrc);
mRemote.transact(Stub.TRANSACTION_makeCall, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int handleCall(int action) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(action);
mRemote.transact(Stub.TRANSACTION_handleCall, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
}
static final int TRANSACTION_makeCall = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_handleCall = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
}
public int makeCall(int ssrc) throws android.os.RemoteException;
public int handleCall(int action) throws android.os.RemoteException;
}
