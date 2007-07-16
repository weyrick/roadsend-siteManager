#!/bin/bash
#
#
#

. ./common.sh

echo "running docDocs.sh ..."

pass=`cat /home/weyrick/bin/.sql`
outfile="$DOCPACKAGEDIR/manualUpdate.sql"

rm $outfile
rm "$outfile.gz"

echo "DROP TABLE manual;" > $outfile
echo "DROP TABLE apiClass;" >> $outfile
echo "DROP TABLE apiMethod;" >> $outfile
echo "DROP TABLE apiMethodParam;" >> $outfile
echo "DROP TABLE apiClassVar;" >> $outfile

mysqldump -u weyrick -p$pass siteManager manual >> $outfile
mysqldump -u weyrick -p$pass siteManager apiClass >> $outfile
mysqldump -u weyrick -p$pass siteManager apiMethod >> $outfile
mysqldump -u weyrick -p$pass siteManager apiMethodParam >> $outfile
mysqldump -u weyrick -p$pass siteManager apiClassVar >> $outfile

gzip $outfile
