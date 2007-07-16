#!/bin/bash  
#
# create API docs
#
# this uses PhpDocumentor from PEAR (pear install PhpDocumentor)
#
#

. ./common.sh

# API Reference
echo "creating API reference..."
rm -Rf $APIOUTDIR/

# new!
phpdoc -ti "Roadsend PHP SiteManager" -dn roadsend_siteManager -t $APIOUTDIR/ -f $SMDIR/siteManager.inc -o $PDOCOUTFORMAT
phpdoc -ti "Roadsend PHP SiteManager" -dn roadsend_siteManager -t $APIOUTDIR/ -d $SMDIR/lib/ -o $PDOCOUTFORMAT

# old!
#./phpdoc -t $APIOUTDIR/lib/ -f `find $SMDIR/lib/ -maxdepth 1 -type f | awk '{print $0 "," }' | xargs echo`
#./phpdoc -t $APIOUTDIR/contrib/ -d $SMDIR/contrib/
#./phpdoc -t $APIOUTDIR/smartFormFilters/ -d $SMDIR/lib/sfFilters
#./phpdoc -t $APIOUTDIR/smartFormEntities/ -d $SMDIR/lib/sfInputEntities
#./phpdoc -t $APIOUTDIR/configReaders/ -d $SMDIR/lib/configReaders
#./phpdoc -t $APIOUTDIR/sessionContainers/ -d $SMDIR/lib/sessionContainers
#./phpdoc -t $APIOUTDIR/smTags/ -d $SMDIR/lib/smTags
#./phpdoc -t $APIOUTDIR/memberSystems/ -d $SMDIR/lib/memberSystems


