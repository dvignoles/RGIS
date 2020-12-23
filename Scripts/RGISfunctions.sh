#!/bin/bash

case "$(uname)" in
    (Linux)
        export GHAASprocessorNum=$(nproc)
    ;;
    (Darwin)
        export GHAASprocessorNum=$(sysctl -n hw.ncpu)
    ;;
    (*)
        export GHAASprocessorNum=1
    ;;
esac
(( ${GHAASprocessorNum} <=32 )) || export GHAASprocessorNum=32

function GHAASprojectDir () {
    local arg="${1}"; shift;

    if [[ "${arg%/*}"     != "." ]]; then projectDir="${arg%/*}"; projectDir="${projectDir%/Scripts}"; else prodjectDir=".."; fi
    if [[ "${projectDir}" == "." ]]; then projectDir="$(pwd)"; fi

    echo "${projectDir}"
}

function GHAASscriptName () {
    local arg="${1}"; shift;

     scriptName="${arg##*/}"; scriptName="${scriptName%.sh}";
     echo "${scriptName}"
}

function RGISprocessorNum () {
    local processorNum="${0}"; shift

    export GHAASprocessorNum="${processorNum}"
}
export __RGISarchiveFormat="gzipped"

function RGISarchiveFormat () {
   local format="${1}"; shift

   case "${format}" in
        (plain)
            export __RGISarchiveFormat="plain"
        ;;
        (gzipped)
            export __RGISarchiveFormat="gzipped"
        ;;
        (netcdf)
            export __RGISarchiveFormat="netcdf"
        ;;
        (*)
            echo "Unrecognised file format:${format} in: RGISarchiveFormat"
            export __RGISarchiveFormat="plain"
        ;;
   esac
}

export __RGISarchiveVDBversion="vdb2"

function RGISarchiveVDBversion () {
   local vdbVersion="${1}"; shift

   case "${vdbVersion}" in
        (vdb2|vdb3)
            export __RGISarchiveVDBversion="${vdbVersion}"
        ;;
        (*)
            echo "Unrecognised VDBversion:${vdbVersion} in: RGISarchiveVDBversion"
            export __RGISarchiveVDBversion="${vdbVersion}"
        ;;
   esac
}

function RGIScase () {
    local caseVal="${1}"; shift
    local  string="${1}"; shift

    case "${caseVal}" in
        (lower)
            echo "$(echo "${string}"  | tr "[A-Z]" "[a-z]")"
        ;;
        (upper)
            echo "$(echo "${string}"  | tr "[a-z]" "[A-Z]")"
        ;;
        (*)
            echo "${string}"
        ;;
    esac
}

function RGISlookupSubject () {
    local   variable="${1}"; shift

    local subject="$(vdbLookup -g "${variable}" -v "${__RGISarchiveVDBversion}")"
    if [[ "${subject}" == "" ]]
    then
        echo "${variable}"
    else
        echo "${subject}"
    fi
}

function RGISlookupFullName () {
    local   variable="${1}"; shift
    local vdbVersion="${1}";

    local fullName="$(vdbLookup -l "${variable}" -v "${__RGISarchiveVDBversion}")"
    if [[ "${fullName}" == "" ]]
    then
        echo "${variable}"
    else
        echo "${fullName}"
    fi
}

function RGISlookupShadeset () {
    local   variable="${1}"; shift
    local vdbVersion="${1}";

    local shadeSet="$(vdbLookup -s "${variable}" -v "${__RGISarchiveVDBversion}")"
    if [[ "${shadeSet}" == "" ]]
    then
        echo "grey"
    else
        echo "${shadeSet}"
    fi
}

function RGISlookupAggrMethod () {
    local   variable="${1}"; shift
    local vdbVersion="${1}";

    local aggreg="$(vdbLookup -a "${variable}" -v "${__RGISarchiveVDBversion}")"
    if [[ "${aggreg}" == "" ]]
    then
        echo "avg"
    else
        echo "${aggreg}"
    fi
}

