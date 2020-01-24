#!/bin/bash
if [ "${GHAASDIR}" == "" ]; then GHAASDIR="/usr/local/share/ghaas"; fi
source "${GHAASDIR}/Scripts/RGISfunctions.sh"

USAGE="Usage ${0##*/} <rgis directory> <netcdf directory>"
        RGISDIR="${1}"; shift
          NCDIR="${1}"; shift
if [[ "${NCDIR}" == "" ]]; then echo "${USAGE}"; exit 1; fi


procNum=0
find "${RGISDIR}" -name "*.gdbc.gz" | sort |\
(while read RGISFILE
 do
	NCFILE="$(echo "${RGISFILE}" | sed "s:${RGISDIR}:${NCDIR}:" | sed "s:gdbc.gz:nc:")"
	[ -e "${NCFILE%/*}" ] || mkdir -p "${NCFILE%/*}"
	rgis2netcdf "${RGISFILE}" "${NCFILE}" &
	if (( ${procNum} == ${GHAASprocessorNum} ))
	then
   		wait
  		procNum=0
   	else
	   	procNum=$(echo "${procNum} + 1" | bc -l)

   	fi
 done)