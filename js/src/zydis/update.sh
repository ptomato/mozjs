#!/usr/bin/env bash

# Copyright 2019 Mozilla Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# Usage: update.sh [-v version]
#
# The version is the github rev we want; the default is HEAD
#
# Script to pull a new version of zydis, assuming it looks like an
# older version of zydis.  (This is by no means assured.)

if [ ! -d zydis -o ! -f zydis/moz.build ]; then
    echo "Run this script from the mozilla-central/js/src directory"
    exit 1
fi

REV="HEAD"
if [ "$1" = "-v" ]; then
    REV=$2
fi

ZYDISTMP=$(pwd)/zydis_tmp
ZYDISNEW=$(pwd)/zydis_new
ZYDISOLD=$(pwd)/zydis_old
rm -rf $ZYDISTMP $ZYDISNEW $ZYDISOLD

# Get zydis

echo "CLONING..."
mkdir $ZYDISTMP
(
cd $ZYDISTMP
git clone --recursive https://github.com/zyantific/zydis.git
cd zydis
git checkout $REV
)

# Copy Mozilla-created files

echo "COPYING..."
mkdir $ZYDISNEW
cp zydis/{README.md,ZydisAPI.{cpp,h},Zy*ExportConfig.h,update.sh} $ZYDISNEW

# Copy selected zydis files, and rename some

echo "COPYING ZYDIS..."
(
cd $ZYDISNEW
cp -R $ZYDISTMP/zydis/src Zydis
cp -R $ZYDISTMP/zydis/include/Zydis/* Zydis
cp $ZYDISTMP/zydis/LICENSE Zydis
cp -R $ZYDISTMP/zydis/dependencies/zycore/src/ Zycore
cp -R $ZYDISTMP/zydis/dependencies/zycore/include/Zycore/* Zycore
cp $ZYDISTMP/zydis/dependencies/zycore/LICENSE Zycore
rm -f Zycore/API/{Synchronization*,Terminal*,Thread*}
mv Zydis/String.c Zydis/ZydisString.c
mv Zycore/String.c Zycore/ZycoreString.c
( cd $ZYDISTMP/zydis ; git log -n 1 | grep '^commit' | awk '{ print $2 }' ) > imported-revision.txt
)

# Rewrite include paths to be Firefox-canonical and reflect new
# locations of files.

echo "REWRITING..."
(
cd $ZYDISNEW
for fn in $(find . -name '*.h' -o -name '*.c'); do
     cat $fn | gawk '
{ if (match($0, /( *# *include +)<(Zy.*ExportConfig.h|Zy(dis|core).*\.h)>/, res)) {
    print res[1] "\"zydis/" res[2] "\""
    next
  } else if (match($0, /( *# *include +)<(Generated\/.*\.inc)>/, res)) {
    print res[1] "\"zydis/Zydis/" res[2] "\""
    next
  }
  print $0 }' > $fn.bak
     mv $fn.bak $fn
done
)

# Generate moz.build from the sources

echo "GENERATING MOZ.BUILD..."
(
cd $ZYDISNEW
(
cat <<EOF
# This file was generated by update.sh

FINAL_LIBRARY = 'js'

# Includes should be relative to parent path
LOCAL_INCLUDES += [
    '!..',
    '..'
]

include('../js-config.mozbuild')
include('../js-cxxflags.mozbuild')

if CONFIG['JS_CODEGEN_X64'] or CONFIG['JS_CODEGEN_X86']:
    SOURCES += [
EOF
for fn in $(find . -name '*.c' -o -name '*.cpp' | ( LC_ALL=C sort --ignore-case ) | sed 's/\.\///'); do
    echo "        '$fn',"
done
cat <<EOF
    ]
EOF
) > moz.build
)

echo "CLEANING UP..."
mv zydis $ZYDISOLD
mv $ZYDISNEW zydis
rm -rf $ZYDISTMP