function RGISlookupTimeStep () {
	local timeStep=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	case "${timeStep}" in
		(daily)
			echo "day"
		;;
		(monthly)
			echo "month"
		;;
		(annual)
			echo "year"
		;;
	esac
}

function RGIStimeNumberOfDays () {
	local    year="${1}";          shift
	local   month="$((${1} - 1))"; shift
	local century="${year%??}"

	local monthLength[0]=31
	local monthLength[1]=28
	local monthLength[2]=31
	local monthLength[3]=30
	local monthLength[4]=31
	local monthLength[5]=30
	local monthLength[6]=31
	local monthLength[7]=31
	local monthLength[8]=30
	local monthLength[9]=31
	local monthLength[10]=30
	local monthLength[11]=31

	if (( month == 1))
	then
		if [[ "${year}" == "${century}00" ]]
		then
			local leapYear=$(( (${century} & 0xffffe) == ${century} ? 1 : 0))
		else
			local leapYear=$(( (${year}    & 0xffffe) == ${year}    ? 1 : 0))
		fi
	else
		local leapYear=0
	fi
	echo $((${monthLength[${month}]} + ${leapYear}))
}

function _RGISvariableDir () {
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift

	local       varDir=$(RGISlookupSubject "${variable}")

	if [ -e "${archive}/${domain%+}/${varDir}" ]
	then
		echo "${domain%+}/${varDir}"
		return 0
	else
		if [ "${domain%+}" == "${domain}" ] # Exact domain
		then
			echo "Missing ${archive}/${domain}/${varDir}" > /dev/stderr
			echo ""
			return 1
		else # Dynamic domain
			local parent="${archive}/${domain%+}/parent"
			if [ -e "${parent}" ]
			then
				_RGISvariableDir "${archive}" "$(cat ${archive}/${domain%+}/parent)+" "${variable}" || return 1
			else
				echo "Missing ${archive}/${domain%+}/${varDir}" > /dev/stderr
				echo ""
				return 1
			fi
		fi
	fi
	return 0
}

function RGISgeoResolutionInSecond () {
    local resolution="${1}"; shift

   	case "${resolution}" in
   	("03sec")
   	    echo "3"
   	 ;;
   	("7p50s")
   	    echo "7.5"
   	 ;;
	("15sec")
		echo "15"
	;;
	("30sec")
		echo "30"
	;;
	("45sec")
		echo "45"
	;;
	("01min")
		echo "60"
	;;
	("1m30s")
		echo "90"
	;;
	("2m30s")
		echo "150"
	;;
	("03min")
		echo "180"
	;;
	("3m45s")
		echo "225"
	;;
	("05min")
		echo "300"
	;;
	("06min")
		echo "360"
	;;
	("7m30s")
		echo "450"
	;;
	("10min")
		echo "600"
	;;
	("15min")
		echo "900"
	;;
	("30min")
		echo "1800"
	;;
	("60min")
		echo "3600"
	;;
	("02deg")
		echo "7200"
	;;
	(*)
		echo "Invalid resolution" > /dev/stderr
		return 1
	;;
	esac
    return 0
}

