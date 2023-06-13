# SDFS (Simple Distributed File System)

## Preface
   This is a simple project which involved the basic computer networking stuff and computer operating system component `file system`.
The basic idea is from the OSTEP project: ditributed file system. But I changed something because there are no test cases to restrict
my codes. The running and developing environment is Linux.

## Overview
  File system is a way that operating system stores and organizes the datas. For example, windows often uses the NTFS(New Technology
File System) to do that thing.There are many supported filesystems on Linux, such as ext3, ext4, NFS, UFS and so on. In this project,
I implemented a basic unix file system. For more information, see the related documentation.

  File server is a kind of servers that takes the responsibility to store files, manage files and make files accessible for the clients
over the network. In this project, I used a simple thread pool.

  Application-layer network protocol is often designed by the application developer to define the rules, formats and procedures that
different hosts uses to communicate with other in a system. In this project, I designed SNFTP(Simple Distributed File System Network
File Transfer protocol) as out application-layer network protocol, see the related documentation for moreinformation like the procedures, 
behaviours, features and so on. I used `communication layer` in the architecture to specify the place where the SNFTP resides.

  Shared library is a library that can be shared for multiple processes. Symbols are used by a program from the shared libary are load 
at runtime. This is what SDFS supported to its user.
  
  For more information about this project, go to the `docs/design` direcotry and dig out more stuff.

## Website
  You can find the website of this project in the second entry of `My works` on www.centhoo.top.

## More
  It not easy to build a perfect system even impossible, so if you find any bugs or have any suggestions, feel free to tell the maintainer
or disscuss in the `issues` section of this project.
