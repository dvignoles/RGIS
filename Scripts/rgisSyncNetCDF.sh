#!/bin/bash
if [ "${GHAASDIR}" == "" ]; then GHAASDIR="/usr/local/share/ghaas"; fi
source "${GHAASDIR}/Scripts/RGISfunctions.sh"

USAGE="Usage ${0##*/} <rgis directory> <netcdf directory>"
        RGISDIR="${1}"; shift
          NCDIR="${1}"; shift
if [[ "${NCDIR}" == "" ]]; then echo "${USAGE}"; exit 1; fi

function RGIStoNC () {
	local rgisDir="${1}"; shift;
	local   ncDir="${1}"; shift;

	local procNum=0
	find "${rgisDir}" -name "*.gdbc.gz" |\
	(while read rgisFile
	 do
		ncFile="$(echo "${rgisFile}" | sed "s:${rgisDir}:${ncDir}:" | sed "s:gdbc.gz:nc:")"
		[ -e "${ncFile%/*}" ] || mkdir -p "${ncFile%/*}"
		local rgisDate="$(date "+%Y-%m-%d %H:%M:%S" -r "${rgisFile}")"
		if [[ -e "${ncFile}" ]]
		then
			local ncDate="$(date "+%Y-%m-%d %H:%M:%S" -r   "${ncFile}")"
	  	else
	  		local ncData=""
	  	fi
		if [[ "${rgisDate}" > "${ncDate}" ]]
		then
			rgis2netcdf "${rgisFile}" "${ncFile}" &
			if (( ${procNum} == ${GHAASprocessorNum} ))
			then
  				wait
  				procNum=0
	   		else
		   		procNum=$(expr ${procNum} + 1)
  			fi
		fi
	done)
}

function PurgeNCfiles () {
	local rgisDir="${1}"; shift;
	local   ncDir="${1}"; shift;

	find "${ncDir}" -name "*.nc" |\
	(while read ncFile
	 do
		rgisFile="$(echo "${ncFile}" | sed "s:${ncDir}:${rgisDir}:" | sed "s:nc:gdbc.gz:")"
		[ -e "${rgisFile}" ] || echo "${ncFile}"
	done)
}

function PurgeNCdirs () {
	local rgisDir="${1}"; shift;
	local   ncDir="${1}"; shift;

	find "${ncDir}" -type "d" |\
	(while read ncDir
	 do
		rgisDir="$(echo "${ncDir}" | sed "s:${ncDir}:${rgisDir}:")"
		[ -e "${rgisDir}" ] || echo "${ncDir}"
	done)
}

RGIStoNC     "${RGISDIR}" "${NCDIR}"
PurgeNCfiles "${RGISDIR}" "${NCDIR}"
PurgeNCdirs  "${RGISDIR}" "${NCDIR}"