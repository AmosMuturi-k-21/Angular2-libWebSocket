#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/5c0/HD44780.o \
	${OBJECTDIR}/_ext/5c0/ad595.o \
	${OBJECTDIR}/_ext/5c0/ad7714.o \
	${OBJECTDIR}/_ext/5c0/addc.o \
	${OBJECTDIR}/_ext/5c0/aw.o \
	${OBJECTDIR}/_ext/5c0/board.o \
	${OBJECTDIR}/_ext/5c0/boarddef.o \
	${OBJECTDIR}/_ext/5c0/boot.o \
	${OBJECTDIR}/_ext/5c0/button.o \
	${OBJECTDIR}/_ext/5c0/buzzer.o \
	${OBJECTDIR}/_ext/5c0/calibration.o \
	${OBJECTDIR}/_ext/5c0/conv_ascii.o \
	${OBJECTDIR}/_ext/5c0/dac8532.o \
	${OBJECTDIR}/_ext/5c0/ext_calib.o \
	${OBJECTDIR}/_ext/5c0/fan.o \
	${OBJECTDIR}/_ext/5c0/helpers.o \
	${OBJECTDIR}/_ext/5c0/i2c.o \
	${OBJECTDIR}/_ext/5c0/lcd.o \
	${OBJECTDIR}/_ext/5c0/main.o \
	${OBJECTDIR}/_ext/5c0/noyau_temps.o \
	${OBJECTDIR}/_ext/5c0/oldmain.o \
	${OBJECTDIR}/_ext/5c0/opt.o \
	${OBJECTDIR}/_ext/5c0/peltier.o \
	${OBJECTDIR}/_ext/5c0/pttemp.o \
	${OBJECTDIR}/_ext/5c0/sensors.o \
	${OBJECTDIR}/_ext/5c0/shared.o \
	${OBJECTDIR}/_ext/5c0/ssj.o \
	${OBJECTDIR}/_ext/5c0/vardef.o \
	${OBJECTDIR}/_ext/5c0/web.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/testVALMIR.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/serverapp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/serverapp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/serverapp ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/5c0/HD44780.o: ../HD44780.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/HD44780.o ../HD44780.c

${OBJECTDIR}/_ext/5c0/ad595.o: ../ad595.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/ad595.o ../ad595.c

${OBJECTDIR}/_ext/5c0/ad7714.o: ../ad7714.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/ad7714.o ../ad7714.c

${OBJECTDIR}/_ext/5c0/addc.o: ../addc.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/addc.o ../addc.c

${OBJECTDIR}/_ext/5c0/aw.o: ../aw.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/aw.o ../aw.c

${OBJECTDIR}/_ext/5c0/board.o: ../board.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/board.o ../board.c

${OBJECTDIR}/_ext/5c0/boarddef.o: ../boarddef.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/boarddef.o ../boarddef.c

${OBJECTDIR}/_ext/5c0/boot.o: ../boot.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/boot.o ../boot.c

${OBJECTDIR}/_ext/5c0/button.o: ../button.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/button.o ../button.c

${OBJECTDIR}/_ext/5c0/buzzer.o: ../buzzer.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/buzzer.o ../buzzer.c

${OBJECTDIR}/_ext/5c0/calibration.o: ../calibration.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/calibration.o ../calibration.c

${OBJECTDIR}/_ext/5c0/conv_ascii.o: ../conv_ascii.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/conv_ascii.o ../conv_ascii.c

${OBJECTDIR}/_ext/5c0/dac8532.o: ../dac8532.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/dac8532.o ../dac8532.c

${OBJECTDIR}/_ext/5c0/ext_calib.o: ../ext_calib.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/ext_calib.o ../ext_calib.c

${OBJECTDIR}/_ext/5c0/fan.o: ../fan.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/fan.o ../fan.c

${OBJECTDIR}/_ext/5c0/helpers.o: ../helpers.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/helpers.o ../helpers.c

${OBJECTDIR}/_ext/5c0/i2c.o: ../i2c.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/i2c.o ../i2c.c

${OBJECTDIR}/_ext/5c0/lcd.o: ../lcd.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/lcd.o ../lcd.c

${OBJECTDIR}/_ext/5c0/main.o: ../main.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/main.o ../main.c

${OBJECTDIR}/_ext/5c0/noyau_temps.o: ../noyau_temps.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/noyau_temps.o ../noyau_temps.c

${OBJECTDIR}/_ext/5c0/oldmain.o: ../oldmain.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/oldmain.o ../oldmain.c

${OBJECTDIR}/_ext/5c0/opt.o: ../opt.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/opt.o ../opt.c

${OBJECTDIR}/_ext/5c0/peltier.o: ../peltier.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/peltier.o ../peltier.c

${OBJECTDIR}/_ext/5c0/pttemp.o: ../pttemp.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/pttemp.o ../pttemp.c

${OBJECTDIR}/_ext/5c0/sensors.o: ../sensors.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/sensors.o ../sensors.c

${OBJECTDIR}/_ext/5c0/shared.o: ../shared.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/shared.o ../shared.c

${OBJECTDIR}/_ext/5c0/ssj.o: ../ssj.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/ssj.o ../ssj.c

${OBJECTDIR}/_ext/5c0/vardef.o: ../vardef.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/vardef.o ../vardef.c

${OBJECTDIR}/_ext/5c0/web.o: ../web.c
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/web.o ../web.c

${OBJECTDIR}/main.o: main.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/testVALMIR.o: testVALMIR.c
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/testVALMIR.o testVALMIR.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
