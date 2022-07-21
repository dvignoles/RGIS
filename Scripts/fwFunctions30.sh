#!/usr/bin/env bash

##########################################################################################################
#
# MFlib Shell scripts functions for new RGISarchive layout.
#
##########################################################################################################

[[ "${GHAASDIR}" == "" ]] && export GHAASDIR="/usr/local/share/ghaas"
source "${GHAASDIR}/Scripts/RGISfunctions.sh"

         _fwCLEANUP="on"
     _fwDAILYOUTPUT="off"
	    _fwFINALRUN="on"
_fwLENGTHCORRECTION=""
	   _fwOUTFORMAT="gdbc"
         _fwPASSNUM="5"
    _fwOPTIONSPRINT="off"
         _fwMAXPROC="${GHAASprocessorNum}"
         _fwRESTART=""
           _fwSTART="TRUE"
	      _fwSPINUP="on"
	    _fwTESTONLY="off"
         _fwVERBOSE="off"
        _fwWARNINGS="on"

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

	while [ "${1}" != "" ]
	do
		case ${1} in
			(-C|--cleanup)
				shift
				_fwCLEANUP=${1}
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
				case ${1} in
					(off)
						_fwLENGTHCORRECTION=""
					;;
					(*)
						_fwLENGTHCORRECTION="${1}"
					;;
				esac
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
			(-n|--passnum)
				shift
				if (( "${1}" < 1 || "${1}" > 20 ))
				then
					echo "Invalid --preprocess argument [${1}]"
				else
					_fwPASSNUM="${1}"
				fi
			;;
			(-O|--optionsprint)
				_fwOPTIONSPRINT="on"
			;;
			(-P|--postprocessors)
				shift
				if   (( ${1} <= 0 ))
				then
					echo "Invalid --process number [${1}]"
					_fwMAXPROC=1
				else
					_fwMAXPROC=${1}
				fi
			;;
			(-r|--restart)
				shift
				 _fwRESTART="${1}"
				 _fwSTART="FALSE"
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
			(-T|--testonly)
				_fwTESTONLY="on"
			;;
			(-V|--verbose)
				_fwVERBOSE="on"
			;;
			(-W|--warnings)
				shift
				case ${1} in
					(on|off)
						_fwWARNINGS="${1}"
					;;
					(*)
						echo "Invalid --warnings argument [${1}]"
					;;
				esac
			;;
			(-h|--help)
				_fwPROGNAME="${0##*/}"
				echo "${_fwPROGNAME} [options]"
				echo"            -C|--cleanup       on|off|<destination directory>"
				echo "           -D, --dailyoutput  on|off"
				echo "           -f, --finalrun     on|off"
				echo "           -l, --lengthcorrection [<value>|auto]"
				echo "           -m, --outputformat [rgis|netcdf]"
				echo "           -n, --passnum      [num]"
				echo "           -O, --optionsprint"
				echo "           -P, --processors   [# of processors]"
				echo "           -r, --restart      <year>"
				echo "           -s, --spinup       on|off"
				echo "           -T, --testonly"
				echo "           -V, --verbose"
				echo "           -W, --warnings     on|off"
				echo "           -h, --help"
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
	if [[ "${_fwDomainTYPE}" == "Network" && "${_fwLENGTHCORRECTION}" != "" ]]
	then
		if (( $(RGISgeoResolutionInSecond "30sec") >= $(RGISgeoResolutionInSecond "${FwDomainRES}") ))
		then
			_fwLENGTHCORRECTION="1.000"
		else
			if [ ${_fwLENGTHCORRECTION} == "auto" ]
			then
				_fwCellSizeRatio=$(echo "$(RGISgeoResolutionInSecond "30sec") / $(RGISgeoResolutionInSecond ${FwDomainRES})" | bc -l)
				# Modified from Fekete et al. 2001 to go through 1.0 when the cell ratio is 1.0
				_fwLENGTHCORRECTION="$(echo "1.0 - 0.084 * l("${_fwCellSizeRatio}")" | bc -l)"
			fi
			_fwLENGTHCORRECTION="$(printf "%.3f" ${_fwLENGTHCORRECTION})"
		fi
		_fwGDSDomainFILE="${_fwGDSDomainDIR}/${_fwDomainNAME}${_fwDomainTYPE}$(echo "${_fwLENGTHCORRECTION}" | sed "s:\.:p:")_${FwDomainRES}.ds"
		export _fwLENGTHCORRECTION="-l ${_fwLENGTHCORRECTION}"
	else
		_fwGDSDomainFILE="${_fwGDSDomainDIR}/${_fwDomainNAME}${_fwDomainTYPE}_${FwDomainRES}.ds"
	fi
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
			[ "${_fwWARNINGS}" == "on" ] && echo "Skipping output variable: ${1}."
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
	local    fwVARIABLE="${1}"; shift
	local        fwMODE="${1}"; shift
	local  fwExperiment="${1}"; shift
	local        fwYEAR="${1}"; shift
	local        fwSTEP="${1}"; shift

	if [ "${fwYEAR}" == "" ]
	then
		local fwFILENAME="${_fwGDSDomainDIR}/${fwExperiment}/${_fwDomainNAME}_${fwMODE}_${fwVARIABLE}_${fwExperiment}_${FwDomainRES}_${fwSTEP}LT.gds"
	else
		local fwFILENAME="${_fwGDSDomainDIR}/${fwExperiment}/${_fwDomainNAME}_${fwMODE}_${fwVARIABLE}_${fwExperiment}_${FwDomainRES}_${fwSTEP}TS${fwYEAR}.gds"
	fi
	echo ${fwFILENAME}
}

