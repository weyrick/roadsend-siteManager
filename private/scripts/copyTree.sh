#!/bin/sh
#
#

. ./common.sh

# copy it
cp -R $SMDIR $BUILDDIR

# remove private
rm -Rf $BUILDDIR/private

# remove darcs
rm -Rf $BUILDDIR/_darcs 

# remove tests
rm -Rf $BUILDDIR/tests 

# remove ide files
rm $BUILDDIR/*.lrj
rm $BUILDDIR/*.tags

# remove sgml
rm -Rf $BUILDDIR/doc/sgml

