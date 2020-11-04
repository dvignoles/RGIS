#!/bin/bash

function PrintUsage () {
	echo "Usage ${0##*/} [options] <rgisfile>"
	echo "      -c, --case [sensitive|lower|upper]"
	echo "      -m  --mode [append|replace]"
	echo "      -d, --dbname  <dbname>"
	echo "      -s, --schema  <schema>"
	echo "      -t, --tblname <tblname>"
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
  DBNAME=""
  SCHEMA=""
 TBLNAME=""
    CASE="sensitive"
	MODE="append"

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
	(-m|--mode)
	    shift; if [ "${1}" == "" ]; then PrintUsage; fi
	    case "${1}" in
	    (append|replace)
	        MODE="${1}"
	        ;;
	    (*)
	        MODE="append"
	        PrintUsage
	        ;;
	    esac
	    shift;
	    ;;
	(-d|--dbname)
		shift; if [ "${1}" == "" ]; then PrintUsage; fi
		DBNAME="${1}"
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

   DBNAME="${DBNAME}"
   SCHEMA=$(caseFunc "${CASE}" "${SCHEMA}")
  TBLNAME=$(caseFunc "${CASE}" "${TBLNAME}")
       ID=$(caseFunc "${CASE}" "ID")
GRIDVALUE=$(caseFunc "${CASE}" "GridValue")

TEMPFILE="$(tempfile -p "rgis2PostGIS" -s $(caseFunc "${CASE}" ${FILENAME}))"

case "${EXTENSION}" in
	(gdbt|gdbt.gz)
		rgis2sql -c "${CASE}" -m "${MODE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" -r off "${RGISFILE}" | psql "${DBNAME}"
	;;
	(gdbp|gdbp.gz|gdbl|gdbl.gz)
 	   rgis2ascii "${RGISFILE}" "${TEMPFILE}.asc"
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" "${RGISFILE}" | psql "${DBNAME}"
		ogr2ogr -f "ESRI Shapefile" "${TEMPFILE}.shp" "${TEMPFILE}.asc"
    	shp2pgsql -k -s 4326 "${TEMPFILE}.shp" "${SCHEMA}"."${TBLNAME}_geom" | psql "${DBNAME}"
		[ "${MODE}" == "replace" ] && (echo "DROP TABLE IF EXISTS \"${SCHEMA}\".\"${TBLNAME}\"" | psql "${DBNAME}")
    	(echo "ALTER TABLE \"${SCHEMA}\".\"${TBLNAME}\" ADD COLUMN \"geom\" geometry;"
		 echo "UPDATE \"${SCHEMA}\".\"${TBLNAME}\""
		 echo "SET \"geom\" = \"${SCHEMA}\".\"${TBLNAME}_geom\".\"geom\""
    	 echo "FROM   \"${SCHEMA}\".\"${TBLNAME}_geom\""
		 echo "WHERE  \"${SCHEMA}\".\"${TBLNAME}\".\"${ID}\" =  \"${SCHEMA}\".\"${TBLNAME}_geom\".\"gid\";"
    	 echo "DROP TABLE \"${SCHEMA}\".\"${TBLNAME}_geom\";") | psql "${DBNAME}"
    rm "${TEMPFILE}".*
	;;
	(gdbd|gdbd.gz)
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.grd"
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" "${RGISFILE}" | psql "${DBNAME}"
		gdal_translate -a_srs EPSG:4326 "${TEMPFILE}.grd" "${TEMPFILE}.tif"
		gdal_polygonize.py -8 "${TEMPFILE}.tif" -f "ESRI Shapefile" "${TEMPFILE}.shp"
		ogr2ogr -makevalid "${TEMPFILE}valid.shp" "${TEMPFILE}.shp"
		shp2pgsql -k -s 4326 "${TEMPFILE}valid.shp" "${SCHEMA}"."${TBLNAME}_geom" | psql "${DBNAME}"
		[ "${MODE}" == "replace" ] && (echo "DROP TABLE IF EXISTS \"${SCHEMA}\".\"${TBLNAME}\"" | psql "${DBNAME}")
		(echo "ALTER TABLE \"${SCHEMA}\".\"${TBLNAME}\" ADD COLUMN \"geom\" geometry;"
      	 echo "UPDATE \"${SCHEMA}\".\"${TBLNAME}\""
         echo "SET \"geom\" = \"${TBLNAME}_SELECTION\".\"geom\""
         echo "FROM  (SELECT \"DN\", ST_BUFFER (ST_UNION (\"geom\"),0.0) AS \"geom\" FROM \"${SCHEMA}\".\"${TBLNAME}_geom\""
         echo "GROUP BY \"${SCHEMA}\".\"${TBLNAME}_geom\".\"DN\") AS \"${TBLNAME}_SELECTION\""
    	 echo "WHERE  \"${SCHEMA}\".\"${TBLNAME}\".\"${GRIDVALUE}\" = \"${TBLNAME}_SELECTION\".\"DN\";"
		 echo "DELETE FROM \"${SCHEMA}\".\"${TBLNAME}\" WHERE \"geom\" IS NULL;"
		 echo "DROP TABLE  \"${SCHEMA}\".\"${TBLNAME}_geom\";") |  psql "${DBNAME}"
        rm "${TEMPFILE}".* "${TEMPFILE}valid".*
	;;
	(gdbc|gdbc.gz|nc)
        rgis2netcdf "${RGISFILE}" "${TEMPFILE}.nc"
        gdal_translate -a_srs "EPSG:4326" -of "GTiff" "${TEMPFILE}.nc" "${TEMPFILE}.tif"
		[ "${MODE}" == "replace" ] && (echo "DROP TABLE \"${SCHEMA}\".\"${TBLNAME}\" IF EXISTS" | psql "${DBNAME}")
        raster2pgsql "${TEMPFILE}.tif" "${SCHEMA}"."${TBLNAME}_Raster" |\
        sed "s:$(echo "${SCHEMA}"  | tr "[A-Z]" "[a-z]"):${SCHEMA}:g"  |\
        sed "s:$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]"):${TBLNAME}:g" |\
        psql "${DBNAME}"
        rm "${TEMPFILE}.nc" "${TEMPFILE}.tif"
	;;
	(*)
		echo "Unrecognised extension: ${EXTENSION}"
		PrintUsage
	;;
esac