function FwRGISFilename () {
	local   fwVARIABLE="${1}"; shift
	local fwExperiment="${1}"; shift
	local       fwSTEP="${1}"; shift
	local       fwYEAR="${1}"; shift

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
		local fwFILENAME=$(RGISfilePath "${_fwRGISResultsDIR}" "${_fwDomainNAME}" "${fwVARIABLE}" "${fwExperiment}" "${FwDomainRES}" "LT" "${tStep}")
	else
		local fwFILENAME=$(RGISfilePath "${_fwRGISResultsDIR}" "${_fwDomainNAME}" "${fwVARIABLE}" "${fwExperiment}" "${FwDomainRES}" "TS" "${tStep}"  "${fwYEAR}")
	fi
	if [ "${_fwOUTFORMAT}" == "nc" ]
	then
		fwFILENAME="${fwFILENAME%.*}.nc"
	fi
	echo "${fwFILENAME}"
}

function _fwPreprocess () {
	local    fwYEAR="${1}"; shift

	[ -e "${_fwRGISDomainFILE}" ] || ( echo "Missing domain file: ${_fwRGISDomainFILE}"; return 1; )

	[ "${_fwVERBOSE}" == "on" ] && echo "      Preprocessing ${fwYEAR} started:  $(date '+%Y-%m-%d %H:%M:%S')"
	[ "${_fwVERBOSE}" == "on" ] && echo "         Boundary Variables:"
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
			[ "${_fwVERBOSE}" == "on" ] && echo "            ${fwInputITEM} Constant input"
		elif [ "${fwSOURCE[3]}" == "file" ]
		then
			[ "${_fwVERBOSE}" == "on" ] && echo "            ${fwInputITEM} File input"
            if [ -e "${fwSOURCE[4]}" ]
            then
				[ -e "${_fwGDSDomainDIR}/${fwSOURCE[2]}" ] || mkdir -p "${_fwGDSDomainDIR}/${fwSOURCE[2]}"
				local fwFILENAME="$(FwGDSFilename "${fwInputITEM}" "Input" "${fwSOURCE[2]}" "${fwInYEAR}" "d")"
            	rgis2ds -m "${_fwRGISDomainFILE}" "${fwSOURCE[4]}" "${fwFILENAME}"
			else
            	echo "  ${fwInputITEM} datafile [${fwSOURCE[4]}] is missing!"
        	fi
        fi
	done

	if [ ${_fwSTART} == "TRUE" ]
	then
		[ "${_fwVERBOSE}" == "on" ] && echo "         Initial Variables:"
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		do
			local fwInputITEM=${_fwStateARRAY[${fwI}]}
			local    fwSOURCE=($(_fwDataSource "${fwInputITEM}" "static"))
			[ "${fwSOURCE[0]}" == "" ] && continue;
			if [ "${fwSOURCE[3]}" == "const" ]
			then
				[ "${_fwVERBOSE}" == "on" ] && echo "            ${fwInputITEM} Constant input"
			elif [ "${fwSOURCE[3]}" == "file" ]
			then
				[ "${_fwVERBOSE}" == "on" ] && echo "            ${fwInputITEM} File input"
            	if [ -e "${fwSOURCE[4]}" ]
            	then
					[ -e "${_fwGDSDomainDIR}/${fwSOURCE[2]}" ] || mkdir -p "${_fwGDSDomainDIR}/${fwSOURCE[2]}"
					local fwFILENAME="$(FwGDSFilename "${fwInputITEM}" "State" "${fwSOURCE[2]}" "${fwInYEAR}" "d")"
	            	rgis2ds -m "${_fwRGISDomainFILE}" "${fwSOURCE[4]}" "${fwFILENAME}"
				else
            			echo "  ${fwInputITEM} datafile [${fwSOURCE[4]}] is missing!"
        		fi
	        fi
		done
		_fwSTART="FALSE"
	fi
	[ "${_fwVERBOSE}" == "on"  ] && echo "      Preprocessing  ${fwYEAR} finished: $(date '+%Y-%m-%d %H:%M:%S')"
	return 0
}

