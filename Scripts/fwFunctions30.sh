#!/bin/bash

##########################################################################################################
#
# MFlib Shell scripts functions for new RGISarchive layout.
#
##########################################################################################################

if [ "${GHAASDIR}" == "" ]; then GHAASDIR="/usr/local/share/ghaas"; fi
source "${GHAASDIR}/Scripts/RGISfunctions.sh"

if (( "${GHAASprocessorNum}" <= 1))
then
    _fwMAXPROC=1
    export GHAASprocessorNum=${_fwMAXPROC}
elif (( "${GHAASprocessorNum}" <= 16 ))
then
     _fwMAXPROC="${GHAASprocessorNum}"
else
    _fwMAXPROC=16
    export GHAASprocessorNum=${_fwMAXPROC}
fi

_fwPASSNUM=5
_fwRESTART=""

function _fwDataSource () {
	for (( fwI = 0; fwI < ${#_fwDSourceARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwDSourceARRAY[${fwI}]})
		[ "${fwTEMP[0]}${fwTEMP[1]}" == "${1}${2}" ] && { echo "${_fwDSourceARRAY[${fwI}]}"; return 0; }
	done
	echo ""
	return 1
}

function _fwVariable () {
	local fwRETURN=""
	for (( fwI = 0; fwI < ${#_fwVariableARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwVariableARRAY[${fwI}]})
		[ "${fwTEMP[0]}" == "${1}" ] && fwRETURN=${fwTEMP[1]}
	done
	echo "${fwRETURN}"
}

function _fwOutput () {
	local fwRETURN=""
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwOutputARRAY[${fwI}]})
		[ "${fwTEMP}" == "${1}" ] && fwRETURN=${fwTEMP}
	done
	echo "${fwRETURN}"
}

function _fwState () {
	local fwRETURN=""
	for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
	do
		local fwTEMP=(${_fwStateARRAY[${fwI}]})
		[ "${fwTEMP[0]}" == "${1}" ] && fwRETURN=${fwTEMP[0]}
	done
	echo "${fwRETURN}"
}

function FwArguments () {
	          _fwSPINUP="on"
	        _fwFINALRUN="on"
	_fwLENGTHCORRECTION=""
	         FwWARNINGS="on"
	        _fwTESTONLY="off"
	    _fwOPTIONSPRINT="off"
		   _fwOUTFORMAT="gdbc"
	     _fwDAILYOUTPUT="off"
	          FwVERBOSE="off"
	    _fwOPTIONSPIPED="off"

	while [ "${1}" != "" ]
	do
		case ${1} in
			(-r|--restart)
				shift
				 _fwRESTART="${1}"
			;;
			(-s|--spinup)
				shift
				case ${1} in
					(on|off)
						_fwSPINUP="${1}"
					;;
					(*)
						echo "Invalid --spinup argument [${1}]"
					;;
				esac
			;;
			(-f|--finalrun)
				shift
				case ${1} in
					(on|off)
						_fwFINALRUN="${1}"
					;;
					(*)
						echo "Invalid --finalrun argument [${1}]"
					;;
				esac
			;;
			(-l|--lengthcorrection)
				shift
				_fwLENGTHCORRECTION="-l ${1}"
			;;
			(-n|--passnum)
				shift
				if (( "${1}" < 1 || "${1}" > 20 ))
				then
					echo "Invalid --preprocess argument [${1}]"
				else
					_fwPASSNUM="${1}"
				fi
			;;
			(-m|--outputformat)
				shift
				case ${1} in
					(rgis)
						_fwOUTFORMAT="gdbc"
					;;
					(netcdf)
						_fwOUTFORMAT="nc"
					;;
					(*)
						echo "Invalid --preprocess argument [${1}]"
					;;
				esac
				;;
			(-W|--warnings)
				shift
				case ${1} in
					(on|off)
						FwWARNINGS="${1}"
					;;
					(*)
						echo "Invalid --warnings argument [${1}]"
					;;
				esac
			;;
			(-p|--piped)
				shift
				case ${1} in
					(on|off)
					    _fwOPTIONSPIPED="${1}"
					;;
					(*)
						echo "Invalid --piped argument [${1}]"
					;;
				esac
			;;
			(-O|--optionsprint)
				_fwOPTIONSPRINT="on"
			;;
			(-P|--processors)
				shift
				if   (( "${1}"  <  0 ))
				then
					echo "Invalid --process number [${1}]"
					_fwMAXPROC=1
					export GHAASprocessorNum=${_fwMAXPROC}
				elif (( "${1}" <= 16 ))
				then
					_fwMAXPROC="${1}"
					export GHAASprocessorNum=${_fwMAXPROC}
				else
					_fwMAXPROC=16
					export GHAASprocessorNum=${_fwMAXPROC}
				fi
			;;
			(-T|--testonly)
				_fwTESTONLY="on"
			;;
			(-D|--dailyoutput)
				shift
				case ${1} in
					(on|off)
						_fwDAILYOUTPUT="${1}"
					;;
					(*)
						echo "Invalid --dailyoutput argument [${1}]"
					;;
				esac
			;;
			(-V|--verbose)
				FwVERBOSE="on"
			;;
			(-h|--help)
				_fwPROGNAME="${0##*/}" # I don't know how this one works.
				echo "${_fwPROGNAME} [-s on|off] [-f on|off] [-p on|off] -W on|off -T -V"
				echo "           -r, --restart      <year>"
				echo "           -s, --spinup       on|off"
				echo "           -f, --finalrun     on|off"
				echo "           -l, --lengthcorrection [value]"
				echo "           -n, --passnum      [num]"
				echo "           -m, --outputformat [rgis|netcdf]"
				echo "           -p, --piped        on|off"
				echo "           -P, --processors   [# of processors]"
				echo "           -W, --warnings     on|off"
				echo "           -T, --testonly"
				echo "           -O, --optionsprint"
				echo "           -D, --dailyoutput  on|off"
				echo "           -V, --verbose"
                echo "           -g, --gdb"
				return 1
		esac
		shift
	done
	if [ "${_fwRESTART}" != "" ]
	then
		  _fwSPINUP="off"
	    _fwFINALRUN="on"
	fi
	return 0
}

function FwInit () {
	     _fwModelBIN="${1}"; shift
       _fwDomainNAME="${1}"; shift
   _fwRGISDomainFILE="${1}"; shift
       _fwGDSWorkDIR="${1}"; shift
   _fwRGISResultsDIR="${1}"; shift

	    _fwDomainTYPE="${_fwRGISDomainFILE%.gz}"
	    _fwDomainTYPE="${_fwDomainTYPE##*.}"
			FwDomainRES="${_fwRGISDomainFILE%.gz}"
         FwDomainRES="${FwDomainRES%.*}"
         FwDomainRES="${FwDomainRES%_*}"
         FwDomainRES="${FwDomainRES##*_}"

 	[ "${_fwRGISDomainFILE}" == "${FwDomainRES}" ] && FwDomainRES="unset"
	case ${_fwDomainTYPE} in
		(gdbn|gbdn.gz)
			_fwDomainTYPE="Network"
		;;
		(gdbc|gdbc.gz|gdbd|gdbd.gz)
			_fwDomainTYPE="Grid"
		;;
		(*)
			  FwDomainRES=""
			_fwDomainTYPE="Point"
		;;
	esac
	 _fwGDSDomainDIR="${_fwGDSWorkDIR}/${_fwDomainNAME}/${_fwDomainTYPE}_${FwDomainRES}"
	_fwGDSDomainFILE="${_fwGDSDomainDIR}/${_fwDomainNAME}${_fwDomainTYPE}_${FwDomainRES}.ds"
}

function FwDataSrc () {
	unset _fwDSourceARRAY
	(( fwI = 0 ))
	while [ "${1}"	!= "" ]
	do
		local fwDSOURCE=(${1})
		_fwDSourceARRAY[${fwI}]="${fwDSOURCE[0]} ${fwDSOURCE[1]} ${fwDSOURCE[2]} ${fwDSOURCE[3]} ${fwDSOURCE[4]}"
		(( ++fwI ))
		shift
	done
}

function _fwOptionList () {
	for (( fwI = 0; fwI < ${#_fwOptionARRAY[@]} ; ++fwI ))
	do
		echo "-p ${_fwOptionARRAY[${fwI}]}"
	done
}

function FwOptions () {
	(( fwI = 0 ))
	(( fwVARnum   = 0 ))
	(( fwINPUTnum = 0 ))
	(( fwSTATEnum = 0 ))
	unset _fwVariableARRAY
	unset _fwInputARRAY
	unset _fwStateARRAY
	unset _fwOptionARRAY
	while [ "${1}" != "" ]
	do
		local fwOPTION=(${1})
		_fwOptionARRAY[${fwI}]="${fwOPTION[0]}=${fwOPTION[1]}"
		(( ++fwI ))
		shift
	done
	local fwMessageOPTIONS="-m sys_error=off -m app_error=off -m usr_error=off -m debug=off -m warning=off -m info=on"
	if [ "${_fwOPTIONSPRINT}" == "on" ]
	then
		echo "-m sys_error=on"
		echo "-m app_error=on"
		echo "-m usr_error=on"
		echo "-m debug=on"
		echo "-m warning=on"
		echo "-T"
		echo "$(_fwOptionList)"
		return 1
	fi
	local fwLINES=($(${_fwModelBIN} ${_fwGDSDomainFILE} $(_fwOptionList) -T ${fwMessageOPTIONS} | grep "\[.*\]"  | sed "1d" | cut -c15-45,58-64,77-80,81-85,86-94))
	for (( fwVARnum = 0; fwVARnum < ${#fwLINES[@]} / 5 ; ++fwVARnum ))
	do
		_fwVariableITEM="${fwLINES[(( fwVARnum * 5    ))]}"
		local fwVARTYPE="${fwLINES[(( fwVARnum * 5 + 1))]}"
		local     fwSET="${fwLINES[(( fwVARnum * 5 + 2))]}"
		local    fwFLUX="${fwLINES[(( fwVARnum * 5 + 3))]}"
		local fwINITIAL="${fwLINES[(( fwVARnum * 5 + 4))]}"

		if [ "${fwFLUX}"    == "no"  ]
		then
			_fwVariableARRAY[${fwVARnum}]="${_fwVariableITEM} avg"
		else
			_fwVariableARRAY[${fwVARnum}]="${_fwVariableITEM} sum"
		fi
		if [ "${fwSET}"     == "no"  ]
		then
			_fwInputARRAY[${fwINPUTnum}]="${_fwVariableITEM}"
			(( ++fwINPUTnum ))
		fi
		if [ "${fwINITIAL}" == "yes" ]
		then
			_fwStateARRAY[${fwSTATEnum}]="${_fwVariableITEM}"
			(( ++fwSTATEnum ))
		fi
	done
	return 0
}

function FwOutputs () {
	unset _fwOutputARRAY
	local fwI
	local fwJ
	(( fwI = 0 ))
	while [ "${1}"	!= "" ]
	do
		if [ "$(_fwVariable ${1})" == "" ]
		then
			[ "${FwWARNINGS}" == "on" ] && echo "Skipping output variable: ${1}."
		else
			_fwOutputARRAY[${fwI}]="${1}"
			(( ++fwI ))
		fi
		shift
	done
}

function _fwPrintTest () {
	echo "Variables"
	for (( fwI = 0; fwI < ${#_fwVariableARRAY[@]} ; ++fwI ))
	do
		echo "  ${_fwVariableARRAY[${fwI}]}"
	done
	echo "Inputs"
	for (( fwI = 0; fwI < ${#_fwInputARRAY[@]} ; ++fwI ))
	do
		local fwInputITEM=(${_fwInputARRAY[${fwI}]})
		local   fwSOURCE=($(_fwDataSource "${fwInputITEM[0]}" "static"))
		if  [[ "${fwSOURCE[@]}" == ""      ]]
		then
			echo "  ${fwInputITEM[0]} is missing from data sources!"
		elif [ "${fwSOURCE[3]}" == "const" ]
		then
			echo "  ${fwInputITEM[0]} Constant [${fwSOURCE[4]}] input"
		elif [ "${fwSOURCE[3]}" == "pipe"  ]
		then
			echo "  ${fwInputITEM[0]} Piped input"
		elif [ "${fwSOURCE[3]}" == "file"  ]
		then
			if [ -e "${fwSOURCE[4]}" ]
			then
				echo "  ${fwSOURCE[0]} ${fwSOURCE[1]} ${fwSOURCE[2]} ${fwSOURCE[3]} ${fwSOURCE[4]}"
			else
				echo "  ${fwInputITEM[0]} datafile [${fwSOURCE[4]}] is missing!"
			fi
		fi
	done
	echo "States"
	for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
	do
		echo "  ${_fwStateARRAY[${fwI}]}"
	done
	echo "Outputs"
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		echo "  ${_fwOutputARRAY[${fwI}]}"
	done
}

function FwGDSFilename () {
	local fwVARIABLE="${1}"; shift
	local     fwMODE="${1}"; shift
	local  fwVERSION="${1}"; shift
	local     fwYEAR="${1}"; shift
	local     fwSTEP="${1}"; shift

	if [ "${fwYEAR}" == "" ]
	then
		local fwFILENAME="${_fwGDSDomainDIR}/${fwVERSION}/${_fwDomainNAME}_${fwMODE}_${fwVARIABLE}_${fwVERSION}_${FwDomainRES}_${fwSTEP}LT.gds"
	else
		local fwFILENAME="${_fwGDSDomainDIR}/${fwVERSION}/${_fwDomainNAME}_${fwMODE}_${fwVARIABLE}_${fwVERSION}_${FwDomainRES}_${fwSTEP}TS${fwYEAR}.gds"
	fi
	echo ${fwFILENAME}
}

function FwRGISFilename () {
	local fwVARIABLE="${1}"; shift
	local  fwVERSION="${1}"; shift
	local     fwSTEP="${1}"; shift
	local     fwYEAR="${1}"; shift

	case "${fwSTEP}" in
		(a)
			local tStep="annual"
			;;
		(m)
			local tStep="monthly"
			;;
		(d)
			local tStep="daily"
			;;
	esac

	if [ "${fwYEAR}" == "" ]
	then
		local fwFILENAME=$(RGISfilePath "${_fwRGISResultsDIR}" "${_fwDomainNAME}" "${fwVARIABLE}" "${fwVERSION}" "${FwDomainRES}" "LT" "${tStep}")
	else
		local fwFILENAME=$(RGISfilePath "${_fwRGISResultsDIR}" "${_fwDomainNAME}" "${fwVARIABLE}" "${fwVERSION}" "${FwDomainRES}" "TS" "${tStep}"  "${fwYEAR}")
	fi
	if [ "${_fwOUTFORMAT}" == "nc" ]
	then
		fwFILENAME="${fwFILENAME%.*}.nc"
	fi
	echo "${fwFILENAME}"
}

function _fwPreprocess () {
	local    fwYEAR="${1}"; shift

	if [ "${fwYEAR}" == "" ]
	then
		local fwPiped="off"
	else
		local fwPiped="${_fwOPTIONSPIPED}"
	fi
	[ -e "${_fwRGISDomainFILE}" ] || ( echo "Missing domain file: ${_fwRGISDomainFILE}"; return 1; )

	[ "${FwVERBOSE}" == "on" ] && echo "      Preprocessing ${fwYEAR} started:  $(date '+%Y-%m-%d %H:%M:%S')"
	[ -e "${_fwGDSDomainDIR}"  ] || mkdir -p "${_fwGDSDomainDIR}"
	[ -e "${_fwGDSDomainFILE}" ] || rgis2domain ${_fwLENGTHCORRECTION} "${_fwRGISDomainFILE}" "${_fwGDSDomainFILE}"

	for (( fwI = 0; fwI < ${#_fwInputARRAY[@]} ; ++fwI ))
	do
		local fwInputITEM=${_fwInputARRAY[${fwI}]}
		if [ "${fwYEAR}" == "" ]
		then
			local    fwSOURCE=($(_fwDataSource "${fwInputITEM}" "static"))
		else
			local    fwSOURCE=($(_fwDataSource "${fwInputITEM}" "dynamic" | sed "s:xxxx:${fwYEAR}:"))
			if [ "${fwSOURCE[0]}" == "" ]
			then
				local fwSOURCE=($(_fwDataSource "${fwInputITEM}" "static"))
				fwInYEAR=""
			else
				fwInYEAR=${fwYEAR}
			fi
		fi
		[ "${fwSOURCE[0]}" == "" ] && { echo "  ${fwInputITEM} is missing from data sources!";         return 1; }
		[ "${fwSOURCE[1]}" == "" ] && { echo "  ${fwInputITEM} data type is missing!";                 return 1; }
		[ "${fwSOURCE[2]}" == "" ] && { echo "  ${fwInputITEM} version is missing!";                   return 1; }
		[ "${fwSOURCE[3]}" == "" ] && { echo "  ${fwInputITEM} data source type is missing!";          return 1; }
		[ "${fwSOURCE[4]}" == "" ] && { echo "  ${fwInputITEM} data source specification is missing!"; return 1; }

		if [ "${fwSOURCE[3]}" == "const" ]
		then
			[ "${FwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} Constant input"
		elif [ "${fwSOURCE[3]}" == "file" ]
		then
			[ "${FwVERBOSE}" == "on" ] && echo "         ${fwInputITEM} File input"

            		if [ -e "${fwSOURCE[4]}" ]
            		then
				[ -e "${_fwGDSDomainDIR}/${fwSOURCE[2]}" ] || mkdir -p "${_fwGDSDomainDIR}/${fwSOURCE[2]}"
				local fwFILENAME="$(FwGDSFilename "${fwInputITEM}" "Input" "${fwSOURCE[2]}" "${fwInYEAR}" "d")"
				rm -f "${fwFILENAME}"
				[ "${fwPiped}" == "on" ] && mkfifo "${fwFILENAME}"
            			rgis2ds -m "${_fwRGISDomainFILE}" "${fwSOURCE[4]}" "${fwFILENAME}" &
			else
            			echo "  ${fwInputITEM} datafile [${fwSOURCE[4]}] is missing!"
        		fi
        	fi
	done
	if [ "${fwPiped}"   == "off" ]
	then
		wait
		[ "${FwVERBOSE}" == "on"  ] && echo "      Preprocessing ${fwYEAR} finished: $(date '+%Y-%m-%d %H:%M:%S')"
	fi
	return 0
}

function _fwPostprocess () {
    local fwVERSION="${1}"; shift
	local    fwYEAR="${1}"; shift

	if [ "${fwYEAR}" == "" ]; then local fwSUFFIX="LT"; else local fwSUFFIX="TS${fwYEAR}"; fi
	[ "${_fwOPTIONSPIPED}" == "off" ] && [ "${FwVERBOSE}" == "on" ] && { echo "      Postprocessing ${fwYEAR} started:  $(date '+%Y-%m-%d %H:%M:%S')"; }

	local files=""
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		local fwVARIABLE="${_fwOutputARRAY[${fwI}]}"
		local    fwAMODE="$(_fwVariable "${fwVARIABLE}")"
		[ "${fwAMODE}" == "" ] && { echo "Skipping undefinded variable [${fwVARIABLE}]"; continue; }
		local fwGDSFileNAME="$(FwGDSFilename "${fwVARIABLE}" "Output" "${fwVERSION}" "${fwYEAR}" "d")"
		[ -e "${fwGDSFileNAME}" ] || ( echo "Skipping missing variable [${fwVARIABLE}]"; echo ${fwGDSFileNAME}; continue; )

		[ "${_fwOPTIONSPIPED}" == "on" ] && mkfifo "${fwGDSFileNAME}.TMP1" "${fwGDSFileNAME}.TMP2"

		if [ "${_fwDAILYOUTPUT}" == "on" ]:1
		then
			[ "${_fwOPTIONSPIPED}" == "on" ] && mkfifo "${fwGDSFileNAME}.TMP3"
			(cat "${fwGDSFileNAME}"       |\
			 tee "${fwGDSFileNAME}.TMP1" |\
			 tee "${fwGDSFileNAME}.TMP2" > "${fwGDSFileNAME}.TMP3"
			 rm "${fwGDSFileNAME}") &
			(local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwVERSION}" "d" "${fwYEAR}")"
			 [ -e "${fwRGISFileNAME%/*}" ] || mkdir -p "${fwRGISFileNAME%/*}"
			 ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwVERSION} (${FwDomainRES}, Daily${fwSUFFIX})"  \
			                      -m ${_fwRGISDomainFILE} -d "${_fwDomainNAME}" -u "${fwVARIABLE}" -s blue \
			                      "${fwGDSFileNAME}.TMP3" "${fwRGISFileNAME}"
			 rm "${fwGDSFileNAME}.TMP3") &
		else
			(cat "${fwGDSFileNAME}"       |\
			 tee "${fwGDSFileNAME}.TMP1" > "${fwGDSFileNAME}.TMP2"
			rm "${fwGDSFileNAME}") &
		fi
		(local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwVERSION}" "m" "${fwYEAR}")"
		 [ -e "${fwRGISFileNAME%/*}" ] || mkdir -p "${fwRGISFileNAME%/*}"
		 dsAggregate -e month -a ${fwAMODE} "${fwGDSFileNAME}.TMP1" - |\
		 ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwVERSION} (${FwDomainRES}, Monthly${fwSUFFIX})" \
		         -m ${_fwRGISDomainFILE}  -d "${_fwDomainNAME}" -u "${fwVARIABLE}" -s blue - ${fwRGISFileNAME}
		 rm "${fwGDSFileNAME}.TMP1") &
		(local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwVERSION}" "a" "${fwYEAR}")"
		 [ -e "${fwRGISFileNAME%/*}" ] || mkdir -p "${fwRGISFileNAME%/*}"
		 dsAggregate -e year -a ${fwAMODE} "${fwGDSFileNAME}.TMP2" - |\
		 ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwVERSION} (${FwDomainRES}, Yearly${fwSUFFIX})" \
		         -m ${_fwRGISDomainFILE} -d "${_fwDomainNAME}" -u "${fwVARIABLE}"  -s blue - ${fwRGISFileNAME}
		 rm "${fwGDSFileNAME}.TMP2") &
	done
	wait
	[ "${FwVERBOSE}" == "on" ] && { echo "      Postprocessing ${fwYEAR} finished: $(date '+%Y-%m-%d %H:%M:%S')"; }
	return 0
}

function _fwSpinup () {
	local fwVERSION="${1}"; shift
	local    fwYEAR="${1}"; shift

	local fwPASS
	local fwInputITEM
	local fwOutputITEM
	local doState="dostate"

	[ "${FwVERBOSE}" == "on" ] && echo "Initialization started:  $(date '+%Y-%m-%d %H:%M:%S')"
	_fwPreprocess ""
	for ((fwPASS = 1; fwPASS <= _fwPASSNUM; ++fwPASS))
	do
		if   (( fwPASS == 1 ))
		then
			local fwOptionsFILE="${_fwGDSLogDIR}/Spinup0_Options.log"
			local     fwUserLOG="file:${_fwGDSLogDIR}/Spinup0_UserError.log"
			local    fwDebugLOG="file:${_fwGDSLogDIR}/Spinup0_Debug.log"
			local  fwWarningLOG="file:${_fwGDSLogDIR}/Spinup0_Warnings.log"
			local     fwInfoLOG="file:${_fwGDSLogDIR}/Spinup0_Info.log"
		elif (( fwPASS < _fwPASSNUM ))
		then
			local fwOptionsFILE="${_fwGDSLogDIR}/SpinupI_Options.log"
			local     fwUserLOG="file:${_fwGDSLogDIR}/SpinupI_UserError.log"
			local    fwDebugLOG="file:${_fwGDSLogDIR}/SpinupI_Debug.log"
			local  fwWarningLOG="file:${_fwGDSLogDIR}/SpinupI_Warnings.log"
			local     fwInfoLOG="file:${_fwGDSLogDIR}/SpinupI_Info.log"
		else
			local fwOptionsFILE="${_fwGDSLogDIR}/SpinupN_Options.log"
			local     fwUserLOG="file:${_fwGDSLogDIR}/SpinupN_UserError.log"
			local    fwDebugLOG="file:${_fwGDSLogDIR}/SpinupN_Debug.log"
			local  fwWarningLOG="file:${_fwGDSLogDIR}/SpinupN_Warnings.log"
			local     fwInfoLOG="file:${_fwGDSLogDIR}/SpinupN_Info.log"
		fi

		fwOptions=$(echo ${_fwGDSDomainFILE}
		echo "-m sys_error=on"
		echo "-m app_error=on"
		echo "-m usr_error=${fwUserLOG}"
		echo "-m     debug=${fwDebugLOG}"
		echo "-m   warning=${fwWarningLOG}"
		echo "-m      info=${fwInfoLOG}"
		echo "$(_fwOptionList)"
		for (( fwI = 0; fwI < ${#_fwInputARRAY[@]} ; ++fwI ))
		do
			local     fwSOURCE=($(_fwDataSource "${_fwInputARRAY[${fwI}]}" "static"))
			if [[ "${fwSOURCE[@]}" == ""      ]]; then echo "Missing data source" > /dev/stderr; return 1; fi
			if  [ "${fwSOURCE[3]}" == "const" ]
			then
				echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
			else
				echo    "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "" "d")"
			fi
		done
		if (( fwPASS == 1 ))
		then
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
			do
				local     fwSOURCE=($(_fwDataSource "${_fwStateARRAY[${fwI}]}" "static"))
				if [[ "${fwSOURCE[@]}" != "" ]]
				then
					if [[ "${fwSOURCE[3]}" == "const" ]]
					then
						echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
					else
						echo "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "State" "${fwSOURCE[2]}" "" "d")"
					fi
				fi
			done
		else
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
			do
				local fwOutputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwOutputITEM}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "State" "${fwVERSION}" "" "d")"
			done
		fi
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		do
			local fwOutputITEM=(${_fwStateARRAY[${fwI}]})
			echo "-t ${fwOutputITEM[0]}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "State"  "${fwVERSION}" "" "d")"
		done
		if (( fwPASS == _fwPASSNUM ))
		then
			for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
			do
				local fwOutputITEM=(${_fwOutputARRAY[${fwI}]})
				echo "-o ${fwOutputITEM[0]}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "Output" "${fwVERSION}" "" "d")"
				[ "${_fwOPTIONSPIPED}" == "on" ] && mkfifo $(FwGDSFilename "${fwOutputITEM[0]}" "Output" "${fwVERSION}" "" "d")
			done
		fi)

		echo "${fwOptions}" > ${fwOptionsFILE}
		[ "${FwVERBOSE}" == "on" ] && echo "   Passnum [${fwPASS}] started:  $(date '+%Y-%m-%d %H:%M:%S')"

		if [ "${_fwOPTIONSPIPED}" == "on" ]
		then
			if ((fwPASS < _fwPASSNUM))
			then
    			echo ${fwOptions} | xargs ${_fwModelBIN}
    		else
    		  echo ${fwOptions} | xargs ${_fwModelBIN} &
			fi
		else
          echo ${fwOptions} | xargs ${_fwModelBIN}
		fi
	done
	_fwPostprocess "${fwVERSION}" ""
	[ "${FwVERBOSE}"      == "on" ] && echo "Initialization finished: $(date '+%Y-%m-%d %H:%M:%S')"

	local fwInputList=$(echo "${fwOptions}" | grep -e "-i" | grep -e "file:"| grep -e "Input" | sed "s:.*file\:\(.*\):\1:")
	[ "${fwInputList}" == "" ] || rm -f ${fwInputList}
	return 0
}

function _fwRun () {
   local      fwVERSION="${1}"; shift
	local   fwStartYEAR="${1}"; shift
	local     fwEndYEAR="${1}"; shift

	local fwStateList=""
	[ "${FwVERBOSE}" == "on" ] && echo "Model run started:  $(date '+%Y-%m-%d %H:%M:%S')"
    if [ "${_fwRESTART}" == "" ]
    then
        fwFirstYEAR="${fwStartYEAR}"
    else
        fwFirstYEAR="${_fwRESTART}"
    fi
	for (( fwYEAR = fwFirstYEAR; fwYEAR <= fwEndYEAR; ++fwYEAR ))
	do
		local fwOptionsFILE="${_fwGDSLogDIR}/Run${fwYEAR}_Options.log"
		local     fwUserLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_UserError.log"
		local    fwDebugLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Debug.log"
		local  fwWarningLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Warnings.log"
		local     fwInfoLOG="file:${_fwGDSLogDIR}/Run${fwYEAR}_Info.log"

		fwOptions=$(echo ${_fwGDSDomainFILE} -s "${fwYEAR}-01-01" -n "${fwYEAR}-12-31"
		echo "-m sys_error=on"
		echo "-m app_error=on"
		echo "-m usr_error=${fwUserLOG}"
		echo "-m     debug=${fwDebugLOG}"
		echo "-m   warning=${fwWarningLOG}"
		echo "-m      info=${fwInfoLOG}"
		echo "$(_fwOptionList)"

		for (( fwI = 0; fwI < ${#_fwInputARRAY[@]} ; ++fwI ))
		do
			local  fwInputITEM=(${_fwInputARRAY[${fwI}]})
			local     fwSOURCE=($(_fwDataSource "${_fwInputARRAY[${fwI}]}" "dynamic"))
			if [ "${fwSOURCE[0]}" == ""      ]
			then
				local  fwSOURCE=($(_fwDataSource "${_fwInputARRAY[${fwI}]}" "static"))
				if [ "${fwSOURCE[0]}" == ""      ]
				then
					echo "Missing time series input: ${_fwInputARRAY[${fwI}]}" > /dev/stderr
					return 1
				fi
				if [ "${fwSOURCE[3]}" == "const" ]
				then
					echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
				else
					echo "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "" "d")"
				fi
			else
				if [ "${fwSOURCE[3]}" == "const" ]
				then
					echo "-i ${fwSOURCE[0]}=const:${fwSOURCE[4]}"
				else
					echo "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "${fwYEAR}" "d")"
				fi
			fi
		done
		if (( fwYEAR == fwStartYEAR ))
		then
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		 	do
				local fwInputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwInputITEM}=file:$(FwGDSFilename "${fwInputITEM[0]}" "State" "${fwVERSION}" "" "d")"
			done
		else
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		 	do
				local fwInputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwInputITEM}=file:$(FwGDSFilename "${fwInputITEM[0]}" "State" "${fwVERSION}" "$(( fwYEAR - 1 ))" "d")"
			done
		fi

		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
 		do
			local fwStateITEM=(${_fwStateARRAY[${fwI}]})
			echo "-t ${fwStateITEM}=file:$(FwGDSFilename "${fwStateITEM}" "State" "${fwVERSION}" "${fwYEAR}" "d")"
		done
		for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
 		do
			local fwOutputITEM=${_fwOutputARRAY[${fwI}]}
			echo "-o ${fwOutputITEM}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "Output" "${fwVERSION}" "${fwYEAR}" "d")"
			[ "${_fwOPTIONSPIPED}" == "on" ] && mkfifo "$(FwGDSFilename "${fwOutputITEM[0]}" "Output" "${fwVERSION}" "${fwYEAR}" "d")"
 		done)

		echo "${fwOptions}" > ${fwOptionsFILE}

		[ "${FwVERBOSE}" == "on" ] && echo "   Running year [${fwYEAR}] started:  $(date '+%Y-%m-%d %H:%M:%S')"
		_fwPreprocess "${fwYEAR}"
		if [ "${_fwOPTIONSPIPED}" == "on" ]
		then
			echo ${fwOptions} | xargs ${_fwModelBIN} &
		else
            echo ${fwOptions} | xargs ${_fwModelBIN}
		fi
		_fwPostprocess "${fwVERSION}" "${fwYEAR}"
		local fwInputList=$(echo "${fwOptions}" | grep -e "-i" | grep -e "file:"| grep -e "Input" | sed "s:.*file\:\(.*\):\1:")
		[ "${fwInputList}" == "" ] || rm -f ${fwInputList}
	done
	[ "${FwVERBOSE}" == "on" ] && echo "Model run finished: $(date '+%Y-%m-%d %H:%M:%S')"
	return 0
}

function FwRun () {
	local   fwVERSION="${1}"; shift
	local fwStartYEAR="${1}"; shift
	local   fwEndYEAR="${1}"; shift

	     _fwGDSLogDIR="${_fwGDSDomainDIR}/${fwVERSION}"

	[ "${_fwTESTONLY}" == "on" ] && { { _fwPrintTest || return 1; } && return 0; }
	[ -e "${_fwGDSLogDIR}" ] || mkdir -p ${_fwGDSLogDIR}
	[ "${_fwSPINUP}"   == "on" ] && { _fwSpinup "${fwVERSION}"                                 || { echo "Spinup failed";    return 1; } }
	[ "${_fwFINALRUN}" == "on" ] && { _fwRun    "${fwVERSION}" "${fwStartYEAR}" "${fwEndYEAR}" || { echo "Final run failed"; return 1; } }
	return 0
}