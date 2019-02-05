#!/bin/bash
  
##########################################################################################################
#
# PostgreSQL/PostGis scripts to interface with RGIS
#
##############################################################################################jjjkkj############

if [[ "${GHAASDIR}" == "" ]]; then GHAASDIR="/usr/local/share/ghaas"; fi
source "${GHAASDIR}/Scripts/RGISfunctions.sh"

if [[ "${PGHOST}" != "" ]]; then export _GHAASpgHostName="${PGHOST}"; else export _GHAASpgHostName="localhost";  fi
if [[ "${PGPORT}" != "" ]]; then export   _GHAASpgPortID="${PGPORT}"; else export _GHAASpgPortID="5432";         fi
if [[ "${SSLDIR}" != "" ]]; then export   _GHAASpgSSLdir="${SSLDIR}"; elif [ -e "${HOME}/.pgssl" ]; then export _GHAASpgSSLdir="${HOME}/.pgssl"; else _GHAASpgSSLdir=""; fi
if [[ "${_GHAASpgSSLdir}" != "" ]]
then
    if [[ -e "${_GHAASpgSSLdir}/${_GHAASpgHostName}-ca.pem" ]]
    then
        export _GHAASpgSSLhostCA="${_GHAASpgSSLdir}/${_GHAASpgHostName}-ca.pem"
    else
        export _GHAASpgSSLhostCA=""
    fi
    if [[ -e "${_GHAASpgSSLdir}/client-cert.pem" ]]
    then
        export _GHAASpgSSLclientCert="${_GHAASpgSSLdir}/client-cert.pem"
    else
        export _GHAASpgSSLclientCert=""
    fi
    if [[ -e "${_GHAASpgSSLdir}/client-key.pem" ]]
    then
        export _GHAASpgSSLclientKey="${_GHAASpgSSLdir}/client-key.pem"
    else
        export _GHAASpgSSLclientKey=""
    fi
else
    export _GHAASpgSSLhostCA=""
    export _GHAASpgSSLclientCert=""
    export _GHAASpgSSLclientKey=""
fi

if [[ "${PGUSER}" != "" ]]
then export _GHAASpgUserName="${PGUSER}"
else
    if [[ -e "${HOME}/.pgpass" ]]
    then
        export _GHAASpgUserName="$(cat "${HOME}/.pgpass" | grep "${_GHAASpgHostName}:${_GHAASpgPortID}:.*:" | head -n 1 | sed "s|${_GHAASpgHostName}:${_GHAASpgPortID}:\(.*\):\(.*\):\(.*\)|\2|")"
    else
        export _GHAASpgUserName="${LOGNAME}";
    fi
fi

function PGhostName () # Changes the hostname when provided as argument and returns the hostname
{
    local hostName="${1}"

	if [[ "${hostName}" == "" ]]
	then
	    local hostName="${_GHAASpgHostName}"
	else
	    export _GHAASpgHostName="${hostName}"
	fi

	echo "${hostName}"
}

function PGport () # Changes port when provided as argument and returns the port
{
    local portID="${1}"

	if [[ "${portID}" == "" ]]
	then
	    local portID="${_GHAASpgPortID}"
	else
	    export _GHAASpgPortID="${protID}"
	fi
	echo "${portID}"
}

function PGuserName () # Changes username when it is defined as argument otherwise returns the username
{
	local userName="${1}"

	if [[ "${userName}" == "" ]]
	then
	    local  userName="${_GHAASpgUserName}"
	else
	    export _GHAASpgUserName="${userName}"
	fi
	echo "${userName}"
}

function PGuserPassword () # Returns the password from the ~/.pgpass file for a given user. When the database ommitted it will pick the first entry for the user.
{
    	local userName="${1}"
        local   dbName="${2}"
        local passFile="${HOME}/.pgpass"

        if [[ "${userName}" == "" ]]; then local userName="${_GHAASpgUserName}"; fi
        if [[   "${dbName}" == "" ]]; then local   dbName=".*";         fi

        if [[ -f "${passFile}" ]]
        then
            echo "${_GHAASpgHostName}:${_GHAASpgPortID}:${dbName}"
            echo "$(cat "${passFile}" | grep "${_GHAASpgHostName}:${_GHAASpgPortID}:${dbName}" | tee grep.txt | sed "s|${_GHAASpgHostName}:${_GHAASpgPortID}:${dbName}:\(.*\):\(.*\)|\2|" | tee sed.txt | head -n 1)"
        else
            echo ""
        fi
}

