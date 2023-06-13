# SDFS CommunicationLayer Documentation

---

## 1 Introduction

    In SDFS, I plan to design a simple and specific transport protocol called SDFS Network File Transport Protocol (SNFTP). SNFTP defines a series of protocol messages in the SDFS. In SDFS communication layer, I designed a few functions to support SNFTP, including wrapper, parse.

---

## 2 Protocol Specifications

### 2.1 Feature

#### 2.1.1 TCP

To keep the data transport reliable, the SNFTP is based on the TCP/IP protocol family. Especially, the IP is version 4.

#### 2.1.2 Stateless

For higher reliability, faster and simpler recovery. The SNFTP is stateless, which means the server side doesn't restore any state of the client, neither does the client side. 

### 2.1.3 RPC

Like the common network file system implementation, we choose RPC(Remote Procedure call) to support SDFS.

### 2.2 Server Procedures

Convention: The `Error` comes from the server which means it is not the client-side check-returned error.

#### 2.2.1 Lookup

* Description: To find the `name` entry in the parent directory whose inode number is specified by `pinum`.  

* Procedure Code: 0

* Client Behaviour

```
<0><2><int size><str size> \
<arg1 = pinum><arg2 = name>
```

* Server Behaviour

Server takes the responsibility to put the error code into the arg.

```
<0><1><int size>\
<inum OR error code> 
```

* Error: 
  
  * `pinum` is not a direcotry inode number.  (error 1)
  
  * `pinum` does not exist.(error 2)

### 2.2.2 Stat

* Description:  To get the state of the specified inode number `inum` and put it in the state buffer `buf`.

* Procedure code : 1

* Client behaviour

Client send the following message:

```
<1><1><Size>
<inum>
```

* Server behaviour

Server send the following message:

```
<1><3><Size><Size><Size> \
<inum> (Not found set -1) \ 
<type>  \ 
<size>  \
```

* Error:

    1) `inum` does not exist.

### 2.2.3 Create

* Description: To create a new file or direcotry under the parent direcotry.

* Procedure code: 2

* Client behaviour

Checks the name is NULL or not.     Return .

Checks the type is valid or not.    Return .

Send the following message.

```
<2><3><Size><Size><Size>
<pinum>
<name>
<type>
```

* Server behaviour

Send the following message.

```
<2><1><Size>
<ReturnCode>
```

* Error
  
  * pinum does not exist.
  
  * pinum is not a directory's inum.
  
  * name is too long.
  
  * no space

### 2.2.4 Read

* Description: Read the specific bytes in the given inode number.

* Procedure code: 3

* Client behaviour:

    Client first send testauth request to check if the local buffer is old, if so, client send another request to get the new file.

    Sends the following message:

```
<3><3><ArgSize><ArgSize><ArgSize>
<inum>
<offset>
<nbytes>
```

* Server behaviour: 

Send the following message:

```
<3><2><ArgSize><ArgSize>
<Return Value>
<Buffer>
```

* Error:
  
  * Inum does not eixsts
  
  * Inum is not a regular file.
  
  * Offset is illegal

### 2.2.5 Write

* Description: Write the specific content in a buffer to the client by a certain mechanism.

* Procedure code : 4

* Client behaviour

    Client-Side in SDFS write in direct-write way because I want the client to know what is going on immediately when it sends a write() request. In this way, we can instantly know if my write RPC is okay or not.

    Client sends the following message:

```
<4><ArgSize><ArgSie><ArgSize><ArgSize>
<inum> \ 
<offset> \
<buf bytes> \
<buffer> \
```

* Server behaviour

    Server checks the return code from the local BUFS call, update the update time information and sends back the following message:

```
<4><ArgSize>
<ReturnCode>
```

* Error
  
  * Inum does not exist.
  
  * Inum is not of regular file.
  
  * Offset is ilegal.
  
  * Buffer is too large.(This can just be detected by written bytes manually)

### 2.2.6 Unlink

* Unlink the specifed name in the direcotry given by pinum. 

* Procedure code: 5

* Client behaviour

Checkes if the name is empty string.If empty, return.

Send the following messages.

```
<5><2><ArgSize><ArgSize>
<dinum>
<name>
```

* Server behaviour

Send the following message.

```
<5><1><ArgSize>
<ReturnCode>
```

* Error
  
  * Pinum does not exist
  
  * Pinum is not of directory
  
  * name is too long
  
  * not found name.
  
  * the name is of non-empty direcotry.

### 2.2.7 List

* List the stas of the inodes in the given direcotry by an array.

* Procedure code: 6

* Client Behaviour

Send the following message

```
<6><1><ArgSize>
<DInum>
```

* Server Behaviour

Server sends the following message

```
<6><2><ArgSize><ArgSize><ArgSize>
<Return code>
<ArraySize> // If bad, no this
<Stats array>// If bad no this.
```

* Error
  
  * arg dinum does not exist
  
  * arg dinum is not a inode number of directory.

### 2.2.8 Validate

* Description: used to get the timestaps of one file from server in order to check if the file is old or new.

* Procedure code: 7

* Client Behaviour

Send the following message.

```
<7><1><ArgSize>
<Inum>
```

* Server behaviour

Send the following message.

```
<7><1><ArgSize>
<TimeStamp>//0 if inum does not exists
```

* Error
  
  * Inum does not exist.

### 2.3 Message Format

All of the messages transported on SNFTP should follow the format. This message format is a byte-based format, so the byte conversion needs consideration.

```
Header: <Procedure Code><Agrc><Arg1_bytes><Arg2_bytes><Arg..Bytes>\n
Body:<Arg1><Arg2><Arg.....>
```

Header: we set the fixed size for the header (64bytes)

ProceduerCode Field:  Bytes = 4   Type = sdfs_uint

Argc Field: Bytes = 4 Type = sdfs_uint

ArgItem_bytes Field: Bytes = 4 Type = sdfs_uint

Arg Field: Byte is specified in the arg_bytes array and type is defined by each procedure.

---

## 3 Convention

    Root inode number: 0

    Only Letters and numbers are allowed to used in the file name.(Or confusing errors will happen)

    No mutex lock for events like sdfs_read and sdfs_write, because it is ok that when server is handling one writting event while server sending the old data to the other clients who needs the data of the inode number.

## 4 Specification

Timeout = 3s | Consider the max file size is only 120KB

Retry Count = 3 

## 5 Problems

Some facts:

    Although the server closed the socket file descriptor, the client can still send all bytes.

 For Client-Side one remote procedure call:

* Connection closure: Client has not sent any request for a long time and the server closed the connection. This can be detected by returned-0 from recv() call on client-side. We can address the problem by this way: First, the client checks if the sockfd is ready to be read by using select(), if it is ready, we try to check if the connection is ok. Second, if the connection is bad, we try to reconnect, still use select(). If timeout, we retry this RPC operation(send and recv), if not timeout, we just recv.If recv all bytes, we return ok, else, we return bad.

* Server crash: serve process was killed, server machine is power off. This can be detected in the same way of the previous error detection PLUS the SDFS_BAD returned by sdfs_init().

Option explanation:

* Send: `bad` means that the network is in congestion that it cannot send the full data.

* Select: `Timeout` means that the server is unresponsive right now.

* Integrity: In fact, full or not was ensured by the protocol's message recv function  with the help of the socket option, so we can easily check the integrity by the return value from `sdfs_snftp_message_recv`.

Server Need to ensure the coming data is full and throw away the bad data.
