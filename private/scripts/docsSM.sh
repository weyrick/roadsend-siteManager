#!/bin/bash  
#
# create docs
#

. ./common.sh

#
./cleanDocs.sh

# API docs
./apiDocs.sh

# HTML docs
cd $STARTDIR
./htmlManual.sh

# make TEX for PDF
echo "creating TEX doc..."
jade -t tex -d $PRINTDOCBOOK -o $TEXTMP $SGMLFILE
 
# PDF docs
echo "creating PDF docs..."
cd $PDFOUTDIR
pdfjadetex $TEXTMP
pdfjadetex $TEXTMP
pdfjadetex $TEXTMP
rm $PDFOUTDIR/*.aux
rm $PDFOUTDIR/*.log

# PS docs
#echo "creating PostScript docs..."
cd $PSOUTDIR
jadetex $TEXTMP
jadetex $TEXTMP
jadetex $TEXTMP
dvips siteManager_manual_$SERIES.dvi -o siteManager_manual_$SERIES.ps
rm $PSOUTDIR/*.aux
rm $PSOUTDIR/*.dvi
rm $PSOUTDIR/*.log

cd $SMDIR/private/scripts

#./doc2db.php
#./api2db.php
