windres ani.rc -O coff -o anires.obj
rem If RM_DISPLAY is defined, will ask the display size.
tcc ani.c anires.obj -lmsimg32 -D RM_DISPLAY -o ram.exe
rem tcc ani.c anires.obj -lmsimg32 
upx --best ram.exe
