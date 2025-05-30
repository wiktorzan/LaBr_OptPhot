# $Id: GNUmakefile,v 1.15 2008/06/11 22:15:20 maire Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

name := LaBr3_V2
G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif

.PHONY: all
all: lib bin

#### G4ANALYSIS_USE := true

include $(G4INSTALL)/config/architecture.gmk

###ROOT
ROOT_CFLAGS := $(shell root-config --cflags)
ROOT_LIBS   := $(shell root-config --libs)
ROOT_GLIBS  := $(shell root-config --glibs)

CXXFLAGS += $(ROOT_CFLAGS)
CPPFLAGS += $(ROOT_CFLAGS)
CPPFLAGS += -Wl,--no-as-needed
LDFLAGS  += $(ROOT_LIBS)
LDFLAGS  += $(ROOT_GLIBS)



include $(G4INSTALL)/config/binmake.gmk

histclean:
	rm -f $(G4WORKDIR)/tmp/$(G4SYSTEM)/$(G4TARGET)/RunAction.o
	rm -f $(G4WORKDIR)/tmp/$(G4SYSTEM)/$(G4TARGET)/EventAction.o
			
visclean:
	rm -f g4*.prim g4*.eps g4*.wrl
	rm -f .DAWN_*
