# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=e00compr - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to e00compr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "e00compr - Win32 Release" && "$(CFG)" !=\
 "e00compr - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "e00compr.mak" CFG="e00compr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "e00compr - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "e00compr - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "e00compr - Win32 Debug"
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "e00compr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\e00conv.exe"

CLEAN : 
	-@erase ".\e00conv.exe"
	-@erase ".\Release\e00read.obj"
	-@erase ".\Release\e00write.obj"
	-@erase ".\Release\e00error.obj"
	-@erase ".\Release\e00conv.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/e00compr.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE RSC /l 0xc0c /d "NDEBUG"
# ADD RSC /l 0xc0c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/e00compr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"e00conv.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/e00conv.pdb" /machine:I386 /out:"e00conv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/e00read.obj" \
	"$(INTDIR)/e00write.obj" \
	"$(INTDIR)/e00error.obj" \
	"$(INTDIR)/e00conv.obj"

"$(OUTDIR)\e00conv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "e00compr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\e00conv.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\e00conv.exe"
	-@erase ".\Debug\e00write.obj"
	-@erase ".\Debug\e00error.obj"
	-@erase ".\Debug\e00read.obj"
	-@erase ".\Debug\e00conv.obj"
	-@erase ".\e00conv.ilk"
	-@erase ".\Debug\e00conv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/e00compr.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE RSC /l 0xc0c /d "_DEBUG"
# ADD RSC /l 0xc0c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/e00compr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"e00conv.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/e00conv.pdb" /debug /machine:I386 /out:"e00conv.exe" 
LINK32_OBJS= \
	"$(INTDIR)/e00write.obj" \
	"$(INTDIR)/e00error.obj" \
	"$(INTDIR)/e00read.obj" \
	"$(INTDIR)/e00conv.obj"

"$(OUTDIR)\e00conv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "e00compr - Win32 Release"
# Name "e00compr - Win32 Debug"

!IF  "$(CFG)" == "e00compr - Win32 Release"

!ELSEIF  "$(CFG)" == "e00compr - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\e00read.c

!IF  "$(CFG)" == "e00compr - Win32 Release"

DEP_CPP_E00RE=\
	".\e00compr.h"\
	
NODEP_CPP_E00RE=\
	".\iDecimalPoint"\
	".\psInfo"\
	

"$(INTDIR)\e00read.obj" : $(SOURCE) $(DEP_CPP_E00RE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "e00compr - Win32 Debug"

DEP_CPP_E00RE=\
	".\e00compr.h"\
	

"$(INTDIR)\e00read.obj" : $(SOURCE) $(DEP_CPP_E00RE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\e00error.c
DEP_CPP_E00ER=\
	".\e00compr.h"\
	

"$(INTDIR)\e00error.obj" : $(SOURCE) $(DEP_CPP_E00ER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\e00write.c

!IF  "$(CFG)" == "e00compr - Win32 Release"

DEP_CPP_E00WR=\
	".\e00compr.h"\
	
NODEP_CPP_E00WR=\
	".\+"\
	

"$(INTDIR)\e00write.obj" : $(SOURCE) $(DEP_CPP_E00WR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "e00compr - Win32 Debug"

DEP_CPP_E00WR=\
	".\e00compr.h"\
	

"$(INTDIR)\e00write.obj" : $(SOURCE) $(DEP_CPP_E00WR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\e00conv.c

!IF  "$(CFG)" == "e00compr - Win32 Release"

DEP_CPP_E00CO=\
	".\e00compr.h"\
	
NODEP_CPP_E00CO=\
	".\of"\
	

"$(INTDIR)\e00conv.obj" : $(SOURCE) $(DEP_CPP_E00CO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "e00compr - Win32 Debug"

DEP_CPP_E00CO=\
	".\e00compr.h"\
	

"$(INTDIR)\e00conv.obj" : $(SOURCE) $(DEP_CPP_E00CO) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
