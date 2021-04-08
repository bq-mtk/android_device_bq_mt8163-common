#define LOG_TAG "IMtkHdmiClient"
#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <IMtkHdmiClient.h>
/*****************************************************************************
*                          C O N S T A N T S
******************************************************************************
*/

namespace android
{


/*****************************************************************************
*                         D A T A   T Y P E S
******************************************************************************
*/
enum
{
    NOTIFY_EVENT = IBinder::FIRST_CALL_TRANSACTION,
};


/*****************************************************************************
*                        C L A S S   D E F I N I T I O N
******************************************************************************
*/

class BpMtkHdmiClient : public BpInterface<IMtkHdmiClient>
{
public:
    BpMtkHdmiClient(const sp<IBinder>& impl)
        : BpInterface<IMtkHdmiClient>(impl)
    {
    }
    virtual void onHdmiEvent(int event)
    {
        Parcel data, reply;
        ALOGD("[BpMtkHdmiClient]onHdmiEvent=%d", event);
        data.writeInterfaceToken(IMtkHdmiClient::getInterfaceDescriptor());
        ALOGD("[BpMtkHdmiClient]onHdmiEvent 1=%d", event);
        data.writeInt32(event);
        ALOGD("[BpMtkHdmiClient]onHdmiEvent readInt =%d", data.readInt32());
        remote()->transact(NOTIFY_EVENT, data, &reply);
    }

};
IMPLEMENT_META_INTERFACE(MtkHdmiClient, "hdmi.IMtkHdmiClient");

// ----------------------------------------------------------------------

status_t BnMtkHdmiClient::onTransact(
    uint32_t code, const Parcel &data, Parcel *reply, uint32_t flags)
{
    switch(code)
    {
        case NOTIFY_EVENT:
        {
            CHECK_INTERFACE(IMtkHdmiClient, data, reply);
            int event = data.readInt32();
            ALOGD("[BnMtkHdmiClient 1]onHdmiEvent=%d", event);
            onHdmiEvent(event);
            return NO_ERROR;
        }
        default:
            ALOGD("[BnMtkHdmiClient 2]onTransact");
            return BBinder::onTransact(code, data, reply, flags);
    }
}

// ----------------------------------------------------------------------------

}; // namespace android

