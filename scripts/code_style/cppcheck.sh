#!/bin/bash
set -e
CODE_STYLE_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPTS_DIR="$(dirname "$CODE_STYLE_DIR")"
REPO_DIR="$(dirname "$SCRIPTS_DIR")"

cppcheck --enable=all \
         --inconclusive \
         --error-exitcode=1 \
         --std=c11 \
         --suppress=missingIncludeSystem \
         --suppress=unusedFunction \
         --suppress=toomanyconfigs \
         -I ${REPO_DIR}/build/external/libparams/libparams \
         -I ${REPO_DIR}/Libs \
         -I ${REPO_DIR}/include/application \
         -I ${REPO_DIR}/include/serialization \
         -I ${REPO_DIR}/platform_specific/socketcan/ \
         -I /usr/include/ \
         ${REPO_DIR}/src/
