CC=mpicc

LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lGLEW -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L../libs/ilclient -L../libs/vgfont
INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I../libs/ilclient -I../libs/vgfont
CFLAGS= -DRASPI -mfloat-abi=hard -mfpu=vfp -O3 -lm -ffast-math

all:
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) ogl_utils.c egl_utils.c lodepng.c image_gl.c cursor_gl.c launcher.c -o bin/launcher.out
clean:
	rm -f ./bin/sph.out
	rm -f ./*.o
