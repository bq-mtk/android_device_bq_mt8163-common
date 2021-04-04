#include <stdint.h>
#include <log/log.h>

#define LOG_TAG "CRYPTO"

extern "C"
{
    void CRYPTO_free(void *str) {
        ALOGD("CRYPTO_free: str=%s", str);
        free(str);
    }

    int CRYPTO_malloc(int num, const char *file, int line) {
        ALOGD("CRYPTO_malloc: num=%d file=%s line=%d", num, file, line);
        return malloc(num);
    }

    void CRYPTO_lock(int mode, int type, const char *file, int line) {
        ALOGD("CRYPTO_lock: mode=%d type=%d file=%s line=%d", mode, type, file, line);
    }
}
