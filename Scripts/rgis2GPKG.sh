#!/usr/bin/env bash

function PrintUsage () {
	echo "Usage ${0##*/} [options] <rgisfile>"
	echo "      -c, --case [sensitive|lower|upper]"
	echo "      -m  --mode [append|replace]"
	echo "      -g, --geopackage <geopackage>"
	echo "      -s, --schema     <schema>"
	echo "      -t, --tblname    <tblname>"
	exit 1
}

function caseFunc () {
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

RGISFILE=""
  GEOPACKAGE=""
  SCHEMA=""
 TBLNAME=""
    CASE="sensitive"
	MODE="append"

function _GPKGsql () {
	local schemaName="${1}"; shift
	local  tableName="${1}"; shift
	local   dataType="${1}"; shift
	local   relateID="${1}"; shift
	local     joinID="${1}"; shift

	case "${dataType}" in
		(POINT|LINESTRING)
			echo "SELECT   \"${SCHEMA}_${TBLNAME}\".*, geom.geom"
			echo "FROM     \"${SCHEMA}_${TBLNAME}\" JOIN geom"
			echo "ON       \"${SCHEMA}_${TBLNAME}\".\"${relateID}\" = geom.\"${joinID}\""
			echo "ORDER BY \"${SCHEMA}_${TBLNAME}\".\"${relateID}\""
		;;
		(POLYGON)	
			echo "SELECT   \"${SCHEMA}_${TBLNAME}\".*, ST_Collect(geom.geom) AS \"geom\""
			echo "FROM     \"${SCHEMA}_${TBLNAME}\" JOIN geom"
			echo "ON       \"${SCHEMA}_${TBLNAME}\".\"${relateID}\" = geom.\"${joinID}\""
			echo "GROUP BY \"${SCHEMA}_${TBLNAME}\".\"${relateID}\""
			echo "ORDER BY \"${SCHEMA}_${TBLNAME}\".\"${relateID}\""
		;;
	esac
}

while [ "${1}" != "" ]
do
	case "${1}" in
	(-c|--case)
	    shift
		if [ "${1}" == "" ]; then PrintUsage; fi
	    case "${1}" in
	    (sensitive|lower)
	        CASE="${1}"
	        ;;
	    (*)
	        CASE="sensitive"
	        PrintUsage
	        ;;
	    esac
	    shift
	    ;;
	(-m|--mode)
	    shift
		if [ "${1}" == "" ]; then PrintUsage; fi
	    case "${1}" in
	    (append|replace)
	        MODE="${1}"
	        ;;
	    (*)
	        MODE="append"
	        PrintUsage
	        ;;
	    esac
	    shift
	    ;;
	(-g|--geopackage)
		shift
		if [ "${1}" == "" ]; then PrintUsage; fi
		GEOPACKAGE="${1}"
		shift
	;;
	(-s|--schema)
		shift; if [ "${1}" == "" ]; then PrintUsage; fi
		SCHEMA="${1}"
		shift
	;;
	(-t|--tblname)
		shift
		if [ "${1}" == "" ]; then PrintUsage; fi
		TBLNAME="${1}"
		shift
	;;
	(-)
		RGISFILE="${1}"
		FILENAME="Default"
		EXTENSION="gdbt"
		shift
	;;
	(-*)
		shift
		PrintUsage
	;;
	(*)
		RGISFILE="${1}"
		FILENAME="${RGISFILE##*/}"
		FILENAME="${FILENAME%%.*}"
		EXTENSION="${RGISFILE#*.}"
		shift
	;;
	esac
done

if [ "${RGISFILE}" == "" ]; then PrintUsage; fi
if [ "${SCHEMA}"   == "" ]; then SCHEMA="public"; fi
if [ "${TBLNAME}"  == "" ]; then TBLNAME="${FILENAME}"; fi

   GEOPACKAGE="${GEOPACKAGE}"
   SCHEMA=$(echo $(caseFunc "${CASE}" "${SCHEMA}")  | sed "s:-::g" | sed "s:+::g")
  TBLNAME=$(echo $(caseFunc "${CASE}" "${TBLNAME}") | sed "s:-::g" | sed "s:+::g")
       ID=$(caseFunc "${CASE}" "ID")
GRIDVALUE=$(caseFunc "${CASE}" "GridValue")

TEMPFILE="$(mktemp -u -t rgis2gpkgXXXX)"

[ -e "${GEOPACKAGE}" ] && [ "${MODE}" == "replace" ] && (echo "DROP TABLE IF EXISTS \"${SCHEMA}_${TBLNAME}\";" | spatialite -silent -batch  "${GEOPACKAGE}")
 
case "${EXTENSION}" in
	(gdbt|gdbt.gz)
		rgis2sql -c "${CASE}" -m "${MODE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" -d "sqlite" -r off "${RGISFILE}" |\
		sqlite3 "${GEOPACKAGE}"
		local sql="SELECT COUNT(\"table_name\") FROM \"gpkg_contents\" WHERE \"table_name\" = \"${SCHEMA}\"_\"${TBLNAME}\";"
		if (( $(echo "${sql}" | sqlite3 "${GEOPACKAGE}") == 0 ))
		then
			echo "INSERT INTO \"gpkg_contents\" (\"table_name\", \"data_type\",\"identifier\",\:kast_change\") VALUES ('${SCHEMA}_${TBLNAME}','attributes', '${SCHEMA}_${TBLNAME}',datetime('now'));" |\
			sqlit3 "${GEOPACKAGE}"
		fi
	;;
	(gdbp|gdbp.gz|gdbl|gdbl.gz)
		[ "${EXTENSION%.gz}" == gdbp ] && DATATYPE="POINT" || DATATYPE="LINESTRING"
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.asc"
		ogr2ogr -update -overwrite -a_srs EPSG:4326 -f "GPKG" -nln "geom" "${TEMPFILE}.gpkg" "${TEMPFILE}.asc"
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" -d "sqlite" -r off "${RGISFILE}" |\
		sqlite3 "${TEMPFILE}.gpkg"
		_GPKGsql "${SCHEMA}" "${TBLNAME}" "${DATATYPE}" "${ID}" "fid" > "${TEMPFILE}.sql"
		ogr2ogr -update -overwrite -nln "${SCHEMA}_${TBLNAME}" -nlt "${DATATYPE}" -dialect "sqlite" -sql "@${TEMPFILE}.sql" "${GEOPACKAGE}" "${TEMPFILE}.gpkg"
		rm "${TEMPFILE}".*
 	;;
	(gdbd|gdbd.gz)
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.grd"
		gdal_translate -a_srs EPSG:4326 "${TEMPFILE}.grd" "${TEMPFILE}.tif"
		gdal_polygonize.py -8 "${TEMPFILE}.tif" -f "GPKG" "${TEMPFILE}.gpkg" "geom" 
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" -d "sqlite" -r off "${RGISFILE}" |\
		sqlite3 "${TEMPFILE}.gpkg"
		_GPKGsql "${SCHEMA}" "${TBLNAME}" "POLYGON" "${GRIDVALUE}" "DN" > "${TEMPFILE}.sql"
		ogr2ogr -update -overwrite -nln "${SCHEMA}_${TBLNAME}" -nlt "POLYGON" -dialect "sqlite" -sql "@${TEMPFILE}.sql" "${GEOPACKAGE}" "${TEMPFILE}.gpkg"
		echo "DELETE FROM \"${SCHEMA}_${TBLNAME}\" WHERE \"${GRIDVALUE}\" = -9999" | sqlite3 "${GEOPACKAGE}" 
        	rm "${TEMPFILE}".*
	;;
	(gdbc|gdbc.gz|nc)
		[ -e "${GEOPACKAGE}" ] && [ "${MODE}" == "append" ] && (echo "DROP TABLE IF EXISTS \"${SCHEMA}_${TBLNAME}\"" | spatialite -silent -batch  "${GEOPACKAGE}")
        rgis2netcdf "${RGISFILE}" "${TEMPFILE}.nc"
        gdal_translate -a_srs "EPSG:4326" -of Rasterlite "${TEMPFILE}.nc" RASTERLITE:"${GEOPACKAGE}",table="${SCHEMA}_${TBLNAME}"
        rm "${TEMPFILE}.nc" "${TEMPFILE}.tif"
	;;
	(*)
		echo "Unrecognised extension: ${EXTENSION}"
		PrintUsage
	;;
esac