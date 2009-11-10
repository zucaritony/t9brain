#!/bin/bash

scp numpad.cpp root@192.168.0.202:~/t9/pad
ssh -X root@192.168.0.202 "cd ~/t9/pad && g++ numpad.cpp -L/usr/bin/X11R6/lib -lX11 && DISPLAY=:0 ./a.out"
