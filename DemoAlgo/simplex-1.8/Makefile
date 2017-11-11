UNAME = $(shell uname)
ifeq ("$(UNAME)", "SunOS")
  BLADROOT = ${HOME}/biblad64
  CPPFLAGS = -DBA0_64BITS -I/usr/local/include/gsl -I${BLADROOT}/include
  CFLAGS   = -g -Wall ${CFLAGS64} -Wall
  LDFLAGS  = -g ${LDFLAGS64} -L/usr/local/lib -Wl,-rpath /usr/local/lib \
		-L${BLADROOT}/lib -Wl,-rpath ${BLADROOT}/lib
else
  BLADROOT = ${HOME}/biblad32
  CPPFLAGS = -I${BLADROOT}/include
  CFLAGS   = -g -Wall 
  LDFLAGS  = -static -g -L${BLADROOT}/lib -Wl,-rpath ${BLADROOT}/lib
endif

BLADROOT = ${HOME}/blad-install
CPPFLAGS = -I${BLADROOT}/include
CFLAGS   = -g -Wall
LDFLAGS  = -static -g -L${BLADROOT}/lib -Wl,-rpath ${BLADROOT}/lib

CC       = gcc
LIBS     = -lbad -lbap -lbav -lba0 -lgmp 

# $< = premier prerequis
# $^ = tous les prerequis
# $@ = cible

.PHONY: all clean

all: 
	$(MAKE) simplex
clean:
	-rm simplex 

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS)