function RGISgeoResolutionMultiplier () { # Destination resolution is devided by source resolution. Decimals ar chapped off.
    local srcRes="$(RGISgeoResolutionInSecond "${1}")"; shift
    local dstRes="$(RGISgeoResolutionInSecond "${1}")"; shift

    if [[ "${srcRes}" == "" || "${dstRes}" == "" ]]
    then
        echo "Invalid resolutions: ${srcRes}, ${dstRes}" > /dev/stderr
        echo "Usage: Destination resolution is devided by source resolution. Decimals ar chapped off." > /dev/stderr
        return 1
    else
        echo "${dstRes} / ${srcRes}" | bc -l | sed "s:\([0-9]*\).*:\1:"
    fi
    return 0

}
function _RGISresolutionDir () {
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift
	local      product="${1}"; shift
	local   resolution="${1}"; shift

	local varDir=$(_RGISvariableDir "${archive}" "${domain}" "${variable}")
	if [ "${varDir}" == "" ]
	then
		return
	else
		if [ -e "${archive}/${varDir}/${product}/${resolution%+}" ]
		then
			echo "${varDir}/${product}/${resolution%+}"
		else
			if [ "${resolution%+}" == "${resolution}" ] # Exact resolution
			then
				echo "Missing ${archive}/${varDir}/${product}/${resolution}" > /dev/stderr
				echo ""
				return 1
			else # Dynamic resolution
				if [ "${resolution%km+}" == "${resolution}" ] # Geographic
				then
					local geogNum=0
					local geogRes[${geogNum}]="7p50s"; (( ++geogNum ))
					local geogRes[${geogNum}]="15sec"; (( ++geogNum ))
					local geogRes[${geogNum}]="30sec"; (( ++geogNum ))
					local geogRes[${geogNum}]="45sec"; (( ++geogNum ))
					local geogRes[${geogNum}]="01min"; (( ++geogNum ))
					local geogRes[${geogNum}]="1m30s"; (( ++geogNum ))
					local geogRes[${geogNum}]="2m30s"; (( ++geogNum ))
					local geogRes[${geogNum}]="03min"; (( ++geogNum ))
					local geogRes[${geogNum}]="3m45s"; (( ++geogNum ))
					local geogRes[${geogNum}]="05min"; (( ++geogNum ))
					local geogRes[${geogNum}]="06min"; (( ++geogNum ))
					local geogRes[${geogNum}]="7m30s"; (( ++geogNum ))
					local geogRes[${geogNum}]="10min"; (( ++geogNum ))
					local geogRes[${geogNum}]="15min"; (( ++geogNum ))
					local geogRes[${geogNum}]="30min"; (( ++geogNum ))
					local geogRes[${geogNum}]="60min"; (( ++geogNum ))
					local geogRes[${geogNum}]="1d30m"; (( ++geogNum ))
					local geogRes[${geogNum}]="02deg"; (( ++geogNum ))
					local geogRes[${geogNum}]="2d30m"; (( ++geogNum ))

					for (( num = 0; num < ${geogNum}; ++num ))
					do
						if [ "${geogRes[${num}]}" == "${resolution%+}" ]; then break; fi
					done
					if (( num < geogNum))
					then
						_RGISresolutionDir "${archive}" "${domain}" "${variable}"  "${product}" "${geogRes[((${num} + 1))]}+" || return 1
					else
						echo "No coarser resolution ${domain%+} ${variable} ${product} ${resolution%+}" > /dev/stderr
						return 1
					fi
				else # Cartesian
					local cartNum=0
					local cartRes[${cartNum}]="1km";    (( ++cartNum ))
					local cartRes[${cartNum}]="2km";    (( ++cartNum ))
					local cartRes[${cartNum}]="5km";    (( ++cartNum ))
					local cartRes[${cartNum}]="10km";   (( ++cartNum ))
					local cartRes[${cartNum}]="25km";   (( ++cartNum ))
					local cartRes[${cartNum}]="50km";   (( ++cartNum ))

					for (( num = 0; num < ${cartNum}; ++num ))
					do
						if [ "${cartRes[${num}]}" == "${resolution%+}" ]; then break; fi
					done
					if (( num < cartNum))
					then
						_RGISresolutionDir "${archive}" "${domain}" "${variable}"  "${product}" "${cartRes[((${num} + 1))]}+" || return 1
					else
						echo "No coarser resolution ${domain%+} ${variable} ${product} ${resolution%+}" > /dev/stderr
						return 1
					fi
				fi
			fi
		fi
	fi
	return 0
}

