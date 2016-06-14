TPI Firefly Criptoaritmetica IA 2016:
=====================================

El programa fue compilado en debian linux wheezy, utilizando gcc y la herramienta make.

El programa utiliza la libreria LibGTK2+, para compilar en linux es necesario bajar las siguientes dependencias, desde consola:

sudo apt-get install build-essential
sudo apt-get install libgtk2.0-dev

Para compilar, desde consola y en el directorio del source, escribir "make" (sin comillas).

Incluye el makefile para compilar win32 desde linux (makefile.win32). 

Se debe ejecutar make -f makefile.win32.

Pero es necesario armar el toolchain para win32, para eso en estos links se explica detalladamente:

http://arrayfire.com/cross-compile-to-windows-from-linux/
http://ftp.acc.umu.se/pub/GNOME/binaries/win32/

e instalar el compilador mingw, desde consola ejecutar:
sudo apt-get install mingw-w64
