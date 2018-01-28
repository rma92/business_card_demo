windres ani.rc -O coff -o anires.obj
tcc ani.c anires.obj -lmsimg32
upx -9 ani.exe
