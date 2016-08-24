#!/bin/bash
clear
gcc -I/usr/include/cairo/ -I/usr/include/gtk-2.0/ -I/usr/include/atk-1.0 -I/usr/lib/x86_64-linux-gnu/gtk-2.0/include/ -I/usr/include/pango-1.0/  -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ -I/usr/include/gimp-2.0 -I /usr/local/cuda/include/ plugin.c -L /lib64 -L/usr/lib/ -L/usr/lib/x86_64-linux-gnu/ -L /usr/local/cuda/lib/  -L/usr/lib/x86_64-linux-gnu/ -lgtk-x11-2.0 -lgimp-2.0 -lglib-2.0 -lgobject-2.0 -lOpenCL -lgimpui-2.0 -lgimpwidgets-2.0 -o plugin
cp plugin /home/marek/.gimp-2.8/plug-ins/
cp kernel.cl /home/marek/.gimp-2.8/plug-ins/
