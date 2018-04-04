#ifndef LOG_H_
#define LOG_H_

#ifndef LOGGING_OFF
#define LOGGING
#endif


#ifdef LOGGING

#define LOG_FILE stderr

#define LOG(...) \
   fprintf(LOG_FILE, __VA_ARGS__); \
   fflush(LOG_FILE);

#else

#define LOG(...)

#endif


#endif
