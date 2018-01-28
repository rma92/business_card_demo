windres ani.rc -O coff -o anires.obj
rem If RM_DISPLAY is defined, will ask the display size.
tcc ani.c anires.obj -lmsimg32 -D RM_DISPLAY
rem tcc ani.c anires.obj -lmsimg32 
upx -9 ani.exe
