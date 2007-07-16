#!/bin/bash
#
#
#

echo "cleaning docs..."

. ./common.sh

rm $APIOUTDIR/*
rm $HTMLOUTDIR/*
rm $PSOUTDIR/*
rm $PDFOUTDIR/*


echo "done."
