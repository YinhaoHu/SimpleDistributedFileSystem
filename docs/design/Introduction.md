# Simple Distributed File System

---  

## Overview

Simple Distributed File System is a basic network file system. It supports to serve multiple clients in one server simultaneously. While developing, I tried as many test cases as possible. I'll use SDFS refer to the Simple Distributed File System. 

---

## Basic Information

* Model: Client - Server model

* Client Operations: init, open, read, write, close, create, state, unlink

* Server LocalFileSystem: basic unix file system

* Workload:
  
  * Simple thread pool.

---

## Work Flow

* Stage 1: Determine the basic information

* Stage 2: Design the architecture

* Stage 3: In component level, design, code, test.

* Stage 4: Arrange the project, test and evaluate entirely

* Stage 5: Build up the exhibition platform for this project

---

## Hightlights

* Autonomous designed network application-layer protocol, SNFTP.

* Modularity and common name convention for project.

* Makefile.

* Server failure check.

* Read buffer concept.
