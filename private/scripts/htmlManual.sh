#!/bin/bash
# 
#

. ./common.sh

# HTML docs
echo "creating HTML docs..."
rm -f $HTMLOUTDIR/*.html
cd $HTMLOUTDIR
jade -t sgml -ihtml -d $HTMLDOCBOOK $SGMLFILE
