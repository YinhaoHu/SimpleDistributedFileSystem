#ifndef _SDFS_CONFIG_H_
#define _SDFS_CONFIG_H_
  
#ifdef _TEST_CONFIG

#define SDFS_SERVER_LOG_FILE_NAME "test/logFile"
#define SDFS_SERVER_FILE_TIMESTAMPS_FILE_NAME "test/timeStampsFile"

#else

#define SDFS_SERVER_LOG_FILE_NAME "logFile"
#define SDFS_SERVER_FILE_TIMESTAMPS_FILE_NAME "timeStampsFile"

#endif

#endif