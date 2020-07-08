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

#TEMPFILE="$(tempfile -p "rgis2geopackage" -s $(caseFunc "${CASE}" ${FILENAME}))"
TEMPFILE="TEMP_${FILENAME}"

case "${EXTENSION}" in
	(gdbp|gdbp.gz|gdbl|gdbl.gz)
    rgis2ascii "${RGISFILE}" "${TEMPFILE}.asc"
    rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}" -d "sqlite" -r off "${RGISFILE}" | sqlite3 "${GEOPACKAGE}"
    ogr2ogr -f "ESRI Shapefile" "${TEMPFILE}.shp" "${TEMPFILE}.asc"
    shp2pgsql -k -s 4326 "${TEMPFILE}.shp" "${SCHEMA}.${TBLNAME}_geom" | sqlite3 "${GEOPACKAGE}"
    echo "ALTER TABLE \"${SCHEMA}.${TBLNAME}\" ADD COLUMN \"geom\" geometry;
      		  UPDATE \"${SCHEMA}.${TBLNAME}\"
        	  SET \"geom\" = \"${SCHEMA}.${TBLNAME}_geom\".\"geom\"
         	  FROM   \"${SCHEMA}.${TBLNAME}_geom\"
        	  WHERE  \"${SCHEMA}.${TBLNAME}\".\"${ID}\" =  \"${SCHEMA}.${TBLNAME}_geom\".\"gid\";
         	  DROP TABLE \"${SCHEMA}.${TBLNAME}_geom\";" | psql "${GEOPACKAGE}"
    rm "${TEMPFILE}".*
	;;
	(gdbd|gdbd.gz)
	    [ -e "${GEOPACKAGE}" ] && UPDATEFLAG="-update -overwrite" || UPDATEFLAG=""
		rgis2ascii "${RGISFILE}" "${TEMPFILE}.grd"
		gdal_translate -a_srs EPSG:4326 "${TEMPFILE}.grd" "${TEMPFILE}.tif"
		gdal_polygonize.py -8 "${TEMPFILE}.tif" -f "ESRI Shapefile" "${TEMPFILE}.shp"
		ogr2ogr ${UPDATEFLAG} -makevalid -f "GPKG" -nln "${SCHEMA}.${TBLNAME}_geom" -nlt PROMOTE_TO_MULTI "${GEOPACKAGE}" "${TEMPFILE}.shp"
		rgis2sql -c "${CASE}" -a "DBItems" -s "${SCHEMA}" -q "${TBLNAME}_attrib" -d "sqlite" -r off "${RGISFILE}" | sqlite3 "${GEOPACKAGE}"
        (echo "CREATE TABLE \"${SCHEMA}.${TBLNAME}\" AS"
         echo "SELECT \"${SCHEMA}.${TBLNAME}_attrib\".*, \"SELECTION\".\"geom\""
         echo "FROM (SELECT \"DN\" AS \"${GRIDVALUE}\", ST_BUFFER (ST_UNION (\"geom\"),0.0) AS \"geom\""
         echo "      FROM \"${SCHEMA}.${TBLNAME}_geom\" GROUP BY \"DN\") AS SELECTION"
         echo "JOIN \"${SCHEMA}.${TBLNAME}_attrib\" ON \"${SCHEMA}.${TBLNAME}_attrib\".\"${GRIDVALUE}\" = \"SELECTION\".\"${GRIDVALUE}\";"
         echo "DELETE FROM \"${SCHEMA}.${TBLNAME}\" WHERE \"geom\" IS NULL;") | tee "${TEMPFILE}.sql" | spatialite "${GEOPACKAGE}"
#		 echo "DROP TABLE  \"${SCHEMA}.${TBLNAME}_geom\";"
#        rm "${TEMPFILE}".*
	;;
	(gdbc|gdbc.gz|nc)
        rgis2netcdf "${RGISFILE}" "${TEMPFILE}.nc"
        gdal_translate -a_srs "EPSG:4326" -of "GTiff" "${TEMPFILE}.nc" "${TEMPFILE}.tif"
        raster2pgsql "${TEMPFILE}.tif" "${SCHEMA}"."${TBLNAME}_Raster" |\
        sed "s:$(echo "${SCHEMA}"  | tr "[A-Z]" "[a-z]"):${SCHEMA}:g"  |\
        sed "s:$(echo "${TBLNAME}" | tr "[A-Z]" "[a-z]"):${TBLNAME}:g" |\
        psql "${GEOPACKAGE}"
        rm "${TEMPFILE}.nc" "${TEMPFILE}.tif"
	;;
	(*)
		echo "Unrecognised extension: ${EXTENSION}"
		PrintUsage
	;;
esac
