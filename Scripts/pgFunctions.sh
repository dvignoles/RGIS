#!/bin/bash
  
##########################################################################################################
#
# PostgreSQL/PostGis scripts to interface with RGIS
#
##############################################################################################jjjkkj############

if [[ "${GHAASDIR}" == "" ]]; then GHAASDIR="/usr/local/share/ghaas"; fi
source "${GHAASDIR}/Scripts/RGISfunctions.sh"

function PGattribTableSQL () {
	local    caseVal="${1}";                            shift
	local     schema="$(RGIScase "${caseVal}" "${1}")"; shift
	local    tblname="$(RGIScase "${caseVal}" "${1}")"; shift
	local       geom="${1}";                            shift

	USAGE="Usage: PGattribTableSQL <case> <schema> <tblName> <id field> <geom field>"
    if [[ "${geom}" == "" ]]; then echo "${USAGE}"; return 1; fi

	echo "DROP TABLE IF EXISTS \"public\".\"temp_tb\";
          CREATE TABLE \"public\".\"temp_tb\" AS (SELECT * FROM \"${schema}\".\"${tblname}\");
          ALTER TABLE \"public\".\"temp_tb\" DROP \"geom\";
 	      COPY \"public\".\"temp_tb\" TO STDOUT
 	      DELIMITER AS '	' NULL AS '-9999' CSV HEADER QUOTE AS '\"';
 	      DROP TABLE \"public\".\"temp_tb\";"
}

