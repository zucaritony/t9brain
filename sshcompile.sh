#!/bin/bash

scp t9.cpp numpad.cpp root@192.168.0.202:~/t9
ssh -X root@192.168.0.202 "cd ~/t9 && g++ -c t9.cpp -L/usr/bin/X11R6/lib -lX11 -pthread && g++ numpad.cpp -L/usr/bin/X11R6/lib -lX11 -pthread && ./a.out"