function _RGIStStepDir () {
	local tStepType="${1}"; shift
	local     tStep="${1}"; shift

	case "${tStepType}" in
		(TS)
			case "${tStep}" in
				(hourly)
					echo "Hourly"
				;;
				(3hourly)
					echo "3Hourly"
				;;
				(6hourly)
					echo "6Hourly"
				;;
				(daily)
					echo "Daily"
				;;
				(monthly)
					echo "Monthly"
				;;
				(annual)
					echo "Annual"
				;;
				(*)
					echo "${tStep}"
				;;
			esac
		;;
		(LT|LTmin|LTmax|LTslope|LTrange|LTstdDev|Stats|static)
			echo "Static"
		;;
	esac
	return 0
}

function RGISdirectoryPath () {
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local  tStepType="${1}"; shift
	local      tStep=$(echo "${1}" | tr "[A-Z]" "[a-z]"); shift

	local        dir=$(_RGIStStepDir ${tStepType} ${tStep})
	local     varDir=$(RGISlookupSubject "${variable}")

	echo "${archive}/${domain}/${varDir}/${product}/${resolution}/${dir}"
}

function RGISdirectory () {
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local  tStepType="${1}"; shift
	local      tStep=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	local dir=$(_RGIStStepDir ${tStepType} ${tStep})

	local resDir=$(_RGISresolutionDir "${archive}" "${domain}" "${variable}" "${product}" "${resolution}")
	if [ "${resDir}" == "" ]
	then
		return 1
	else
		echo "${archive}/${resDir}/${dir}"
	fi
	return 0
}

function RGISfileExtension () {
	local variable="${1}"; shift

    local dataType="$(vdbLookup -t "${variable}" -v "${__RGISarchiveVDBversion}")"
    if [[ "${dataType}" == "" ]]
    then
        local dataType="continuous"
    fi
	case "${dataType}" in
		(network)
			local extension="gdbn"
		;;
		(point)
			local extension="gdbp"
		;;
		(line)
			local extension="gdbl"
		;;
		(polygon)
			local extension="gdbp"
		;;
		(discrete)
			local extension="gdbd"
		;;
		(*)
			local extension="gdbc"
		;;
    esac
    echo "${extension}"
}

