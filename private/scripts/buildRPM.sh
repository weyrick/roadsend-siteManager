#!/bin/sh
#
#
#

RPMBASE="/usr/src/redhat"

. ./common.sh


# copy tar.gz
cp $PACKAGEDIR/$SMV.tar.gz  $RPMBASE/SOURCES

# copy spec file
cat $SMDIR/private/package/siteManager.spec | sed s/SM_VERSION/$RELEASE/ > $RPMBASE/SPECS/$SMV.spec

# remove siteManager symlink
rm /usr/local/lib/php/siteManager

# build RPM
rpm -ba $RPMBASE/SPECS/$SMV.spec

# cleanup
rm /usr/bin/siteManager
rm -Rf /usr/local/lib/php/siteManager

# add symlink back in
ln -s $SMDIR /usr/local/lib/php/siteManager

# copy RPM to releases
cp $RPMBASE/RPMS/noarch/siteManager-$RELEASE-*.rpm $PACKAGEDIR
chown weyrick.weyrick $PACKAGEDIR/*.rpm

