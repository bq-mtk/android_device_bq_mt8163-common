#ifndef ANDROID_IMTKHDMICLIENT_H
#define ANDROID_IMTKHDMICLIENT_H

#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <binder/IInterface.h>
namespace android
{

class IMtkHdmiClient : public IInterface
{
public:
    DECLARE_META_INTERFACE(MtkHdmiClient);

    //hdmi control
    virtual void onHdmiEvent(int event) = 0;

};


class BnMtkHdmiClient : public BnInterface<IMtkHdmiClient>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; // namespace android
#endif // ANDROID_IMTKHDMICLIENT_H