function RGISfilePath () {
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift
	local      product="${1}"; shift
	local   resolution="${1}"; shift
	local    tStepType="${1}"; shift
	local        tStep=$(echo "${1}" | tr "[A-Z]" "[a-z]"); shift
	local    timeRange="${1}"; shift

	USAGE="Usage: RGISfilePath <archive> <domain> <variable> <product> <resolution> <tstep type> <tstep> [time range]"
    if [[ "${tStepType}" == "" ]]; then echo "${USAGE}"; return 1; fi

	case "${tStep}" in
		(hourly)
			local tStepStr="h"
		;;
		(3hourly)
			local tStepStr="3h"
		;;
		(6hourly)
			local tStepStr="6h"
		;;
		(daily)
			local tStepStr="d"
		;;
		(monthly)
			local tStepStr="m"
		;;
		(annual)
			local tStepStr="a"
		;;
		("")
			local tStepStr=""
		;;
		(*)
			echo "Unknown time step ${tStep}" > /dev/stderr
			return 1
		;;
	esac

    local extension=$(RGISfileExtension "${variable}")

	local rgisDirectory=$(RGISdirectoryPath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(RGISdirectoryPath            "" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(echo ${fileName} | sed "s:/::" | sed "s:/:_:g" )
	local      fileName=${fileName%_*}
	if [ "${rgisDirectory}" == "" ]
	then
		echo ""
		return 1
	fi
	if [[ "${tStepType}" == "static" ]]
	then
		local tStepType="Static"
		local tStepStr=""
	fi

    case "${__RGISarchiveFormat}" in
        (gzipped)
            echo "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}.gz"
        ;;
        (netcdf)
            echo "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.nc"
        ;;
        (*)
        	echo "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}"
        ;;
    esac
}

function RGISfile () {
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift
	local      product="${1}"; shift
	local   resolution="${1}"; shift
	local    tStepType="${1}"; shift
	local        tStep=$(echo "${1}" | tr "[A-Z]" "[a-z]"); shift
	local    timeRange="${1}"; shift

	USAGE="Usage: RGISfile <archive> <domain> <variable> <product> <resolution> <tstep type> <tstep> [time range]"
    if [[ "${tStepType}" == "" ]]; then echo "${USAGE}"; return 1; fi

	case "${tStep}" in
		(hourly)
			local tStepStr="h"
		;;
		(3hourly)
			local tStepStr="3h"
		;;
		(6hourly)
			local tStepStr="6h"
		;;
		(daily)
			local tStepStr="d"
		;;
		(monthly)
			local tStepStr="m"
		;;
		(annual)
			local tStepStr="a"
		;;
		("")
			local tStepStr=""
		;;
		(*)
			echo "Unknown time step ${tStep}" > /dev/stderr
			return 1
		;;
	esac

    local extension=$(RGISfileExtension "${variable}")

	local rgisDirectory=$(RGISdirectory      "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(_RGISresolutionDir "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" | sed "s:/:_:g" )
	if [ "${rgisDirectory}" == "" ]
	then
		echo ""
		return 1
	fi
	if [[ "${tStepType}" == "static" ]]
	then
		local tStepType="Static"
		local tStepStr=""
	fi

	if [[ "${timeRange}" == "xxxx" ]]
	then
		 ncPattern="${rgisDirectory}/${fileName}_${tStepStr}${tStepType}????.nc"
		gdbPattern="${rgisDirectory}/${fileName}_${tStepStr}${tStepType}????.${extension}"
		 gzPattern="${rgisDirectory}/${fileName}_${tStepStr}${tStepType}????.${extension}.gz"

		[ "$(ls -1  ${ncPattern} 2> /dev/null)" != "" ] && { echo  "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}xxxx.nc";              return 0; }
		[ "$(ls -1 ${gdbPattern} 2> /dev/null)" != "" ] && { echo  "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}xxxx.${extension}";    return 0; }
		[ "$(ls -1  ${gzPattern} 2> /dev/null)" != "" ] && { echo  "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}xxxx.${extension}.gz"; return 0; }
	else
   	[ -e "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.nc" ]              && { echo    "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.nc";              return 0;}
   	[ -e "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}.gz" ] && { echo    "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}.gz"; return 0;}
   	[ -e "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}" ]    && { echo    "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}";    return 0;}
   fi
   echo ""
   return 1
}

function RGIStitle () {
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local  tStepType="${1}"; shift
	local      tStep="${1}"; shift
	local  timeRange="${1}"; shift
	local    version="${1}"; shift

	USAGE="Usage: RGIStitle <domain> <variable> <product> <resolution> <tstep type> <tstep> <time range> <version>"
    if [[ "${tStepType}" == "" ]]; then echo "${USAGE}"; return 1; fi

	local variableFullName=$(RGISlookupFullName "${variable}")

	case "$(echo "${tStepType}" | tr "[A-Z]" "[a-z]")" in
	    (static)
		    local tStepString=""
		;;
	    (lt|ts)
    		case "$(echo "${tStep}" | tr "[A-Z]" "[a-z]")" in
			    (hourly)
				    local tStepStr="Hourly"
			    ;;
			    (3hourly)
				    local tStepStr="3Hourly"
			    ;;
			    (6hourly)
				    local tStepStr="6Hourly"
			    ;;
			    (daily)
				    local tStepStr="Daily"
			    ;;
			    (monthly)
				    local tStepStr="Monthly"
			    ;;
			    (annual)
				    local tStepStr="Annual"
			    ;;
			    (*)
				    echo "Unknown time step ${tStep}"  > /dev/stderr
			    ;;
    		esac
		    if [[ "${timeRange}" == "" ]]
		    then
			    local tStepString=", ${tStepStr}$(echo "${tStepType}" | tr "[a-z]" "[A-Z]")"
		    else
			    local tStepString=", ${tStepStr}$(echo "${tStepType}" | tr "[a-z]" "[A-Z]"), ${timeRange}"
		    fi
        ;;
    	(*)
    	    echo "Unknown time step type"  > /dev/stderr
    	;;
    esac
    if [[ "${version}" == "" ]]
    then
    	echo "${domain}, ${product} ${variableFullName} (${resolution}${tStepString})"
    else
    	echo "${domain}, ${product} ${variableFullName} (${resolution}${tStepString}) V${version}"
    fi
	return 0
}

function RGISappend () {
    local    archive="${1}"; shift
    local     domain="${1}"; shift
    local   variable="${1}"; shift
    local    product="${1}"; shift
    local resolution="${1}"; shift
    local    version="${1}"; shift
    local  startyear="${1}"; shift
	local    endyear="${1}"; shift
	local      tStep="${1}"; shift

	USAGE="Usage: RGISSAppend <archive> <domain> <variable> <product> <resolution> <version> <start year> <end year> <time step>"
    if [[ "${tStep}" == "" ]]; then echo "${USAGE}"; return 1; fi

 	local      files=""
	local  separator=" "

	for (( year = ${startyear}; year <= ${endyear}; ++year ))
	do
		local  filename=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${year}")
		local     files="${files}${separator}${filename}"
		local separator=" "
	done

	local filename=$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${startyear}-${endyear}")
	local    title=$(RGIStitle                  "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${startyear}-${endyear}" "${version}")
	local  subject=$(RGISlookupSubject  "${variable}")
    local shadeset=$(RGISlookupShadeset "${variable}")

	grdAppendLayers -t "${title}" -d "${domain}" -u "${subject}" -v "${version}" -o "${filename}" ${files}
}

function RGISsetHeader () {
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift
	local      product="${1}"; shift
	local   resolution="${1}"; shift
	local    tStepType="${1}"; shift
	local        tStep="${1}"; shift
	local    timeRange="${1}"; shift
	local      version="${1}"; shift
	local        title="${1}"; shift
	local      comment="${1}"; shift
	local     citation="${1}"; shift
	local  institution="${1}"; shift
	local   sourceInst="${1}"; shift
	local sourcePerson="${1}"; shift

	USAGE="Usage: RGISsetHeader <archive> <domain> <variable> <product> <resolution> <tstep type> <tstep> <time range> <version> <title> <comment> <citation> <institution> <source inst> <source person>"
    if [[ "${resolution}" == "" ]]; then echo "${USAGE}"; return 1; fi

	if [[ "$(RGIScase "lower" "${tStepType}")" == "static" ]]
	then
	    local rgisFile="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}")"
	elif [[     "${tStep}" == "" ]]
	then
	    local rgisFile="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "Annual")"
	elif [[ "${timeRange}" == "" ]]
	then
	    local rgisFile="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")"
	else
	    local rgisFile="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}" "${timeRange}")"
	fi

	if [[ "${rgisFile}" == "" ]]
	then
		echo "Missing ${rgisFile} in RGISsetHeader"
		return 0
	fi
	if [[ "$(which finger)" == "" ]]
	then
		local person="Annonymous"
	else
		local person=$(finger $(env | grep "LOGNAME" | sed "s:LOGNAME=::") | grep Name | sed -e "s|.*Name: ||")
	fi

	if [[ "${tStepType}"    == "" ]]; then local tStepType="static";  fi
	if [[ "${version}"      == "" ]]; then local   version="pre0.01"; fi

	if [[ "${title}"        == "" ]]; then local        title="$(RGIStitle "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}" "${timeRange}" "${version}")"; fi
	if [[ "${comment}"      == "" ]]; then local      comment="${domain} $(RGISlookupFullName "${variable}") from ${product} at ${resolution}"; fi
	if [[ "${citation}"     == "" ]]; then local     citation="Pirated ${variable} from ${product}"; fi
	if [[ "${institution}"  == "" ]]; then local  institution="Advanced Science Research Center at the Graduate Center, CUNY"; fi
	if [[ "${sourceInst}"   == "" ]]; then local   sourceInst="City College of New York"; fi
	if [[ "${sourcePerson}" == "" ]]; then local sourcePerson="${person}"; fi
	setHeader  -t "${title}" -d "${domain}" -u "$(RGISlookupSubject "${variable}")" -y "on" -c "${comment}" -i "${citation}" -n "${institution}" -o "${sourceInst}" -p "${sourcePerson}" -v "${version}" "${rgisFile}" "${rgisFile}"
}

