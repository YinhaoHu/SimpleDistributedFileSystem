#ifndef _SDFS_SNFTP_ERRNO_H_
#define _SDFS_SNFTP_ERRNO_H_

#define SDFS_SERVER_CRASH               (0x8fffffff)
 

#define SDFS_LOOKUP_PINUM_DIR           (0x80001002)
#define SDFS_LOOKUP_PINUM_EXIST         (0x80001003)
#define SDFS_LOOKUP_NOT_FOUND           (0x80001004)

#define SDFS_CREATE_SPACE               (0x80003011)
#define SDFS_CREATE_LONG_NAME           (0x80003012)
#define SDFS_CREATE_INUM_NDIR           (0x80003013)
#define SDFS_CREATE_INUM_EXIST          (0x80003014)
#define SDFS_CREATE_NAME_EXIST          (0x80003015)


#define SDFS_WRITE_INUM_EXIST           (0x80004011)
#define SDFS_WRITE_INUM_TYPE            (0x80004012)
#define SDFS_WRITE_OFFSET               (0x80004013) 


#define SDFS_READ_INUM_EXIST            (0x80005011)
#define SDFS_READ_INUM_TYPE             (0x80005012)
#define SDFS_READ_OFFSET                (0x80005013)


#define SDFS_UNLINK_PINUM_EXIST         (0x80006011)
#define SDFS_UNLINK_PINUM_TYPE          (0x80006012)
#define SDFS_UNLINK_NAME_LONG           (0x80006013)
#define SDFS_UNLINK_NOT_EXIST           (0x80006014)
#define SDFS_UNLINK_NON_EMPTY_DIR       (0x80006015)


#define SDFS_LIST_DINUM_EXIST           (0x80007001)
#define SDFS_LIST_DINUM_TYPE            (0x80007002)

#endif