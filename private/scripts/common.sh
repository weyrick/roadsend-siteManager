#!/bin/sh
#

# root 
SMDIR="/home/$USER/workspace/sitemanager/"
STARTDIR="$SMDIR/private/scripts/"

# version
RELEASE=`awk -F\' '/SM_VERSION/ { print \$4 }' $SMDIR/siteManager.inc`

SERIES=`echo $RELEASE | sed "s/\([0-9]*\.[0-9]*\)\.[0-9]*.*/\1.x/"`

# smv
SMV="siteManager-$RELEASE"

# build
BUILDDIR="/home/$USER/tmp/$SMV"

# docs 
PDOCOUTFORMAT="HTML:default:default"

APIOUTDIR="$SMDIR/private/docs/siteManager_API"
if [ ! -d $APIOUTDIR ]; then
	mkdir $APIOUTDIR
fi 

HTMLOUTDIR="$SMDIR/private/docs/siteManager_HTML"
if [ ! -d $HTMLOUTDIR ]; then
	mkdir $HTMLOUTDIR
fi 
PDFOUTDIR="$SMDIR/private/docs/siteManager_PDF"
if [ ! -d $PDFOUTDIR ]; then
	mkdir $PDFOUTDIR
fi 
PSOUTDIR="$SMDIR/private/docs/siteManager_PS"
if [ ! -d $PSOUTDIR ]; then
	mkdir $PSOUTDIR
fi 

SGMLFILE="$SMDIR/doc/sgml/siteManager.sgml"
HTMLDOCBOOK="$SMDIR/doc/sgml/html-out.dsl"
PRINTDOCBOOK="$SMDIR/doc/sgml/print-out.dsl"
TEXTMP="/tmp/siteManager_manual_$SERIES.tex"

# release directories
PACKAGEDIR="/home/$USER/workspace/releases"
DOCPACKAGEDIR="$PACKAGEDIR/docs"


export SMDIR RELEASE APIOUTDIR HTMLOUTDIR PDFOUTDIR PSOUTDIR SGMLFILE PRINTDOCBOOK TEXTMP STARTDIR PDOCDIR
export PACKAGEDIR DOCPACKAGEDIR HTMLDOCBOOK

echo "this is SiteManager version $RELEASE in series $SERIES"

cd $STARTDIR