function RGISaggregateTS () {
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local    version="${1}"; shift
	local  startyear="${1}"; shift
	local    endyear="${1}"; shift
	local  fromtStep="${1}"; shift
	local    totStep="${1}"; shift

	USAGE="Usage: RGISAggregateTS <archive> <domain> <variable> <product> <resolution> <version> <start year> <end year> <from timestep> <to timestep>"
    if [[ "${totStep}" == "" ]]; then echo "${USAGE}"; return 1; fi

   local     files=""
   local separator=""

   for ((year = ${startyear}; year <= ${endyear} ; ++year))
   do
      local fromFile=$(RGISfile     "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${fromtStep}" "${year}")
      local   toFile=$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${totStep}"   "${year}")
      local    title=$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${totStep}"   "${year}" "${version}")
      local  subject=$(RGISlookupSubject    "${variable}")
      local shadeset=$(RGISlookupShadeset   "${variable}")
      local   method=$(RGISlookupAggrMethod "${variable}")

      [ -e "${toFile%/*}" ] || mkdir -p "${toFile%/*}"
      grdTSAggr -a "${method}" -e "$(RGISlookupTimeStep ${totStep})" -t "${title}" -d "${domain}" -u "${subject}" -s "${shadeset}" "${fromFile}" "${toFile}" || return 1
   done
}

