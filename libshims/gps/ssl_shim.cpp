#define LOG_TAG "SSL_ctrl_stub"
#include <log/log.h>
#include <string.h>

extern "C"
{
    long SSL_ctrl(int *ssl, int cmd, long larg, void *parg) {
        ALOGD("SSL_ctrl: ssl=%p cmd=%d larg=%ld parg=%p", ssl, cmd, larg, parg);
        return -1;
    }

    long SSL_CTX_ctrl(int *ctx, int cmd, long larg, void *parg) {
        ALOGD("SSL_CTX_ctrl: ctx=%p cmd=%d larg=%ld parg=%p", ctx, cmd, larg, parg);
        return -1;
    }
}
