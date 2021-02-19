ECHO OFF
CLS
ECHO ***************************************************************************
ECHO *                       Embedded Systems Building Blocks
ECHO *                    Complete and Ready-to-Use Modules in C
ECHO *                               Second Edition
ECHO *
ECHO *                              Jean J. Labrosse
ECHO *
ECHO *
ECHO *                                INSTALLATION
ECHO *
ECHO *
ECHO * Usage  :  <source>:INSTALL [destination]
ECHO *
ECHO *           where: 'source'      is the drive letter of you CD-ROM drive
ECHO *                  'destination' is an optional destination drive letter
ECHO *
ECHO * Example:  E:INSTALL D
ECHO *           would install Embedded Systems Building Blocks on drive D:
ECHO *           from your CD-ROM drive which is drive E:
ECHO ***************************************************************************
ECHO *
IF "%1" == "" GOTO CURRENT
%2:
:CURRENT

CD   \
MD   \SOFTWARE
CD   \SOFTWARE
COPY %1:ESBB.EXE
ESBB -d -n \SOFTWARE
DEL  ESBB.EXE
CD   \SOFTWARE\BLOCKS\SAMPLE\TEST

CLS
ECHO ***************************************************************************
ECHO *                        Embedded Systems Building Blocks
ECHO *                     Complete and Ready-to-Use Modules in C
ECHO *                               Second Edition
ECHO *
ECHO *                              Jean J. Labrosse
ECHO *
ECHO *                           INSTALLATION  COMPLETE
ECHO ***************************************************************************
