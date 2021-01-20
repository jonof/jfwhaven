# Witchaven Makefile for GNU Make

# Create Makefile.user yourself to provide your own overrides
# for configurable values
-include Makefile.user

##
##
## CONFIGURABLE OPTIONS
##
##

# Debugging options
RELEASE ?= 1

# Path where game data is installed
DATADIR ?= /usr/local/share/games/jfwhaven

# Engine source code path
EROOT ?= jfbuild

# Engine options
#  USE_POLYMOST   - enables Polymost renderer
#  USE_OPENGL     - enables OpenGL support in Polymost
#     Define as 0 to disable OpenGL
#     Define as USE_GL2 (or 1 or 2) for GL 2.0/2.1 profile
#     Define as USE_GLES2 (or 12) for GLES 2.0 profile
#  USE_ASM        - enables the use of assembly code
USE_POLYMOST ?= 1
USE_OPENGL ?= 1
USE_ASM ?= 1


##
##
## HERE BE DRAGONS
##
##

# build locations
SRC=src
RSRC=rsrc
EINC=$(EROOT)/include
ELIB=$(EROOT)
INC=$(SRC)
o=o
res=o

ifneq (0,$(RELEASE))
  # debugging disabled
  debug=-fomit-frame-pointer -O2
else
  # debugging enabled
  debug=-ggdb -Og
endif

CC?=gcc
CXX?=g++
NASM?=nasm
RC?=windres
OURCFLAGS=$(debug) -W -Wall -Wimplicit -Wno-unused \
	-fno-strict-aliasing -DNO_GCC_BUILTINS \
	-I$(INC) -I$(EINC)
OURCXXFLAGS=-fno-exceptions -fno-rtti
LIBS=-lm
GAMELIBS=
NASMFLAGS=-s #-g
EXESUFFIX=

GAMEOBJS= \
	$(SRC)/b5compat.$o \
	$(SRC)/config.$o \
	$(SRC)/whani.$o \
	$(SRC)/whaven.$o \
	$(SRC)/whfx.$o \
	$(SRC)/whinp.$o \
	$(SRC)/whmenu.$o \
	$(SRC)/whnet.$o \
	$(SRC)/whobj.$o \
	$(SRC)/whplr.$o \
	$(SRC)/whsndmod.$o \
	$(SRC)/whtag.$o

EDITOROBJS=$(SRC)/whbuild.$o

include $(EROOT)/Makefile.shared

ifeq ($(PLATFORM),LINUX)
	NASMFLAGS+= -f elf
endif
ifeq ($(PLATFORM),BSD)
	NASMFLAGS+= -f elf
	GAMELIBS+= -pthread
endif
ifeq ($(PLATFORM),WINDOWS)
	OURCFLAGS+= -I$(DXROOT)/include
	NASMFLAGS+= -f win32 --prefix _
	GAMEOBJS+= $(SRC)/gameres.$(res) $(SRC)/startwin_game.$o
	EDITOROBJS+= $(SRC)/buildres.$(res)
endif

ifeq ($(RENDERTYPE),SDL)
	OURCFLAGS+= $(SDLCONFIG_CFLAGS)
	LIBS+= $(SDLCONFIG_LIBS)

	ifeq (1,$(HAVE_GTK))
		OURCFLAGS+= $(GTKCONFIG_CFLAGS)
		LIBS+= $(GTKCONFIG_LIBS)
		GAMEOBJS+= $(SRC)/startgtk_game.$o $(RSRC)/startgtk_game_gresource.$o
		EDITOROBJS+= $(RSRC)/startgtk_build_gresource.$o
	endif

	GAMEOBJS+= $(RSRC)/sdlappicon_game.$o
	EDITOROBJS+= $(RSRC)/sdlappicon_build.$o
endif

# Source-control version stamping
ifneq (,$(findstring git version,$(shell git --version)))
GAMEOBJS+= $(SRC)/version-auto.$o
EDITOROBJS+= $(SRC)/version-auto.$o
else
GAMEOBJS+= $(SRC)/version.$o
EDITOROBJS+= $(SRC)/version.$o
endif

OURCFLAGS+= $(BUILDCFLAGS)
LIBS+= $(BUILDLIBS)

