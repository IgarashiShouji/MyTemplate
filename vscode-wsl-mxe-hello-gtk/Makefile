# Makefile for MXE + WSL + msys2
#
# build: g++ on MXE
# debug: gdb on msys2(mingw64)

#x86_64-w64-mingw32.static-g++ -std=c++17 -o test3.exe  test3.cpp  `x86_64-w64-mingw32.static-pkg-config --cflags --libs gtkmm-3.0` &
CC=/opt/mxe/usr/bin/x86_64-w64-mingw32.static-g++
CFLAGS=-g -std=c++17 `x86_64-w64-mingw32.static-pkg-config --cflags gtkmm-3.0`
LIBS=`x86_64-w64-mingw32.static-pkg-config --libs gtkmm-3.0` -lboost_program_options-mt-x64
#CFLAGS=-g `/opt/mxe/usr/bin/x86_64-w64-mingw32.static-pkg-config --cflags --libs gtkmm-3.0`
#CFLAGS=-g `/opt/mxe/usr/bin/x86_64-w64-mingw32.static-pkg-config --cflags --libs gtkmm-3.0 glibmm-2.4 gtk+-3.0 glib-2.0`
#CFLAGS=-g -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/gtkmm-3.0 -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/gtkmm-3.0/include -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/atkmm-1.6 -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/atkmm-1.6/include -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/gdkmm-3.0 -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/gdkmm-3.0/include -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/giomm-2.4 -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/giomm-2.4/include -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/pangomm-1.4 -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/pangomm-1.4/include -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/cairomm-1.0 -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/cairomm-1.0/include -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/glibmm-2.4 -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/glibmm-2.4/include -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/sigc++-2.0 -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/sigc++-2.0/include -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/gtk-3.0 -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/pango-1.0 -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/harfbuzz -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/cairo -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/pixman-1  -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/freetype2 -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/gdk-pixbuf-2.0 -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/libpng16 -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/webp -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/atk-1.0 -DEPOXY_STATIC -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/fribidi -DFRIBIDI_LIB_STATIC -I/opt/mxe/usr/x86_64-w64-mingw32.static/include/glib-2.0 -pthread -I/opt/mxe/usr/x86_64-w64-mingw32.static/lib/glib-2.0/include -mms-bitfields -DPCRE_STATIC -lgtkmm-3.0 -latkmm-1.6 -lgdkmm-3.0 -lgiomm-2.4 -lpangomm-1.4 -lcairomm-1.0 -lglibmm-2.4 -lsigc-2.0 -lgtk-3 -lgdk-3 -limm32 -lshell32 -Wl,-luuid -ldwmapi -lsetupapi -lcfgmgr32 -lhid -lwinspool -lcomctl32 -lcomdlg32 -lpangocairo-1.0 -lpangowin32-1.0 -ldwrite -lpango-1.0 -lcairo-gobject -lcairo -lfontconfig -lharfbuzz -lfreetype -lbz2 -lharfbuzz_too -lfreetype_too -lbrotlidec -lbrotlicommon -lgdk_pixbuf-2.0 -lpng16 -ltiff -lwebp -lsharpyuv -llzma -ljpeg -latk-1.0 -lepoxy -lopengl32 -lfribidi -lgio-2.0 -ldnsapi -liphlpapi -lgobject-2.0 -L/opt/mxe/usr/x86_64-w64-mingw32.static/lib/../lib -lffi -lgmodule-2.0 -ldl -lglib-2.0 -lintl -liconv -pthread -lws2_32 -lwinmm -lpcre -lmsimg32 -lgdi32 -lpixman-1 -lexpat -lz -lm  -lole32 -lshlwapi -luuid  -lstdc++

all: hello.exe

clean:
	/bin/rm hello.exe

hello.exe: hello.cpp
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
