#!/bin/bash
#
# package all files
#
#
# TODO: make a file list first, then use that for package commands?
#

. ./common.sh

# copy the tree
./copyTree.sh

# SOURCE PACKAGES
cd $BUILDDIR/..
echo "creating .tar.gz"
rm $PACKAGEDIR/$SMV.tar.gz
tar -zcvf "$PACKAGEDIR/$SMV.tar.gz" $SMV
echo "creating .tar.bz2"
rm $PACKAGEDIR/$SMV.tar.bz2
tar -jcvf "$PACKAGEDIR/$SMV.tar.bz2" $SMV
echo "creating .zip"
rm $PACKAGEDIR/$SMV.zip
zip -r "$PACKAGEDIR/$SMV.zip" $SMV

