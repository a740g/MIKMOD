# MIKMOD
MikMod 2.10 with AWE32 driver

This is only here for legacy and historical reasons. MikMod 2.10 source is just include here for this repository to be self-contained.

If you want to use and compile it, use DJGPP in DOSBox. You can do with this whatever you please. As usual, I do not accept responsibility for any effects, adverse or otherwise, that this code may have on you, your computer, your sanity, your dog, and anything else that you can think of. Use it at your own risk. Have fun.

/***************************************************************************\
*    _____ ____        __          ________                                 *
*   / ____|  _ \      /\ \        / /  ____|                                *
*  | (___ | |_) |    /  \ \  /\  / /| |__                                   *
*   \___ \|  _ <    / /\ \ \/  \/ / |  __|                                  *
*   ____) | |_) |  / ____ \  /\  /  | |____                                 *
*  |_____/|____/  /_/    \_\/  \/   |______|    Native Mode Driver.         *
*                                                                           *
*                                                                           *
*  Copyright (c) Samuel Gomes, 1998-2002.                                   *
*  All rights reserved.                                                     *
*                                                                           *
*  github.com/a740g                                                         *
*                                                                           *
*  MikMod driver for output on SB AWE and compatibles (native mode).        *
*                                                                           *
\***************************************************************************/

/***************************************************************************\
*                                                                           *
*  Bibliography:                                                            *
*                                                                           *
*  1) "The Un-official Sound Blaster AWE32 Programming Guide" by Vince Vu   *
*       a.k.a. Judge Dredd.                                                 *
*                                                                           *
*  2) "Low-level control of the EMU8000" sources by Lada Kopecky.           *
*                                                                           *
*  3) "AWE32/EMU8000 Programmer's Guide" (AEPG) by Dave Rossum. The AEPG    *
*       is part of the "AWE32 Developer's Information Pack" (ADIP) from     *
*       E-mu/Creative Technology Ltd.                                       *
*                                                                           *
*  4) The "Omega AWE32 Module Player" source by Cygnus X-1.                 *
*                                                                           *
*  5) The "Open Cubic Player" sources by Niklas Beisert/Tammo Hinrichs.     *
*                                                                           *
*  6) "Allegro Game Library" sources by Shawn Hargreaves.                   *
*                                                                           *
*  7) "MikMod 3 AWE32 Driver" sources by Steffen Rusitschka/Jake Stine.     *
*                                                                           *
*  8) "MikMod Sound System" sources by Jean Paul Mikkers.                   *
*                                                                           *
\***************************************************************************/

/***************************************************************************\
*                                                                           *
*  Conditions:                                                              *
*                                                                           *
*  This code frankly speaking is open source/freeware. You can do whatever  *
*  you want with it.                                                        *
*                                                                           *
*  If you use this code in your projects, please try to give me some        *
*  credit, as I have spent long sleepless nights fiddling with my SBAWE64.  *
*                                                                           *
*  I do not accept responsibility for any effects, adverse or otherwise,    *
*  that this code may have on you, your computer, your sanity, your dog,    *
*  and anything else that you can think of. Use it at your own risk.        *
*                                                                           *
\***************************************************************************/

/***************************************************************************\
*                                                                           *
*  Portability:                                                             *
*                                                                           *
*  DOS  :  Borland C(?) Watcom C(y) DJGPP(y)                                *
*  Win32:  n                                                                *
*  OS/2 :  n                                                                *
*  Linux:  n                                                                *
*                                                                           *
*  (y) - yes                                                                *
*  (n) - no (not possible or not useful)                                    *
*  (?) - may be possible, but not tested                                    *
*                                                                           *
\***************************************************************************/

/***************************************************************************\
*                                                                           *
*  Requirements:                                                            *
*                                                                           *
*  1) MikMod 2.10 (full MS-DOS support). Seems like the original MikMod     *
*       team's AWE driver never saw the light of day! ;)                    *
*                                                                           *
*  2) Watcom C 9.1+ (I use 9.1) or DJGPP 2.01+ (I use 2.03). Can't we just  *
*       forget those old half-hearted 16-bit compilers ;)                   *
*                                                                           *
*  3) And ofcourse, a SB32 or SBAWE32 or SBAWE64 or compatible with some    *
*       DRAM (512KB+ to be exact).                                          *
*                                                                           *
\***************************************************************************/

/***************************************************************************\
*                                                                           *
*  Notes:                                                                   *
*                                                                           *
*  1) Don't be fooled by the number in AWE64! Even though it is a better    *
*       and slicker version of the AWE32, it only supports upto 32 voices   *
*       at the same time (like it's AWE32 cousin). Then why the hell        *
*       Creative calls it the AWE64?!!! Just because of the lame reason     *
*       that in Windows they use the WaveSynth/WG Software Wavetable        *
*       Synthesizer along with the AWE64 to provide a total of              *
*       32 + 32 = 64 channels! Isn't that sick? And the speed really sucks  *
*       if ya don't have a fast CPU. On top of that if we use the onboard   *
*       DRAM we gotta sacrifice 2 channels for the DRAM refresh. :( That    *
*       leaves us with 30 channels.                                         *
*                                                                           *
*  2) The detection function only looks at the BLASTER environment          *
*       variable; no hardware scanning is done. So make sure you set that   *
*       environment variable correctly.                                     *
*                                                                           *
*  3) Using the BLASTER environment variable we can detect the majority of  *
*       sound blaster compatible devices:                                   *
*       BLASTER=A220 I5 D1 H5 P330 E620 T6                                  *
*       |       |    |  |  |  |    |    |                                   *
*       |       |    |  |  |  |    |    |_____ Type of Card                 *
*       |       |    |  |  |  |    |__________ AWE32/64 Only Parameter      *
*       |       |    |  |  |  |_______________ MIDI Port                    *
*       |       |    |  |  |__________________ "High" DMA Channel           *
*       |       |    |  |_____________________ DMA Channel                  *
*       |       |    |________________________ Interrupt                    *
*       |       |_____________________________ Port Address                 *
*       |_____________________________________ Environment Variable         *
*                                                                           *
*  4) The AWE uses only 16-bit samples internally; all 8-bit sample data    *
*       are converted to 16-bit by the driver. Also, each sound requires    *
*       few bytes of overhead for internal storage. This is so that we can  *
*       make all uploaded samples anti-click. On top of that if the sound   *
*       has to do a bi-directional loop then this calls for unrolling the   *
*       sound in the AWE DRAM, 'cause the AWE can't do reverse playback     *
*       (properly). As a result this hogs up some extra amount of AWE DRAM  *
*       needed for the sound.                                               *
*                                                                           *
\***************************************************************************/
