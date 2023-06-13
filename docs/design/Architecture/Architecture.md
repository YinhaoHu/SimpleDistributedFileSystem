# Architecture

---

## Overview

* Client Application: The user who uses the SDFS.

* SDFS-Client-Side: Provide the SDFS interface for the Client Application and interacts with the server through the communication layer.

* Communication Layer: Complete the communication between the client and the server.(Protocol is SNFTP, Simple Distrubuted File System Network File Transport Protocol)

* SDFS-Server-Side: Response the request sent by the SDFS-Client-Side by interacting with the local file system.

* Basic Unix File System: A file system implemented similarly with the Unix File System. It is the local file system of the server in the SDFS architecture.

---