function PGpolygonColorizeSQL () {
    local   caseVal="${1}";                            shift
    local    schema="$(RGIScase "${caseVal}" "${1}")"; shift
    local   tblName="$(RGIScase "${caseVal}" "${1}")"; shift
    local   geomFLD="$(RGIScase "${caseVal}" "${1}")"; shift
    local     idFLD="$(RGIScase "${caseVal}" "${1}")"; shift
    local  colorFLD="$(RGIScase "${caseVal}" "${1}")"; shift

	USAGE="Usage: PGpolygonColorizeSQL <case> <schema> <tblName> <geom field> <id field> <color field>"
    if [[ "${colorFLD}" == "" ]]; then echo "${USAGE}"; return 1; fi

    echo   "CREATE TEMPORARY TABLE \"TMPcolors\" (\"tmpCOLOR\" INTEGER NOT NULL CONSTRAINT \"tmpCOLOR_pkey\" PRIMARY KEY);
            INSERT INTO  \"TMPcolors\" (\"tmpCOLOR\") VALUES (1),(2),(3),(4),(5),(6),(7),(8),(9),(10),(11),(12);
            ALTER TABLE \"${schema}\".\"${tblName}\" DROP COLUMN IF EXISTS \"${colorFLD}\",
                                                     DROP COLUMN IF EXISTS \"newCOLOR\";
            ALTER TABLE \"${schema}\".\"${tblName}\" ADD COLUMN \"${colorFLD}\" INTEGER,
                                                     ADD COLUMN \"newCOLOR\" INTEGER;
            UPDATE      \"${schema}\".\"${tblName}\" SET \"${colorFLD}\" = 0, \"newCOLOR\" = 0;
            DO \$COLORIZE\$
            DECLARE
                polygons CURSOR FOR
                    SELECT \"LeftPoly\".\"${idFLD}\", \"LeftPoly\".\"${geomFLD}\", \"LeftPoly\".\"${colorFLD}\" AS \"${colorFLD}\",
                            CASE WHEN \"LeftPoly\".\"${colorFLD}\" >  4 THEN \"LeftPoly\".\"${colorFLD}\" - 4
                                 WHEN \"LeftPoly\".\"${colorFLD}\" <= 4 THEN 0 END AS \"hcolor\",
                            COUNT (\"RightPoly\".\"${colorFLD}\") AS \"tmpNCol\", COUNT (\"LeftPoly\".\"${idFLD}\") AS \"NumNeighbour\"
                    FROM     \"${schema}\".\"${tblName}\" AS \"LeftPoly\",
                             \"${schema}\".\"${tblName}\" AS \"RightPoly\"
                    WHERE \"LeftPoly\".\"${idFLD}\" != \"RightPoly\".\"${idFLD}\"
                    AND    St_Dimension (St_Intersection (\"LeftPoly\".\"${geomFLD}\",\"RightPoly\".\"${geomFLD}\")) > 0
                    GROUP BY \"LeftPoly\".\"${idFLD}\", \"LeftPoly\".\"${geomFLD}\", \"LeftPoly\".\"${colorFLD}\"
                    ORDER BY \"hcolor\" DESC, \"tmpNCol\" DESC, \"NumNeighbour\" DESC, \"${colorFLD}\" DESC;
                colorVAL integer;
            BEGIN
                FOR poly IN polygons LOOP
                    colorVAL = (SELECT MIN (\"tmpCOLORS\".\"tmpCOLOR\")
                                FROM (SELECT \"TMPcolors\".\"tmpCOLOR\", COUNT (\"Adjacent_Polygons\".\"${idFLD}\") AS \"NumPoly\"
                                      FROM \"TMPcolors\"
                                      LEFT JOIN (SELECT \"${schema}\".\"${tblName}\".\"${idFLD}\",
                                                        \"${schema}\".\"${tblName}\".\"${geomFLD}\",
                                                        \"${schema}\".\"${tblName}\".\"newCOLOR\"
                                                 FROM   \"${schema}\".\"${tblName}\"
                                                 WHERE  \"${schema}\".\"${tblName}\".\"${idFLD}\" != \"poly\".\"${idFLD}\"
                                                 AND  St_Dimension (St_Intersection (\"${schema}\".\"${tblName}\".\"${geomFLD}\", \"poly\".\"${geomFLD}\")) > 0) AS \"Adjacent_Polygons\"
                                ON  \"TMPcolors\".\"tmpCOLOR\" = \"Adjacent_Polygons\".\"newCOLOR\"
                                GROUP BY \"TMPcolors\".\"tmpCOLOR\") AS \"tmpCOLORS\"
                                WHERE \"tmpCOLORS\".\"tmpCOLOR\" != 0 AND \"NumPoly\" = 0);
                    UPDATE \"${schema}\".\"${tblName}\" SET \"newCOLOR\" = colorVAL WHERE \"${idFLD}\" = \"poly\".\"${idFLD}\";
                END LOOP;
                UPDATE \"${schema}\".\"${tblName}\" SET \"${colorFLD}\" = \"newCOLOR\";
                UPDATE \"${schema}\".\"${tblName}\" SET \"${colorFLD}\" = 1 WHERE \"${colorFLD}\" = 0;
                ALTER TABLE \"${schema}\".\"${tblName}\" DROP COLUMN \"newCOLOR\";
            END;
          \$COLORIZE\$;
          SELECT   \"${schema}\".\"${tblName}\".\"${colorFLD}\", COUNT (\"${schema}\".\"${tblName}\".\"${idFLD}\")
          FROM     \"${schema}\".\"${tblName}\"
          GROUP BY \"${schema}\".\"${tblName}\".\"${colorFLD}\"
          ORDER BY \"${schema}\".\"${tblName}\".\"${colorFLD}\";"
}

function PGimportShapeFile () {
	local   caseVal="${1}";                            shift
	local    dbName="$(RGIScase "${caseVal}" "${1}")"; shift
	local    schema="$(RGIScase "${caseVal}" "${1}")"; shift
	local   tblName="$(RGIScase "${caseVal}" "${1}")"; shift
	local shapeFile="${1}";                            shift
	local       gid="${1}";                            shift
	local      geom="${1}";                            shift
	local     color="${1}";                            shift

	USAGE="Usage: PGimportShapeFile <case> <dbName> <schema> <tblName> <shapefile> [id field] [geom field] [color field]"
    if [[ "${shapeFile}" == "" ]]; then echo "${USAGE}"; return 1; fi

    if [[ "${caseVal}" == "lower" ]]; then local caseOpt=""; else caseOpt="-k"; fi
	echo "DROP TABLE IF EXISTS \"${schema}\".\"${tblName}\";" | psql "${dbName}"
	shp2pgsql ${caseOpt} -s 4326 -W "latin1" "${shapeFile}" "${schema}.${tblName}" |\
	psql "${dbName}"

	if [[ "${gid}" != "" ]]
	then
		if [[  "${geom}" == "" ]]; then local  geom="geom"; fi
		if [[ "${color}" == "" ]]; then local color="$(RGIScase "${caseVal}" "Color")"; fi
		PGpolygonColorizeSQL "${caseVal}" "${schema}" "${tblName}" "${geom}" "${gid}" "${color}" |\
		psql "${dbName}"
	fi
}

function PGrasterDimension () {
	local resolution="${1}"; shift
	local     coord0="${1}"; shift
	local     coord1="${1}"; shift

	USAGE="Usage: PGrasterDimension <resolution> <coord0> <coord1>"
    if [[ "${coord1}" == "" ]]; then echo "${USAGE}"; return 1; fi

	echo $(echo "(${coord1} - ${coord0}) * 3600 / $(RGISgeoResolutionInSecond "${resolution}")" | bc)
}

function PGrasterize () {
	local     dbName="${1}"; shift
	local     schema="${1}"; shift
	local    tblName="${1}"; shift
	local    idField="${1}"; shift
	local    initVal="${1}"; shift
	local       geom="${1}"; shift
	local rgisArchiv="${1}"; shift
	local     domain="${1}"; shift
	local    subject="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local extent_llx="${1}"; shift
	local extent_lly="${1}"; shift
	local extent_urx="${1}"; shift
	local extent_ury="${1}"; shift

    USAGE="Usage: PGrasterize <dbName> <schema> <tblName> <idField> <initVal> <geom> <rgisArchive> <domain> <subject> <oroduct> <resolution> llx lly urx ury"
    if [[ "${extent_ury}" == "" ]]; then echo "${USAGE}"; return 1; fi

	local rgisFile="$(RGISfilePath "${rgisArchiv}" "${domain}" "${subject}" "${product}" "${resolution}" "static")"

	[[ -e "${rgisFile%/*}" ]] || mkdir -p "${rgisFile%/*}"

	local ncols=$(PGrasterDimension "${resolution}" "${extent_llx}" "${extent_urx}")
	local nrows=$(PGrasterDimension "${resolution}" "${extent_lly}" "${extent_ury}")

	# I don't understand why gdal_translate does not like gid field
	if [[ "${idField}" == "gid" ]]
	then
		echo "ALTER TABLE \"${schema}\".\"${tblName}\"
		      DROP COLUMN IF EXISTS \"idField\",
		      ADD  COLUMN \"idField\" INTEGER;
		      UPDATE      \"${schema}\".\"${tblName}\"
		      SET \"idField\" = ${idField};" |\
		psql -q "${dbName}"
		local idField="idField"
	fi
	gdal_rasterize -l "${schema}"."${tblName}" -a "${idField}" -init "${initVal}" -ot Integer  -of GTiff \
	               -ts "${ncols}" "${nrows}" -te "${extent_llx}" "${extent_lly}" "${extent_urx}" "${extent_ury}" \
	               "PG:${dbName}" "${rgisFile%.gdbd*}.tif"

	gdal_translate -of AAIGrid  "${rgisFile%.gdbd*}.tif" "${rgisFile%.gdbd*}.grd"
	(echo "2"
	 echo "-9999"
	 echo "0"
	 echo "${rgisFile}" 
	 echo "0") | grdImport -b "${rgisFile%.gdbd*}.grd"
	rm  "${rgisFile%.gdbd*}.tif" "${rgisFile%.gdbd*}.grd.aux.xml" "${rgisFile%.gdbd*}.prj" "${rgisFile%.gdbd*}.grd"

	PGattribTableSQL "sensitive" "${schema}" "${tblName}" "${idField}" "${geom}" |\
	psql -q "${dbName}" |\
	table2rgis - "${rgisFile%.gdbd*}.gdbt"

	tblJoinTables -t "${schema} ${tblName}" -u "Zones" -a "${rgisFile%.gdbd*}.gdbt" -e "DBItems" -o "DBItems" -r "GridValue" -j "${idField}" "${rgisFile}" - |\
	tblDeleteField -f "DBItems" - "${rgisFile}"
	rm "${rgisFile%.gdbd*}.gdbt"
	grdRenameLayers -r 1 "$(RGISlookupSubject "${subject}")" "${rgisFile}" "${rgisFile}"
	RGISsetHeader "${rgisArchiv}" "${domain}" "${subject}" "${product}" "${resolution}" "static"
}

function PGaddCodeField () {
    local    dbName="${1}"; shift
    local    schema="${1}"; shift
    local     table="${1}"; shift
    local charField="${1}"; shift
    local codeField="${1}"; shift

    USAGE="Usage: PGaddCodeField <dbname> <schema> <table> <name field> <code field>"
    if [[ "${codeField}" == "" ]]; then echo "${USAGE}"; return 1; fi

echo "DROP TABLE IF EXISTS \"RGISTemp_TABLE\";
CREATE TEMPORARY TABLE \"RGISTemp_TABLE\"
(\"${codeField}\" SERIAL, \"${charField}\" CHARACTER VARYING(80) COLLATE pg_catalog.default,
 CONSTRAINT \"RGISTemp_TABLE_pkey\" PRIMARY KEY (\"${codeField}\")) WITH (OIDS = FALSE);

INSERT INTO \"RGISTemp_TABLE\" (\"${charField}\")
SELECT \"${charField}\" FROM \"${schema}\".\"${table}\" GROUP BY \"${charField}\" ORDER BY \"${charField}\";

ALTER TABLE \"${schema}\".\"${table}\" ADD \"${codeField}\" INTEGER;

UPDATE \"${schema}\".\"${table}\"
    SET \"${codeField}\" = \"RGISTemp_TABLE\".\"${codeField}\"
FROM \"RGISTemp_TABLE\"
WHERE \"${schema}\".\"${table}\".\"${charField}\" like \"RGISTemp_TABLE\".\"${charField}\";

DROP TABLE \"RGISTemp_TABLE\";" | psql -q "${dbName}"
}

function PGgpkgExportObject () {
	local   dbName="${1}"; shift
	local   schema="${1}"; shift
	local  objName="${1}"; shift
	local gpkgFile="${1}"; shift

	USAGE="Usage: PGgpkgExportObject <dbname> <schema> <object> <gpkgfile>"
	if [[ "${gpkgFile}" == "" ]]; then echo "${USAGE}"; return 1; fi

	local updateFlag=""
	[ -e "${gpkgFile}" ] && local updateFlag="-update"

	ogr2ogr -f GPKG -nln "${schema}.${objName}" "${gpkgFile}" ${updateFlag} -sql "SELECT * FROM \"${schema}\".\"${objName}\"" PG:dbname="${dbName}"
}

function PGgpkgExportSchema () {
	local     dbName="${1}"; shift
	local     schema="${1}"; shift
	local    objects="${1}"; shift
	local    pattern="${1}"; shift
	local   gpkgFile="${1}"; shift

	USAGE="Usage: PGgpkgExportSchema <dbname> <schema> <objects> <pattern> <gpkgfile>"
	if [[ "${gpkgFile}" == "" ]]; then echo "${USAGE}"; return 1; fi

	case "${objects}" in
	(tables)
		echo "Exporting ${schema} tables"
		local sqlCommand="\dt;"
	;;
	(views)
		echo "Exporting ${schema} views"
		local sqlCommand="\dv;"
	;;
	esac

	local ifs="${IFS}"
	IFS='|'

	echo "SET search_path TO \"${schema}\"; ${sqlCommand}" |\
	psql "${dbName}" | sed "1,4d" | sed '$d' | sed '$d' | grep "${pattern}" |\
	while read line
	do
		local columns=(${line})
		local tblName=$(echo "${columns[1]}" | sed -e 's/^[[:space:]]*//' | sed -e 's/[[:space:]]*$//')
		PGgpkgExportObject "${dbName}" "${schema}" "${tblName}" "${gpkgFile}"
	done
	IFS="${ifs}"
}