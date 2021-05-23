
# makefile 
# please at the first time only type to 'make tree' on bash console.

# Are you which select to use by the m68000 core?(only one)
 MV= 22
     ## [based on MUSASHI 2.0a(?)] (C)1998-1999 Karl Stenerud.@MAME(M22+)
#MV= 33
     ## [based on MUSASHI 3.3(?)]  (C)1998-2001 Karl Stenerud.@MAME(M33+)
#MV= 40
     ## [based on MUSASHI 3.3(?)]  (C)1998-2001 Karl Stenerud.@MAME(M44+v3)
#MV= 44
     ## [based on MUSASHI 3.3(?)]  (C)1998-2001 Karl Stenerud.@MAME(M44+v2)
#MV= 55
     ## <STILL NOT WORKED> [based on C68K (68000 CPU emulator) version 0.80 (from Gens??)] (C)2003-2004 Stephane Dallongeville.

# Are you which choosed to do use the z80 core?(only one)
#ZV= 01
     ## sz11 [based on Multi-Z80 32 Bit emulator(from pspDGEN1.00)]  (C)1996-2000 Neil Bradley.
#ZV= 02
     ## sz22 [based on Multi-Z80 32 Bit emulator(from pc-DGEN1.17)]  (C)1996-1999 Neil Bradley.
#ZV= 03
     ## pz11 [based on Portable Z80 emulator V2.8(3.2) V3.5(3.6)] (C)1998-2000 Juergen Buchmueller.@MAME(from MAME1.06s)(== pspNEOCDZ045)
 ZV= 05
     ## cz80 [based on CZ80 version 0.91(from pspNEOCDZ045)] (C)2004-2005 Stephane Dallongeville.@Gens

include src/version.txt

#add tools
RANLIB	 = psp-ranlib
MD		 = -mkdir.exe
CP		 = cp

TEMP_FILE= *.tmp
BINARY	 = tmp_out.tmp
BINARY_P = tmp_outp.tmp
OUTPATCH = ./outpatch.exe
ELF2PBP  = ./elf2pbp.exe
ICON0	 = ICONm$(MV).PNG

CC		 = psp-gcc-4.0.2
LD		 = psp-gcc-4.0.2
STRIP	 = psp-strip

## O1は少しサイズが小さいがとろい。（半分？くらい）
## O3==O4みたい。残念。(GCC4.0.0)

AS_OPT	 = -mgp32 -mlong32 -msingle-float -mabi=eabi
CC_OPT	 = -Werror -Wall -fomit-frame-pointer -fno-exceptions -mgp32 -mlong32 -msingle-float -mabi=eabi -c
LINK_OPT = -nostartfiles -Wl,-Ttext=0x08900000
DEFINES	 = 



DEFINES += -D_mpu$(MV) -D_z$(ZV)

TITLE	 = $(VERSION)"m"$(MV)"z"$(ZV)

OBJ		 = obj
OBJDIRS	 = \
$(OBJ) \
$(OBJ)/mpu$(MV) \
$(OBJ)/z$(ZV) \
$(OBJ)/sound \
$(OBJ)/UnzipLib

ZOBJ = $(OBJ)/UnzipLib

## UNZIPLIB OBJS
ZOBJS = \
$(OBJ)/UnzipLib/adler32.o \
$(OBJ)/UnzipLib/crc32.o \
$(OBJ)/UnzipLib/infback.o \
$(OBJ)/UnzipLib/inffast.o \
$(OBJ)/UnzipLib/inflate.o \
$(OBJ)/UnzipLib/inftrees.o \
$(OBJ)/UnzipLib/uncompr.o \
$(OBJ)/UnzipLib/zutil.o \
$(OBJ)/UnzipLib/lib_main.o 

## (sega) picodrive OBJS
SOBJS = \
$(OBJ)/startup.o \
$(OBJ)/mpu$(MV)/m68kmpu.o \
$(OBJ)/z$(ZV)/z80.o \
$(OBJ)/sound/md_sound.o \
$(OBJ)/sound/sound.o \
$(OBJ)/genesis.o \
$(OBJ)/main_sound.o \
$(OBJ)/psp_main.o \
$(OBJ)/font3333.o 

OBJS  = $(SOBJS) $(ZOBJS)

#$(OBJ)/main_text.o 
#$(OBJ)/loadrom.o 

