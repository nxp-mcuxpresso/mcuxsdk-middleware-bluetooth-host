https://suryagaddipati.wordpress.com/2008/03/12/creating-python-extensions-in-cc-with-swig-on-windows/



pexports python27.dll > python27.def
dlltool --dllname python27.dll --input-def python27.def --output-lib libpython27.a


Add for C++ compilation:   -D_hypot=hypot





-----------
64 Bit version:


info: https://stackoverflow.com/questions/17988904/compile-64-bit-binary-with-mingw-dev-c

use TDM-GCC64 as compiler (only gcc compiler on windows for 64bit generation)
Use installed Python in 64 bit version

http://tdm-gcc.tdragon.net/

# anaconda (python2.7) 64 bit download
https://www.anaconda.com/download/


export PATH=/c/TDM-GCC-64/bin:$PATH



https://github.com/kivy/kivy/wiki/Creating-a-64-bit-development-environment-with-MinGW-on-Windows

gendef python27.dll
dlltool --dllname python27.dll --input-def python27.def --output-lib libpython27.a


 try to add -D MS_WIN64 to your command line
 
