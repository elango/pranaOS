#!/usr/bin/env bash

set -eo pipefail

script_path=$(cd -P -- "$(dirname -- "$0")" && pwd -P)
cd "$script_path/.."

grep -Pirh '(?<!file://)(?<!\.)(?<!})(?<!\()/(etc|res|usr|www)/' base/ sysroot devtools/ documentation/ kernel/ services/ userland/ | \
sed -re 's,^.*["= `]/([^"%`: ]+[^"%`: /.])/?(["%`: .].*)?$,\1,' | \
sort -u | \
while read -r referenced_resource
do
    if ! [ -r "Base/${referenced_resource}" ] && ! [ -r "Build/Root/${referenced_resource}" ]
    then
        echo "missing resource: ${referenced_resource}"
    fi
done
