#!/bin/bash
#
#

. ./common.sh

./dumpDocs.sh

# DOCUMENTATION PACKAGES
cd $SMDIR/private/docs

rm $DOCPACKAGEDIR/*

# HTML tar
tar -zcvf "$DOCPACKAGEDIR/siteManager_manual_${SERIES}.tar.gz" siteManager_HTML/
# HTML zip
zip -r "$DOCPACKAGEDIR/siteManager_manual_${SERIES}.zip" siteManager_HTML/
# PDF
# copy, not compressed
cp siteManager_PDF/siteManager*.pdf $DOCPACKAGEDIR
# PS tar
tar -zcvf "$DOCPACKAGEDIR/siteManager_manual_${SERIES}_ps.tar.gz" siteManager_PS/
# HTML zip
zip -r "$DOCPACKAGEDIR/siteManager_manual_${SERIES}_ps.zip" siteManager_PS/

# API Reference HTML
tar -zcvf "$DOCPACKAGEDIR/siteManager_API_${SERIES}.tar.gz" siteManager_API/
# API Reference zip
zip -r "$DOCPACKAGEDIR/siteManager_API_${SERIES}.zip" siteManager_API/

