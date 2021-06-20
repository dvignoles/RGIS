#!/bin/bash
  
##########################################################################################################
#
# bash functions for converting GIS formats.
#
##############################################################################################jjjkkj############

if [[ "${GHAASDIR}" == "" ]]; then GHAASDIR="/usr/local/share/ghaas"; fi
source "${GHAASDIR}/Scripts/RGISfunctions.sh"

function shp2rgisGrid () {
   local    archive="${1}"; shift
   local     domain="${1}"; shift
   local   variable="${1}"; shift
   local    product="${1}"; shift
   local resolution="${1}"; shift
   local extent_llx="${1}"; shift
   local extent_lly="${1}"; shift
   local extent_urx="${1}"; shift
   local extent_ury="${1}"; shift
   local  shapefile="${1}"; shift
   local    idField="${1}"; shift
   local     noData="${1}"; shift

   local   title="$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "static" "" "${version}")"
   local    grid="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "static")"

   [[ -e "${grid%/*}" ]] || mkdir -p "${grid%/*}"
   local seconds="$(RGISgeoResolutionInSecond "${resolution}")"
   local ncols=$(echo "(${extent_urx} - ${extent_llx}) * 3600 / ${seconds}" | bc)
   local nrows=$(echo "(${extent_ury} - ${extent_lly}) * 3600 / ${seconds}" | bc)

   echo "${ncols} ${nrows}"
   gdal_rasterize  -a "${idField}" -init "${noData}" -a_nodata "${noData}" -ot Integer -of GTiff -ts "${ncols}" "${nrows}" \
                  -te "${extent_llx}" "${extent_lly}" "${extent_urx}" "${extent_ury}" "${shapefile}" "${grid%.gdb*}.tif" || return 0
   gdal_translate -of AAIGrid  "${grid%.gdb*}.tif" "${grid%.gdb*}.grd" || return 0

   (echo  "1"        # Datatype
    echo "${noData}" # Nodata
    echo  "0"        # asciia input
    echo "${grid}"
    echo  "0") | grdImport -b "${grid%.gdb*}.grd"

   grdDateLayers -e year "${grid}" "${grid}"
   table2rgis    "${shapefile%.shp}.tsv" "${grid%.gdb*}.gdbt"
   tblJoinTables -t "${title}" -u "$(RGISlookupSubject "${variable}")" -d "${domain}" \
                 -a "${grid%.gdb*}.gdbt" -r "GridValue" -j "${idField}" "${grid}" "${grid}"
   tblDeleteField -f "DBItems" "${grid}" "${grid}"
   rm "${grid%.gdb*}".tif* "${grid%.gdb*}".grd* "${grid%.gdb*}".prj* "${grid%.gdb*}.gdbt"
}

function _GPKGsql () {
	local   schema="${1}"; shift
	local  tblName="${1}"; shift
	local relateID="${1}"; shift
	local   joinID="${1}"; shift

	echo "SELECT   \"${schema}_${tblName}\".*, geom.*"
	echo "FROM     \"${schema}_${tblName}\" JOIN geom"
	echo "ON       \"${schema}_${tblName}\".\"${relateID}\" = geom.\"${joinID}\""
	echo "ORDER BY \"${schema}_${tblName}\".\"${relateID}\""
}

