TARGET = ani_c.exe

SOURCES = \
          ani.c

LIBS = \
          kernel32.lib \
          user32.lib \
          gdi32.lib \
          msimg32.lib

RC =      ani.rc

all : app

app : $(SOURCES)
  rc /Fo ani.res $(RC)
  cl -nologo /Fe$(TARGET) -Gm- -GR- -EHa- -GS- -Oi -Tc $(SOURCES) /link -nodefaultlib /ENTRY:_start -subsystem:windows ani.res $(LIBS)

