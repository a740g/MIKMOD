# Use the C or C++ compiler
Compiler                = wcc386
#Compiler                = wpp386

# compiler options for code that has to run from an interrupt:
Compiler_Options1       = /oxl+ /j /s /zu /Iinclude

# options for other code:
Compiler_Options2       = /oxl+ /j /s /Iinclude

Linker_options          = option stack=60000

# Compile using dos4g or pmodew
#System                  = dos4g
System                 = pmodew

Exe_file                = mikmod.exe

LoaderObjs              = mloader.obj &
			  load_mod.obj &
			  load_m15.obj &
			  load_mtm.obj &
			  load_s3m.obj &
			  load_stm.obj &
			  load_xm.obj &
			  load_uni.obj &
                          load_ult.obj 

DriverObjs              = mdriver.obj &
			  drv_nos.obj &
			  drv_gus.obj &
                          drv_sb.obj &
                          drv_ss.obj

####################
## Makefile rules ##
####################

all        : mikmod.exe mikwav.exe mikcvt.exe

mikmod.exe : mikmod.obj mmio.obj mirq.obj mdma.obj virtch.obj resample.obj &
             munitrk.obj mplayer.obj wildfile.obj getopt.obj &
             $(LoaderObjs) $(DriverObjs)

             *wlink system $(System) $(Linker_Options) name mikmod.exe &
             file {$<} 

mikwav.exe : mikwav.obj mmio.obj mirq.obj mdma.obj virtch.obj resample.obj &
             mwav.obj $(DriverObjs)

             *wlink system $(System) $(Linker_Options) name mikwav.exe &
             file {$<} 


mikcvt.exe : mikcvt.obj mmio.obj munitrk.obj wildfile.obj &
             $(LoaderObjs)

             *wlink system $(System) $(Linker_Options) name mikcvt.exe &
             file {$<}

clean
        del *.obj
        del *.err
        del *.bak

# next are the exceptions that don't have to be compiled with the /zu option
# since they will never be called from a interrupt. hey.. I _tried_ to find
# a clean solution..

wildfile.obj :  wildfile.c
                $(Compiler) $(Compiler_Options2) $<

getopt.obj :    getopt.c
                $(Compiler) $(Compiler_Options2) $<

mmio.obj :      mmio.c
                $(Compiler) $(Compiler_Options2) $<

mloader.obj :   mloader.c
                $(Compiler) $(Compiler_Options2) $<

mikcvt.obj :    mikcvt.c
                $(Compiler) $(Compiler_Options2) $<

load_m15.obj :  load_m15.c
                $(Compiler) $(Compiler_Options2) $<

load_mod.obj :  load_mod.c
                $(Compiler) $(Compiler_Options2) $<

load_mtm.obj :  load_mtm.c
                $(Compiler) $(Compiler_Options2) $<

load_ult.obj :  load_ult.c
                $(Compiler) $(Compiler_Options2) $<

load_stm.obj :  load_stm.c
                $(Compiler) $(Compiler_Options2) $<

load_s3m.obj :  load_s3m.c
                $(Compiler) $(Compiler_Options2) $<

load_xm.obj :   load_xm.c
                $(Compiler) $(Compiler_Options2) $<

load_uni.obj :  load_uni.c
                $(Compiler) $(Compiler_Options2) $<

.c.obj:
        $(Compiler) $(Compiler_Options1) $<

.asm.obj:
        wasm -3pr $<
