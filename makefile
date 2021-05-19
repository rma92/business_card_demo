TARGET = ani_c.exe

SOURCES = \
          ani.c

LIBS = \
          user32.lib \
          gdi32.lib \
          msimg32.lib

RC =      ani.rc

all : app

app : $(SOURCES)
  rc /Fo ani.res $(RC)
  cl /Fe$(TARGET) $(SOURCES) /link ani.res $(LIBS)
