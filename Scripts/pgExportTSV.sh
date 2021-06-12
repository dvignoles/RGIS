#!/usr/bin/env bash

USAGE="Usage ${0##*/} <dbname> <schema>"
if [[ "${2}" == "" ]]; then echo "${USAGE}"; exit 1; fi
         DBNAME="${1}"; shift
         SCHEMA="${1}"; shift

IFS='|'

echo "SET search_path TO \"${SCHEMA}\"; \dt" |\
psql "${DBNAME}" | sed "1,4d" | sed '$d' | sed '$d' |\
while read LINE
do
	COLUMNS=(${LINE})
	TBLNAME=$(echo "${COLUMNS[1]}" | sed -e 's/^[[:space:]]*//' | sed -e 's/[[:space:]]*$//')
	echo "COPY \"${SCHEMA}\".\"${TBLNAME}\" TO STDOUT CSV DELIMITER E'\t' HEADER;" |\
	psql "${DBNAME}" > "${TBLNAME}.tsv"
done