function RGISclimatology () {
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local   timeStep="$(echo "${1}" | tr "[A-Z]" "[a-z]")"; shift
 	local  startYear="${1}"; shift
	local    endYear="${1}"; shift

	USAGE="Usage: RGISclimatology <archive> <domain> <variable> <product> <resolution> [annual|monthly] [start year] [end year]"
    if [[ "${timeStep}" == "" ]]; then echo "${USAGE}"; return 1; fi

	if [[ "${startYear}" == "" ]]
	then
		if [[ "${endYear}" == "" ]]
		then
			local tsFiles=""
			local     sep=""
			for fileName in $(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "*")
			do
				local tsFiles="${tsFiles}${sep}${fileName}"
				local     sep=" "
			done
			local range=""
		else
			local tsFiles=""
			local     sep=""
			local lastFile="$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "${endYear}")"
			for fileName in $(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "*")
			do
				if [[ ${fileName} == ${lastFile} ]]
				then
					break;
				else
					local tsFiles="${tsFiles}${sep}${fileName}"
					local     sep=" "
				fi
			done
			local tsFiles="${tsFiles}${sep}${fileName}"
			local range="year-${endYear}"
		fi
	elif [[ "${endYear}" == "" ]]
	then
		local tsFiles=""
		local     sep=""
		for (( year = ${startYear}; ; ++year ))
			do
			local fileName="$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "${year}")"
			if [ -e ${fileName} ]
			then
				local tsFiles="${tsFiles}${sep}${fileName}"
				local     sep=" "
			else
				break;
			fi
			done
			local range="${startYear}-year"
	else
		local tsFiles=""
		local     sep=""
		for (( year = ${startYear}; year <= ${endYear}; ++year ))
			do
			local fileName="$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "${year}")"
			if [ -e ${fileName} ]
			then
				local tsFiles="${tsFiles}${sep}${fileName}"
				local     sep=" "
			else
				break;
			fi
		done
		local range="${startYear}-${endYear}"
	fi

	case "${timeStep}" in
	(annual)
		local     nStep="1"
		local tStepUnit="year"
	;;
	(monthly)
		local     nStep="12"
		local tStepUnit="month"
	;;
	(*)
		echo "Invalid time step: ${timeStep}"
		exit 1
	;;
	esac

	local fileName="$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT" "${timeStep}" "${range}")"
	local    title="$(RGIStitle                  "${domain}" "${variable}" "${product}" "${resolution}" "LT" "${timeStep}" "${range}")"
	[ -e "${fileName%/*}"  ] || mkdir -p "${fileName%/*}"
	grdAppendLayers ${tsFiles} |\
	grdCycleMean -n "${nStep}" -t "${title}" -u "$(RGISlookupSubject "${variable}")" -d "${domain}" -s "$(RGISlookupShadeset "${variable}")" |\
	grdDateLayers -e "${tStepUnit}" - "${fileName}"
}