#$(OBJ)/menu.o 
#$(OBJ)/system.o 
#$(OBJ)/vdp.o 
#$(OBJ)/io.o 
#$(OBJ)/error.o 
#$(OBJ)/main_sub.o 

#$(OBJ)/mpu$(MV)/m68kops.o 
#$(OBJ)/mpu$(MV)/m68kopac.o 
#$(OBJ)/mpu$(MV)/m68kopdm.o 
#$(OBJ)/mpu$(MV)/m68kopnz.o 



#$(OBJ)/sound/sn76496.o 

#$(OBJ)/load/loadrom.o 

#$(OBJ)/_clib.o #near loadrom.c
#$(OBJ)/pg.o 	#near main.o

#$(OBJ)/render.o 
#$(OBJ)/memvdp.o 
#$(OBJ)/membnk.o 
#$(OBJ)/memz80.o 
#$(OBJ)/mem68k.o 


#LIBS = 

LIBS	 = unziplib.a

# UnzipLib 関係は－Ｏ１にしてみた。

$(OBJ)/UnzipLib/%.o : src/UnzipLib/%.c
	$(CC) -O1 $(CC_OPT) $(DEFINES) $< -o $@

$(OBJ)/%.o : src/%.c
	$(CC) -O3 $(CC_OPT) $(DEFINES) $< -o $@

$(OBJ)/%.o : src/%.S
	$(CC) $(DEFINES) $(AS_OPT) -c $< -o $@

#	$(CC) -mgp32 -c -xassembler -O -o $@ $<

all: $(BINARY)

#344版unziplib.a組み込み
#	$(AR)		rcs unziplib.a $(ZOBJS)

$(BINARY): $(OBJS) unziplib.a
	$(LD)		$(SOBJS) $(LIBS) $(LINK_OPT) -o $@
	$(STRIP)	$(BINARY)
	$(OUTPATCH) $(BINARY) $(BINARY_P) "USERPROG"
	$(ELF2PBP)	$(BINARY_P) $(TITLE) $(ICON0)
	$(CP)		EBOOT.PBP m$(MV)z$(ZV)EBOOT.PBP
	$(RM)		$(TEMP_FILE)

#	psp-ld -O0  $(OBJS) $(LIBS) -M -Ttext 8900000 -q -o $@ > main.map

unziplib.a: $(ZOBJS)
	$(AR)		rcs unziplib.a $(ZOBJS)

$(sort $(OBJDIRS)):
	$(MD) $@

tree: $(sort $(OBJDIRS))

cputree:
	$(MD) $(OBJ)/mpu22
	$(MD) $(OBJ)/mpu33
	$(MD) $(OBJ)/mpu44
	$(MD) $(OBJ)/z01
	$(MD) $(OBJ)/z02
	$(MD) $(OBJ)/z03
	$(MD) $(OBJ)/z04
	$(MD) $(OBJ)/z05

clean:
	$(RM) $(BINARY)
	$(RM) $(BINARY_P)
	$(RM) $(OBJ)/*.o
	$(RM) $(OBJ)/mpu22/*.o
	$(RM) $(OBJ)/mpu33/*.o
	$(RM) $(OBJ)/mpu44/*.o
	$(RM) $(OBJ)/z01/*.o
	$(RM) $(OBJ)/z02/*.o
	$(RM) $(OBJ)/z03/*.o
	$(RM) $(OBJ)/z04/*.o
	$(RM) $(OBJ)/z05/*.o
	$(RM) $(OBJ)/UnzipLib/*.o
	$(RM) $(OBJ)/sound/*.o
	$(RM) *.map

# clean unzip-lib.
uz:
	$(RM) $(ZOBJ)/*.o
	$(RM) *.a

# remake core ,need to safety the changed z80.
core:
	$(RM) $(OBJ)/genesis.o

# remake sound.
sound:
	$(RM) $(OBJ)/main_sound.o
	$(RM) $(OBJ)/sound/*.o

# clean mpu.
mpu:
	$(RM) $(OBJ)/mpu$(MV)/*.o

# remaked z80.
z80:
	$(RM) $(OBJ)/z$(ZV)/*.o

# remake version.
ver:
	$(RM) $(OBJ)/psp_main.o
	$(RM) *.map
	$(RM) $(TEMP_FILE)

# released.
rr:
	$(RM) EBOOT.PBP
	$(RM) unziplib.a
