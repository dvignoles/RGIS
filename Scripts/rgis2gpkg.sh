#!/usr/bin/env bash

function PrintUsage () {
	echo "Usage ${0##*/} [options] <rgisfile>"
	echo "      -c, --case [sensitive|lower|upper]"
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

function _GPKGattribTable () {
    local schemaName="${1}"; shift
    local  tableName="${1}"; shift
	local   dataType="${1}"; shift
	local   relateID="${1}"; shift
	local     joinID="${1}"; shift

	echo "INSERT INTO \"gpkg_contents\""
	echo "SELECT \"${schemaName}_${tableName}\", \"data_type\", \"${schemaName}_${tableName}\", \"description\", \"last_change\", \"min_x\", \"min_y\", \"max_x\", \"max_y\", \"srs_id\""
	echo "FROM "gpkg_contents" WHERE "table_name" = \"${schemaName}_${tableName}_geom\";"
	echo "INSERT INTO \"gpkg_ogr_contents\""
	echo "SELECT \"${schemaName}_${tableName}\", \"feature_count\" FROM "gpkg_ogr_contents" WHERE "table_name" = \"${schemaName}_${tableName}_geom\";"
	echo "SELECT gpkgAddGeometryColumn(\"${schemaName}_${tableName}\", \"geom\", '${dataType}', 0, 0, 4326);"
	echo "UPDATE \"${schemaName}_${tableName}\""
    echo "SET \"geom\" = \"${schemaName}_${tableName}_geom\".\"geom\""
    echo "FROM  \"${schemaName}_${tableName}_geom\" WHERE \"${schemaName}_${tableName}\".\"${relateID}\" = \"${schemaName}_${tableName}_geom\".\"${joinID}\";"
	echo "DROP TABLE \"${schemaName}_${TBLNAME}_geom\";"
	echo "DELETE FROM \"gpkg_metadata_reference\" WHERE \"table_name\" = \"${schemaName}_${tableName}_geom\";"
	echo "DELETE FROM \"gpkg_geometry_columns\"   WHERE \"table_name\" = \"${schemaName}_${tableName}_geom\";"
	echo "DELETE FROM \"gpkg_contents\"           WHERE \"table_name\" = \"${schemaName}_${tableName}_geom\";"
	echo "DELETE FROM \"gpkg_ogr_contents\"       WHERE \"table_name\" = \"${schemaName}_${tableName}_geom\";"
}

while [ "${1}" != "" ]
do
	case "${1}" in
	(-c|--case)
	    shift; if [ "${1}" == "" ]; then PrintUsage; fi
	    case "${1}" in
	    (sensitive|lower)
	        CASE="${1}"
	        ;;
	    (*)
	        CASE="sensitive"
	        PrintUsage
	        ;;
	    esac
	    shift;
	    ;;
	(-g|--geopackage)
		shift; if [ "${1}" == "" ]; then PrintUsage; fi
		GEOPACKAGE="${1}"
		shift
	;;
	(-s|--schema)
		shift; if [ "${1}" == "" ]; then PrintUsage; fi
		SCHEMA="${1}"; shift
	;;
	(-t|--tblname)
		shift; if [ "${1}" == "" ]; then PrintUsage; fi
		TBLNAME="${1}"; shift
	;;
	(-*)
		PrintUsage
	;;
	(*)
		RGISFILE="${1}"; shift
	esac
done

if [ "${RGISFILE}" == "" ]; then PrintUsage; fi
 FILENAME="${RGISFILE##*/}"
 FILENAME="${FILENAME%%.*}"
EXTENSION="${RGISFILE#*.}"

if [ "${SCHEMA}"  == "" ]; then  SCHEMA="public"; fi
if [ "${TBLNAME}" == "" ]; then TBLNAME="${FILENAME}"; fi

   GEOPACKAGE="${GEOPACKAGE}"
   SCHEMA=$(caseFunc "${CASE}" "${SCHEMA}")
  TBLNAME=$(caseFunc "${CASE}" "${TBLNAME}")
       ID=$(caseFunc "${CASE}" "ID")
GRIDVALUE=$(caseFunc "${CASE}" "GridValue")

TEMPFILE="$(mktemp -u -t rgis2gpkgXXXX)"

[ -e "${GEOPACKAGE}" ] && UPDATEFLAG="-update -overwrite" || UPDATEFLAG=""
 
case "${EXTENSION}" in
	(gdbt)
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" -d "sqlite" -r off "${RGISFILE}" | spatialite  -silent "${GEOPACKAGE}"
	;;
	(gdbp|gdbp.gz|gdbl|gdbl.gz)
		[ "${EXTENSION%.gz}" == gdbp ] && DATATYPE="POINT" || DATATYPE="LINESTRING"
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.asc"
		ogr2ogr -a_srs EPSG:4326 -f "ESRI Shapefile" "${TEMPFILE}.shp" "${TEMPFILE}.asc"
		ogr2ogr ${UPDATEFLAG} -a_srs EPSG:4326 -f "GPKG" -nln "${SCHEMA}_${TBLNAME}_geom" "${GEOPACKAGE}" "${TEMPFILE}.shp"
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" -d "sqlite" -r off "${RGISFILE}" | spatialite  -silent "${GEOPACKAGE}"
		_GPKGattribTable "${SCHEMA}" "${TBLNAME}" "${DATATYPE}" "${ID}" "fid" | spatialite -silent "${GEOPACKAGE}"
		rm "${TEMPFILE}".*
 	;;
	(gdbd|gdbd.gz)
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.grd"
		gdal_translate -a_srs EPSG:4326 "${TEMPFILE}.grd" "${TEMPFILE}.tif"
		gdal_polygonize.py -8 "${TEMPFILE}.tif" -f "ESRI Shapefile" "${TEMPFILE}.shp"
		ogr2ogr ${UPDATEFLAG} -a_srs EPSG:4326 -f "GPKG" -nln "${SCHEMA}_${TBLNAME}_geom" -nlt PROMOTE_TO_MULTI "${GEOPACKAGE}" "${TEMPFILE}.shp"
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" -d "sqlite" -r off "${RGISFILE}" | spatialite -silent "${GEOPACKAGE}"
		_GPKGattribTable "${SCHEMA}" "${TBLNAME}" "POLYGON" "${ID}" "DN" | spatialite -silent "${GEOPACKAGE}"
        rm "${TEMPFILE}".*
	;;
	(gdbc|gdbc.gz|nc)
        rgis2netcdf "${RGISFILE}" "${TEMPFILE}.nc"
        gdal_translate -a_srs "EPSG:4326" -of Rasterlite "${TEMPFILE}.nc" RASTERLITE:"${GEOPACKAGE}",table="${SCHEMA}_${TBLNAME}"
        rm "${TEMPFILE}.nc" "${TEMPFILE}.tif"
	;;
	(*)
		echo "Unrecognised extension: ${EXTENSION}"
		PrintUsage
	;;
esac