function PGdbName ()
{
	local   dbName="${1}"
	local userName="${2}"

    if [[ "${userName}" == "" ]]; then local userName="$(PGuserName)"; fi
    if [[ "${dbName}"   == "" ]]; then local   dbName="${userName}";   fi
    local sslString="dbname=${dbName} user=${userName}"
    if [[ "${_GHAASpgSSLclientKey}"  != "" ]]; then local sslString="sslkey=${_GHAASpgSSLclientKey}   ${sslString}"; fi
	if [[ "${_GHAASpgSSLclientCert}" != "" ]]; then local sslString="sslcert=${_GHAASpgSSLclientCert} ${sslString}"; fi
	if [[ "${_GHAASpgSSLhostCA}"     != "" ]]; then local sslString="sslmode=verify-ca sslrootcert=${_GHAASpgSSLhostCA} ${sslString}";  fi

    echo "${sslString}"
}

function PGrasterDimension ()
{
	local resolution="${1}"
	local     coord0="${2}"
	local     coord1="${3}"
	 
	case "${resolution}" in
	("15sec")
		local seconds="15"
	;;
	("30sec")
		local seconds="30"
	;;
	("45sec")
		local seconds="45"
	;;
	("01min")
		local seconds="60"
	;;
	("1m30s")
		local seconds="90"
	;;
	("03min")
		local seconds="180"
	;;
	("3m45s")
		local seconds="225"
	;;
	("05min")
		local seconds="300"
	;;
	("06min")
		local seconds="360"
	;;
	("10min")
		local seconds="600"
	;;
	("15min")
		local seconds="900"
	;;
	("30min")
		local seconds="1800"
	;;
	(*)
		echo "Invalid resolution" > /dev/std/err
		return 1
	;;
	esac
	echo $(echo "(${coord1} - ${coord0}) * 3600 / ${seconds}" | bc)
}

function PGattribTable ()
{
	local    caseVal="${1}"
	local   database="$(RGIScase "${caseVal}" "${2}")"
	local     schema="$(RGIScase "${caseVal}" "${3}")"
	local    tblname="$(RGIScase "${caseVal}" "${4}")"
	local    idField="$(RGIScase "${caseVal}" "${5}")"
	local       geom="${7}"

	echo "DROP TABLE IF EXISTS \"public\".\"temp_tb\";
          CREATE TABLE \"public\".\"temp_tb\" AS (SELECT * FROM \"${schema}\".\"${tblname}\");
          ALTER TABLE \"public\".\"temp_tb\" DROP \"geom\";
 		  COPY \"public\".\"temp_tb\" TO STDOUT
 		  DELIMITER AS '	' NULL AS '-9999' CSV HEADER QUOTE AS '\"';
 		  DROP TABLE \"public\".\"temp_tb\";" |\
	psql -q "$(PGdbName "${database}")"
}

