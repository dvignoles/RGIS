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
if [[ "${PGUSER}" != "" ]]; then export _GHAASpgUserName="${PGUSER}"
else
    if [[ -e "${HOME}/.pgpass" ]]
    then
        export _GHAASpgUserName="$(cat "${HOME}/.pgpass" | grep "${_GHAASpgHostName}:${_GHAASpgPortID}:.*:" | head -n 1 | sed "s|${_GHAASpgHostName}:${_GHAASpgPortID}:\(.*\):\(.*\):\(.*\)|\2|")"
    else
        export _GHAASpgUserName="${LOGNAME}";
    fi
fi

export     _GHAASpgSSLhostCA=""
export _GHAASpgSSLclientCert=""
export  _GHAASpgSSLclientKey=""

function PGsslDir ()
{
    local   sslDir="${1}"

    if [[ "${sslDir}" != "" ]]; then local sslDir="${HOME}/.pgssl"; fi
    if [[   -e "${sslDir}"  ]]
    then
        export _GHAASpgSSLdir="${sslDir}"
        PGhostName "${_GHAASpgHostName}"
    else
        _GHAASpgSSLdir=""
        echo "Non existing ssl directory: ${sslDir}" > /dev/stderr
    fi
}

function PGhostName () # Changes the hostname or returns its value
{
    local hostName="${1}"

	if [[ "${hostName}" == "" ]]
	then
	    echo "${_GHAASpgHostName}"
	else
	    export _GHAASpgHostName="${hostName}"
        if [[ -e "${_GHAASpgSSLdir}" ]]
        then
            if [[ -e "${_GHAASpgSSLdir}/${hostName}-ca.pem" ]]
            then
                export _GHAASpgSSLhostCA="${_GHAASpgSSLdir}/${hostName}-ca.pem"
                PGuserName "${_GHAASpgUserName}"
            else
                export _GHAASpgSSLhostCA=""
            fi
        else
            export _GHAASpgSSLhostCA=""
        fi
	fi
}

function PGport () # Changes port or returns its value
{
    local portID="${1}"

	if [[ "${portID}" == "" ]]
	then
	    echo "${_GHAASpgPortID}"
	else
	    export _GHAASpgPortID="${protID}"
	fi
}

function PGuserName () # Changes username or returns its value
{
	local userName="${1}"

	if [[ "${userName}" == "" ]]
	then
	    echo "${_GHAASpgUserName}"
	else
	    export _GHAASpgUserName="${userName}"
        if [[ -e "${_GHAASpgSSLdir}" ]]
        then
            if [[ -e "${_GHAASpgSSLdir}/${userName}-cert.pem" ]]
            then
                export _GHAASpgSSLclientCert="${_GHAASpgSSLdir}/${userName}-cert.pem"
            else
                export _GHAASpgSSLclientCert=""
                echo "Missing user certificate: ${userName}-cert.pem" > /dev/stderr
            fi

            if [[ -e "${_GHAASpgSSLdir}/${userName}-key.pem" ]]
            then
                export _GHAASpgSSLclientKey="${_GHAASpgSSLdir}/${userName}-key.pem"
            else
                export _GHAASpgSSLclientKey=""
                echo "Missing user key: f${userName}-key.pem" > /dev/stderr
            fi
        else
            export _GHAASpgSSLclientCert=""
            export  _GHAASpgSSLclientKey=""
        fi
	fi
}

function PGuserPassword () # Returns the password from the ~/.pgpass file for a given user. When the database omitted it will pick the first entry for the user.
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

function PGsslHostCA () # Changes the host CA or returns its value
{
	local hostCA="${1}"

	if [[ "${hostCA}" == "" ]]
	then
    	echo "${_GHAASpgSSLhostCA}"
	else
	    export _GHAASpgSSLhostCA="${hostCA}"
	fi
}

function PGsslClientCert () # Changes the client certificate or returns its value
{
	local clientCert="${1}"

	if [[ "${clientCert}" == "" ]]
	then
    	echo "${_GHAASpgSSLclientCert}"
	else
	    export _GHAASpgSSLclientCert="${clientCert}"
	fi
}

function PGsslClientKey () # Changes the client key or returns its value
{
	local clientKey="${1}"

	if [[ "${clientKey}" == "" ]]
	then
	    local  clientKey="${_GHAASpgSSLclientKey}"
    	echo "${clientKey}"
	else
	    export _GHAASpgSSLclientKey="${clientKey}"
	fi
}

function PGdbName ()
{
	local   dbName="${1}"
	local userName="${2}"

    if [[ "${userName}" == "" ]]; then local userName="$(PGuserName)"; fi
    if [[ "${dbName}"   == "" ]]; then local   dbName="${userName}";   fi
    local sslString="host=${_GHAASpgHostName} dbname=${dbName} user=${userName}"
	if [[ "${_GHAASpgSSLhostCA}"     != "" ]]; then local sslString="${sslString} sslmode=verify-ca sslrootcert=${_GHAASpgSSLhostCA}";  fi
	if [[ "${_GHAASpgSSLclientCert}" != "" ]]; then local sslString="${sslString} sslcert=${_GHAASpgSSLclientCert}"; fi
    if [[ "${_GHAASpgSSLclientKey}"  != "" ]]; then local sslString="${sslString} sslkey=${_GHAASpgSSLclientKey}"; fi

    echo "${sslString}"
}

function PGpolygonColorize ()
{
    local   caseVal="${1}"
    local    dbName="$(RGIScase "${caseVal}" "${2}")"
    local    schema="$(RGIScase "${caseVal}" "${3}")"
    local   tblName="$(RGIScase "${caseVal}" "${4}")"
    local     field="$(RGIScase "${caseVal}" "${5}")"

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

if [[ "${GHAASSSLDIR}" != "" ]]; then PGsslDir "${GHAASSSLDIR}"; else export _GHAASpgSSLdir=""; fi