function RGIScellStats () {
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local    version="${1}"; shift
 	local  startyear="${1}"; shift
	local    endyear="${1}"; shift

	USAGE="Usage: RGISCellStats <archive> <domain> <variable> <product> <resolution> <version> <start year> <end year>"
    if [[ "${endyear}" == "" ]]; then echo "${USAGE}"; return 1; fi

	local  statsDir=$(RGISdirectory  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual")
	local    tsFile=$(RGISfile       "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "annual" "${startyear}-${endyear}")
	local statsFile=$(RGISfilePath   "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual" "${startyear}-${endyear}")
	local     title=$(RGIStitle                   "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual" "${startyear}-${endyear}" "${version}")
	local   subject=$(RGISlookupSubject "${variable}")

	[ -e "${statsDir}" ] || mkdir -p "${statsDir}" || return 1
	grdCellStats    -t "${title}" -u "${variable}" -d "${domain}" -v "${version}" "${tsFile}" "${statsFile}" || return 1

	local  annualLTfile=$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT"      "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "LT"      "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "avg" -l "avg" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${annualLTfile}" || return 1

	local     maxLTfile=$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTmax"    "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "LTmax"    "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Maximum" -l "Maximum" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${maxLTfile}" || return 1

	local     minLTfile=$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTmin"    "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "LTmin"    "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Minimum" -l "Minimum" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${minLTfile}" || return 1

	local   rangeLTfile=$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTrange"  "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "LTrange"  "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Range"   -l "Range"   -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${rangeLTfile}" || return 1

	local   slopeLTfile=$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTslope"  "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "LTslope"  "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "SigSlopeB1" -l "SigSlopeB1" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${slopeLTfile}" || return 1

	local  stdDevLTfile=$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTstdDev" "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "LTstdDev" "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "StdDev" -l "StdDev" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${stdDevLTfile}" || return 1

	return 0
}

function RGISstatistics () {
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local    version="${1}"; shift
	local  startyear="${1}"; shift
	local    endyear="${1}"; shift

	USAGE="Usage: RGISStatistics <archive> <domain> <variable> <product> <resolution> <version> <start year> <end year>"
    if [[ "${endyear}" == "" ]]; then echo "${USAGE}"; return 1; fi

	RGISappend "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}" "monthly" || return 1
	RGISappend "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}" "annual"  || return 1

	RGISclimatology "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}"      || return 1
	RGIScellStats   "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}"      || return 1
	local  annualTSfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "annual"  "${startyear}-${endyear}")
	local monthlyTSfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "monthly" "${startyear}-${endyear}")
	[ -e  "${annualTSfile}" ] && rm  "${annualTSfile}"
	[ -e "${monthlyTSfile}" ] && rm "${monthlyTSfile}"
	return 0
}

if [[ ${0} == "${BASH_SOURCE}" ]]
then
    if (( $# > 1))
    then
        FUNCTION="${1}"; shift
        ARGUMENTS="$@"
        ${FUNCTION} ${ARGUMENTS}
    fi
fi