function _fwPostprocess () {
    local fwExperiment="${1}"; shift
	local       fwYEAR="${1}"; shift

	if [ "${fwYEAR}" == "" ]; then local fwSUFFIX="LT"; else local fwSUFFIX="TS${fwYEAR}"; fi
	[ "${_fwVERBOSE}" == "on" ] && { echo "      Postprocessing  ${fwYEAR} started: $(date '+%Y-%m-%d %H:%M:%S')"; }

	local procNum=0
	local files=""
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		if (( ${procNum} < $(( ${_fwMAXPROC} / 4 )) )) # Limiting data stream conversions
    	then
			local procNum=$((${procNum} + 1))
		else
    	    local procNum=0
			wait
	    fi
		local fwVARIABLE="${_fwOutputARRAY[${fwI}]}"
		local    fwAMODE="$(_fwVariable "${fwVARIABLE}")"
		[ "${fwAMODE}" == "" ] && { echo "Skipping undefinded variable [${fwVARIABLE}]"; continue; }
		local fwGDSFileNAME="$(FwGDSFilename "${fwVARIABLE}" "Output" "${fwExperiment}" "${fwYEAR}" "d")"
		[ -e "${fwGDSFileNAME}" ] || ( echo "Skipping missing variable [${fwVARIABLE}]"; echo ${fwGDSFileNAME}; ) || continue

		(local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwExperiment}" "a" "${fwYEAR}")"
		 [ -e "${fwRGISFileNAME%/*}" ] || mkdir -p "${fwRGISFileNAME%/*}"
		 dsAggregate -e year -a ${fwAMODE} "${fwGDSFileNAME}" - |\
		 ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwExperiment} (${FwDomainRES}, Yearly${fwSUFFIX})" \
		         -m ${_fwRGISDomainFILE} -d "${_fwDomainNAME}" -u "${fwVARIABLE}"  -s blue - ${fwRGISFileNAME}) &
		(local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwExperiment}" "m" "${fwYEAR}")"
		 [ -e "${fwRGISFileNAME%/*}" ] || mkdir -p "${fwRGISFileNAME%/*}"
		 dsAggregate -e month -a ${fwAMODE} "${fwGDSFileNAME}" - |\
		 ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwExperiment} (${FwDomainRES}, Monthly${fwSUFFIX})" \
		         -m ${_fwRGISDomainFILE}  -d "${_fwDomainNAME}" -u "${fwVARIABLE}" -s blue - ${fwRGISFileNAME}) &
		if [ "${_fwDAILYOUTPUT}" == "on" ]
		then
			(local fwRGISFileNAME="$(FwRGISFilename "${fwVARIABLE}" "${fwExperiment}" "d" "${fwYEAR}")"
			 [ -e "${fwRGISFileNAME%/*}" ] || mkdir -p "${fwRGISFileNAME%/*}"
			 ds2rgis -t "${_fwDomainNAME}, ${fwVARIABLE} ${fwExperiment} (${FwDomainRES}, Daily${fwSUFFIX})"  \
			                      -m ${_fwRGISDomainFILE} -d "${_fwDomainNAME}" -u "${fwVARIABLE}" -s blue \
			                      "${fwGDSFileNAME}" "${fwRGISFileNAME}") &
		fi
	done
	wait
	for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
	do
		local fwVARIABLE="${_fwOutputARRAY[${fwI}]}"
		local fwGDSFileNAME="$(FwGDSFilename "${fwVARIABLE}" "Output" "${fwExperiment}" "${fwYEAR}" "d")"
		case "${_fwCLEANUP}" in
			("on")
				[ -e "${fwGDSFileNAME}" ] && rm "${fwGDSFileNAME}" &
			;;
			("off")
				[ -e "${fwGDSFileNAME}.gz" ] && rm "${fwGDSFileNAME}.gz" # removing possible gzip residures from broken runs.
				[ -e "${fwGDSFileNAME}"    ] && gzip "${fwGDSFileNAME}" &
			;;
			(*)
				local dstDir=$(echo ${_fwGDSDomainDIR} | sed "s:${_fwGDSWorkDIR}:${_fwCLEANUP}:")/${fwExperiment}
				[ -e "${dstDir}" ] || mkdir -p "${dstDir}"
				if [ -e "${dstDir}" ]
				then
					[ -e "${fwGDSFileNAME}.gz" ] && rm "${fwGDSFileNAME}.gz" # removing possible gzip residures from broken runs.
					(gzip "${fwGDSFileNAME}"     && mv -f "${fwGDSFileNAME}.gz" "${dstDir}/") &
				else
					rm "${fwGDSFileNAME}" &
				fi
			;;
		esac
	done
	wait
	[[ "${dstDir}" != "" &&  -e "${dstDir}" ]] && mv ${_fwGDSDomainDIR}/${fwExperiment}/*.log "${dstDir}/"
	[ "${_fwVERBOSE}" == "on" ] && { echo "      Postprocessing ${fwYEAR} finished: $(date '+%Y-%m-%d %H:%M:%S')"; }
	return 0
}

function _fwSpinup () {
	local fwExperiment="${1}"; shift

	local fwPASS
	local fwInputITEM
	local fwOutputITEM
	local doState="dostate"

	[ "${_fwVERBOSE}" == "on" ] && echo "Initialization started:  $(date '+%Y-%m-%d %H:%M:%S')"
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
				echo "-i ${fwSOURCE[0]}=file:$(FwGDSFilename "${fwSOURCE[0]}" "Input" "${fwSOURCE[2]}" "" "d")"
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
				echo "-i ${fwOutputITEM}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "State" "${fwExperiment}" "" "d")"
			done
		fi
		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		do
			local fwOutputITEM=(${_fwStateARRAY[${fwI}]})
			echo "-t ${fwOutputITEM[0]}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "State"  "${fwExperiment}" "" "d")"
		done
		if (( fwPASS == _fwPASSNUM ))
		then
			for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
			do
				local fwOutputITEM=(${_fwOutputARRAY[${fwI}]})
				echo "-o ${fwOutputITEM[0]}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "Output" "${fwExperiment}" "" "d")"
			done
		fi)

		echo "${fwOptions}" > ${fwOptionsFILE}
		[ "${_fwVERBOSE}" == "on" ] && echo "   Passnum [${fwPASS}] started:  $(date '+%Y-%m-%d %H:%M:%S')"

        echo ${fwOptions} | xargs ${_fwModelBIN} || return 1
	done
	_fwPostprocess "${fwExperiment}" "" || return 1
	[ "${_fwVERBOSE}"      == "on" ] && echo "Initialization finished: $(date '+%Y-%m-%d %H:%M:%S')"

	local fwInputList=$(echo "${fwOptions}" | grep -e "-i" | grep -e "file:"| grep -e "Input" | sed "s:.*file\:\(.*\):\1:")
	[ "${fwInputList}" == "" ] || rm -f ${fwInputList}
	return 0
}

function _fwRun () {
   local      fwExperiment="${1}"; shift
	local   fwStartYEAR="${1}"; shift
	local     fwEndYEAR="${1}"; shift

	local fwStateList=""
	[ "${_fwVERBOSE}" == "on" ] && echo "Model run started:  $(date '+%Y-%m-%d %H:%M:%S')"
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
				echo "-i ${fwInputITEM}=file:$(FwGDSFilename "${fwInputITEM[0]}" "State" "${fwExperiment}" "" "d")"
			done
		else
			for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
		 	do
				local fwInputITEM=(${_fwStateARRAY[${fwI}]})
				echo "-i ${fwInputITEM}=file:$(FwGDSFilename "${fwInputITEM[0]}" "State" "${fwExperiment}" "$(( fwYEAR - 1 ))" "d")"
			done
		fi

		for (( fwI = 0; fwI < ${#_fwStateARRAY[@]} ; ++fwI ))
 		do
			local fwStateITEM=(${_fwStateARRAY[${fwI}]})
			echo "-t ${fwStateITEM}=file:$(FwGDSFilename "${fwStateITEM}" "State" "${fwExperiment}" "${fwYEAR}" "d")"
		done
		for (( fwI = 0; fwI < ${#_fwOutputARRAY[@]} ; ++fwI ))
 		do
			local fwOutputITEM=${_fwOutputARRAY[${fwI}]}
			echo "-o ${fwOutputITEM}=file:$(FwGDSFilename "${fwOutputITEM[0]}" "Output" "${fwExperiment}" "${fwYEAR}" "d")"
 		done)

		echo "${fwOptions}" > ${fwOptionsFILE}

		[ "${_fwVERBOSE}" == "on" ] && echo "   Running year [${fwYEAR}] started:  $(date '+%Y-%m-%d %H:%M:%S')"
		_fwPreprocess "${fwYEAR}"                 || return 1
        echo ${fwOptions} | xargs ${_fwModelBIN}  || return 1
		_fwPostprocess "${fwExperiment}" "${fwYEAR}" || return 1
		local fwInputList=$(echo "${fwOptions}" | grep -e "-i" | grep -e "file:"| grep -e "Input" | sed "s:.*file\:\(.*\):\1:")
		[ "${fwInputList}" == "" ] || rm -f ${fwInputList}
	done
	[ "${_fwVERBOSE}" == "on" ] && echo "Model run finished: $(date '+%Y-%m-%d %H:%M:%S')"
	return 0
}

function FwRun () {
	local fwExperiment="${1}"; shift
	local  fwStartYEAR="${1}"; shift
	local    fwEndYEAR="${1}"; shift

	     _fwGDSLogDIR="${_fwGDSDomainDIR}/${fwExperiment}"

	[ "${_fwTESTONLY}" == "on" ] && { { _fwPrintTest || return 1; } && return 0; }
	[ -e "${_fwGDSLogDIR}" ] || mkdir -p ${_fwGDSLogDIR}

	[ "${_fwSPINUP}"   == "on" ] && [ "${_fwRESTART}" == "" ] && { _fwSpinup "${fwExperiment}"    || { echo "Spinup failed";    return 1; } }
	[ "${_fwFINALRUN}" == "on" ] && { _fwRun    "${fwExperiment}" "${fwStartYEAR}" "${fwEndYEAR}" || { echo "Final run failed"; return 1; } }
	local fwGDSFileNAME="$(echo $(FwGDSFilename "*" "State" "${fwExperiment}" "*" "") | sed "s:TS::")"
	case "${_fwCLEANUP}" in
		("on")
			for fwGDSfile in ${fwGDSFileNAME}
			do
				rm "${fwGDSfile}"
			done
			wait
		;;
		("off")
			for fwGDSfile in ${fwGDSFileNAME}
			do
				[[ -e ${fwGDSfile}.gz ]] && rm ${fwGDSfile}.gz
			 	gzip ${fwGDSfile} &
			done
			wait
		;;
		(*)
			local dstDir=$(echo ${_fwGDSDomainDIR} | sed "s:${_fwGDSWorkDIR}:${_fwCLEANUP}:")
			if [ -e "${dstDir}" ]
			then
				mv "${_fwGDSDomainFILE}" "${dstDir}"
				for fwGDSfile in ${fwGDSFileNAME}
				do
					[ -e "${fwGDSfile}.gz" ] && rm "${fwGDSfile}.gz" # removing possible gzip residures from broken runs.
					(gzip "${fwGDSfile}"; mv -f "${fwGDSfile}.gz" "${dstDir}/${fwExperiment}") &
				done
				wait
			else
				for fwGDSfile in ${fwGDSFileNAME}
				do
					[[ -e ${fwGDSfile}.gz ]] && rm ${fwGDSfile}.gz
					[[ -e ${fwGDSfile}    ]] && rm ${fwGDSfile}
				done
				wait
			fi
			rmdir -p --ignore-fail-on-non-empty ${_fwGDSDomainDIR}/${fwExperiment}
		;;
	esac

	return 0
}