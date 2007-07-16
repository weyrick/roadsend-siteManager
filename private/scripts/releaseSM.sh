#!/bin/bash
#
# do a release
#

#./docsSM.sh
./packageSM.sh
#./packageDocs.sh

echo "done, now:"
echo "1) create snapshot in accurev"
#echo "2) su to root, then run ./buildRPM.sh"
echo "3) bump version number in siteManager.inc!"

