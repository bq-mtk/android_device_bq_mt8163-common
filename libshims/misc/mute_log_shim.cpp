// Note for MsensorDaemon: Nobody cares if you're not using LIBC_PROPERTIES, just shut the fuck up.
extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt, ...) { return 0; }
