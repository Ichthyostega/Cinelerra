include ../../global_config

FFTW_PATH := $(shell expr ../../fftw* )
OBJS = $(OBJDIR)/fourier.o \
	$(OBJDIR)/pitch.o


OUTPUT = ../$(OBJDIR)/pitch.plugin

LFLAGS += $(FFTW_PATH)/.libs/libfftw3.a
CFLAGS += -I $(FFTW_PATH)/api

include ../../plugin_config


$(OBJDIR)/pitch.o: pitch.C
