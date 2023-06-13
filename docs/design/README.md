# Simple Distributed File System

---  

## Overview

Simple Distributed File System is a basic network file system. It supports to serve multiple clients in one server simultaneously. While developing, I tried as many test cases as possible. I'll use SDFS refer to the Simple Distributed File System. 

---

## Basic Information

* Model: Client - Server model

* Client Operations: init, open, read, write, close, create, state, unlink

* Server Local File System: basic unix file system

* Communication layer: SNFTP.

* Server: simple thread pool.
---