function rgisFile2gpkg {
    local  rgisFile="${1}"; shift
    local  gpkgFile="${1}"; shift
    local    schema="${1}"; shift
    local   tblName="${1}"; shift
    local      mode="${1}"; shift

    local    schema=$(echo "${schema}"  | sed "s:-::g" | sed "s:+::g")
    local   tblName=$(echo "${tblName}" | sed "s:-::g" | sed "s:+::g")
    local   tmpFile="$(mktemp -u -t rgis2gpkgXXXX)"
	local extension="${rgisFile#*.}"

    case "${extension}" in
	    (gdbt|gdbt.gz)
		    rgis2sql -c "sensitive" -m "${mode}" -a "DBItems" -s "${schema}" -q "${tblName}" -d "sqlite" -r off "${rgisFile}" |\
		    sqlite3 "${gpkgFile}"
		    local sql="SELECT COUNT(\"table_name\") FROM \"gpkg_contents\" WHERE \"table_name\" = \"${schema}\"_\"${tblName}\";"
		    if (( $(echo "${sql}" | sqlite3 "${gpkgFile}") == 0 ))
		    then
			    echo "INSER INTO \"gpkg_contents\" (\"table_name\", \"data_type\",\"identifier\",\"last_change\") VALUES ('${schema}_${tblName}','attributes', '${schema}_${tblName}',datetime('now'));" |\
			    sqlit3 "${gpkgFile}"
		    fi
	    ;;
	    (gdbp|gdbp.gz|gdbl|gdbl.gz)
		    [ "${extension%.gz}" == gdbp ] && DATATYPE="POINT" || DATATYPE="LINESTRING"
		    rgis2ascii "${rgisFile}" "${tmpFile}.asc"
		    ogr2ogr -update -overwrite -a_srs EPSG:4326 -f "GPKG" -nln "geom" "${tmpFile}.gpkg" "${tmpFile}.asc"
		    rgis2sql -c "sensitive" -a "DBItems" -s "${schema}" -q "${tblName}" -d "sqlite" -r off "${rgisFile}" |\
		    sqlite3 "${tmpFile}.gpkg"
		    _GPKGsql "${schema}" "${tblName}" "ID" "fid" > "${tmpFile}.sql"
		    ogr2ogr -update -overwrite -nln "${schema}_${tblName}" -nlt "${DATATYPE}" -dialect "sqlite" -sql "@${tmpFile}.sql" "${gpkgFile}" "${tmpFile}.gpkg"
		    rm "${tmpFile}".*
 	    ;;
	    (gdbd|gdbd.gz)
		    rgis2ascii "${rgisFile}" "${tmpFile}.grd"
		    gdal_translate -a_srs EPSG:4326 "${tmpFile}.grd" "${tmpFile}.tif"
		    gdal_polygonize.py "${tmpFile}.tif" -f "GPKG" "${tmpFile}.gpkg" "full_geom"
		    ogr2ogr -update -overwrite -nlt POLYGON -dialect sqlite \
			        -sql "SELECT DN, ST_Union(geom) FROM full_geom GROUP BY DN" \
			        -f "GPKG" -nln "geom" "${tmpFile}.gpkg" "${tmpFile}.gpkg"
		    rgis2sql -c "sensitive" -a "DBItems" -s "${schema}" -q "${tblName}" -d "sqlite" -r off "${rgisFile}" |\
		    sqlite3 "${tmpFile}.gpkg"
		    _GPKGsql "${schema}" "${tblName}" "GridValue" "DN" > "${tmpFile}.sql"
		    ogr2ogr -update -overwrite -nln "${schema}_${tblName}" -nlt "POLYGON" -dialect "sqlite" -sql "@${tmpFile}.sql" "${gpkgFile}" "${tmpFile}.gpkg"
	    	rm "${tmpFile}".*
	    ;;
	    (gdbc|gdbc.gznc)
    	    [ -e "${gpkgFile}" ] && [ "${mode}" == "append" ] && (echo "DROP TABLE IF EXISTS \"${schema}_${tblName}\"" | sqlite3  "${gpkgFile}")
            rgis2netcdf "${rgisFile}" "${tmpFile}.nc"
            gdal_translate -a_srs "EPSG:4326" -of Rasterlite "${tmpFile}.nc" RASTERLITE:"${gpkgFile}",table="${schema}_${tblName}"
            rm "${tmpFile}.nc" "${tmpFile}.tif"
	    ;;
    	(*)
	    	echo "Unrecognised extension: ${extension}"
		    return 1
	    ;;
    esac
    return 0
}

function rgis2gpkg {
   local    archive="${1}"; shift;
   local     domain="${1}"; shift;
   local   variable="${1}"; shift;
   local    product="${1}"; shift;
   local resolution="${1}"; shift;
   local   gpkgFile="${1}"; shift;

   [[ -e "${gpkgFile%/*}" ]] || mkdir -p "${gpkgFile%/*}"
   local rgisFile=$(RGISfile "${archive}" "${domain}"  "${variable}" "${product}" "${resolution}" "static")
   rgisFile2gpkg "${rgisFile}" "${gpkgFile}"  "${product}" "$(RGISlookupSubject "${variable}")"
   return 0
}