ifneq ($(PLATFORM),WINDOWS)
	OURCFLAGS+= -DDATADIR=\"$(DATADIR)\"
endif

.PHONY: clean all engine $(ELIB)/$(ENGINELIB) $(ELIB)/$(EDITORLIB)

# TARGETS

# Invoking Make from the terminal in OSX just chains the build on to xcode
ifeq ($(PLATFORM),DARWIN)
ifeq ($(RELEASE),0)
style=Debug
else
style=Release
endif
.PHONY: alldarwin
alldarwin:
	cd xcode && xcodebuild -project whaven.xcodeproj -target all -configuration $(style)
endif

all: whaven$(EXESUFFIX) build$(EXESUFFIX)

whaven$(EXESUFFIX): $(GAMEOBJS) $(ELIB)/$(ENGINELIB)
	$(CXX) $(CXXFLAGS) $(OURCXXFLAGS) $(OURCFLAGS) -o $@ $^ $(LIBS) $(GAMELIBS) -Wl,-Map=$@.map

build$(EXESUFFIX): $(EDITOROBJS) $(ELIB)/$(EDITORLIB) $(ELIB)/$(ENGINELIB)
	$(CXX) $(CXXFLAGS) $(OURCXXFLAGS) $(OURCFLAGS) -o $@ $^ $(LIBS) -Wl,-Map=$@.map

include Makefile.deps

.PHONY: enginelib editorlib
enginelib editorlib:
	$(MAKE) -C $(EROOT) \
		USE_POLYMOST=$(USE_POLYMOST) \
		USE_OPENGL=$(USE_OPENGL) \
		USE_ASM=$(USE_ASM) \
		RELEASE=$(RELEASE) $@
$(EROOT)/generatesdlappicon$(EXESUFFIX):
	$(MAKE) -C $(EROOT) generatesdlappicon$(EXESUFFIX)

$(ELIB)/$(ENGINELIB): enginelib
$(ELIB)/$(EDITORLIB): editorlib

# RULES
$(SRC)/%.$o: $(SRC)/%.nasm
	$(NASM) $(NASMFLAGS) $< -o $@

$(SRC)/%.$o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(OURCFLAGS) -c $< -o $@
$(SRC)/%.$o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(OURCXXFLAGS) $(OURCFLAGS) -c $< -o $@

$(SRC)/%.$(res): $(SRC)/%.rc
	$(RC) -i $< -o $@ --include-dir=$(EINC) --include-dir=$(SRC)

$(SRC)/%.$o: $(SRC)/util/%.c
	$(CC) $(CFLAGS) $(OURCFLAGS) -c $< -o $@

$(RSRC)/%.$o: $(RSRC)/%.c
	$(CC) $(CFLAGS) $(OURCFLAGS) -c $< -o $@

$(RSRC)/%_gresource.c: $(RSRC)/%.gresource.xml
	glib-compile-resources --generate --manual-register --c-name=startgtk --target=$@ --sourcedir=$(RSRC) $<
$(RSRC)/%_gresource.h: $(RSRC)/%.gresource.xml
	glib-compile-resources --generate --manual-register --c-name=startgtk --target=$@ --sourcedir=$(RSRC) $<
$(RSRC)/sdlappicon_%.c: $(RSRC)/%.png $(EROOT)/generatesdlappicon$(EXESUFFIX)
	$(EROOT)/generatesdlappicon$(EXESUFFIX) $< > $@

# PHONIES
clean:
ifeq ($(PLATFORM),DARWIN)
	cd osx && xcodebuild -target All clean
else
	-rm -f $(GAMEOBJS) $(EDITOROBJS)
	$(MAKE) -C $(EROOT) clean
endif

veryclean: clean
ifeq ($(PLATFORM),DARWIN)
else
	-rm -f whaven$(EXESUFFIX) build$(EXESUFFIX) core*
	$(MAKE) -C $(EROOT) veryclean
endif

.PHONY: $(SRC)/version-auto.c
$(SRC)/version-auto.c:
	printf "const char *game_version = \"%s\";\n" "$(shell git describe --always || echo git error)" > $@
	echo "const char *game_date = __DATE__;" >> $@
	echo "const char *game_time = __TIME__;" >> $@