function RGISsetHeader ()
{
	local   rgisArchiv="${1}"
	local       domain="${2}"
	local      subject="${3}"
	local      product="${4}"
	local   resolution="${5}"
	local    tStepType="${6}"
	local        tStep="$(echo "${7}" | tr "[A-Z]" "[a-z]")"
	local    timeRange="${8}"
	local      version="${9}"
	local        title="${8}"
	local      comment="${10}"
    local     citation="${11}"
	local  institution="${12}"
	local   sourceInst="${13}"
	local sourcePerson="${14}"
	
	rgisFile="$(RGISfilePath "${rgisArchiv}" "${domain}" "${subject}" "${product}" "${resolution}" "${tStepType}" "${tStep}" "${timeRange}")"
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
	if [[ "${tStep}"        == "" ]]; then local     tStep="";        fi
	if [[ "${timeRange}"    == "" ]]; then local timeRange="";        fi
	if [[ "${version}"      == "" ]]; then local   version="pre0.01"; fi


	if [[ "${title}"        == "" ]]; then local        title="$(RGIStitle "${domain}" "${subject}" "${product}" "${resolution}" "static" "" "" "${version}")"; fi
	if [[ "${comment}"      == "" ]]; then local      comment="${domain} $(_RGISlookupFullName "${subject}") from ${product} at ${resolution}"; fi
	if [[ "${citation}"     == "" ]]; then local     citation="Pirated ${subject} from ${product}"; fi
	if [[ "${institution}"  == "" ]]; then local  institution="Advanced Science Research Center at the Graduate Center, CUNY"; fi
	if [[ "${sourceInst}"   == "" ]]; then local   sourceInst="City College of New York"; fi
	if [[ "${sourcePerson}" == "" ]]; then local sourcePerson="${person}"; fi
	setHeader  -t "${title}" -d "${domain}" -u "${subject}" -y "on" -c "${comment}" -i "${citation}" -n "${institution}" -o "${sourceInst}" -p "${sourcePerson}" -v "${version}" "${rgisFile}" "${rgisFile}"
}

function PGrasterize ()
{
	local    caseVal="${1}"
	local     dbName="$(RGIScase "${caseVal}" "${2}")"
	local     schema="$(RGIScase "${caseVal}" "${3}")"
	local    tblname="$(RGIScase "${caseVal}" "${4}")"
	local    idField="$(RGIScase "${caseVal}" "${5}")"
	local    initVal="$(RGIScase "${caseVal}" "${6}")"
	local       geom="$(RGIScase "${caseVal}" "${7}")"
	local rgisArchiv="${8}"
	local     domain="${9}"
	local    subject="${10}"
	local    product="${11}"
	local resolution="${12}"
	local extent_llx="${13}"
	local extent_lly="${14}"
	local extent_urx="${15}"
	local extent_ury="${16}"

	local rgisFile="$(RGISfilePath "${rgisArchiv}" "${domain}" "${subject}" "${product}" "${resolution}" "static")"

	[[ -e "${rgisFile/*}" ]] || mkdir -p "${rgisFile%/*}"

	local ncols=$(PGrasterDimension "${resolution}" "${extent_llx}" "${extent_urx}")
	local nrows=$(PGrasterDimension "${resolution}" "${extent_lly}" "${extent_ury}")

	gdal_rasterize -l "${schema}"."${tblname}" -a "${idField}" -init "${initVal}" -ot Integer  -of GTiff \
	               -ts "${ncols}" "${nrows}" -te "${extent_llx}" "${extent_lly}" "${extent_urx}" "${extent_ury}" \
	               "PG:$(PGdbName "${dbName}") password=$(PGuserPassword)" "${rgisFile%.gdbd*}.tif"

	gdal_translate -of AAIGrid  "${rgisFile%.gdbd*}.tif" "${rgisFile%.gdbd*}.grd"
	(echo "2"
	 echo "-9999"
	 echo "0"
	 echo "${rgisFile}" 
	 echo "0") | grdImport -b "${rgisFile%.gdbd*}.grd"
	rm  "${rgisFile%.gdbd*}.tif" "${rgisFile%.gdbd*}.grd.aux.xml" "${rgisFile%.gdbd*}.prj" "${rgisFile%.gdbd*}.grd"
	PGattribTable "sensitive" "${dbName}" "${schema}" "${tblname}" "${idField}" "${geom}" |\
	table2rgis - "${rgisFile%.gdbd*}.gdbt"

	tblJoinTables -t "${schema} ${tblname}" -u "Zones" -a "${rgisFile%.gdbd*}.gdbt" -e "DBItems" -o "DBItems" -r "GridValue" -j "${idField}" "${rgisFile}" - |\
	tblDeleteField -f "DBItems" - "${rgisFile}"
	rm "${rgisFile%.gdbd*}.gdbt"
	RGISsetHeader "${rgisArchiv}" "${domain}" "${subject}" "${product}" "${resolution}" "static" 
}
