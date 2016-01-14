#!/bin/sh

#  rsync files from cwd to/from nevil-res3
#    copies all files except those that match lines in exclude-list.txt

# Nevil Brownlee, Wed, 23 Oct 13 (NZDT)

CWD=`pwd`
echo "CWD = $CWD"
SDIR=`basename "$CWD"`
echo "CDIR = $CDIR"
DDIR=`dirname $CWD`
echo "DDIR = $DDIR"

if [ "$1" = "save" ]; then
   echo "About to SAVE files from $CWD to nevil-res3? (y/n)"
   read yon
   if [ "$yon" != "y" ]; then
      exit; fi

   #  Update (sync) files on nevil-res3
   rsync -avz --progress --stats --exclude-from 'exclude-list.txt' --rsh=/usr/bin/ssh $CWD jbro111@nevil-res3.itss.auckland.ac.nz:.  # Save as current directory
   exit

elif [ "$1" = "sync" ]; then  # Update files from res3
   echo "About to retrtieve files from nevil-res3? (y/n)"
   read yon
   if [ "$yon" != "y" ]; then
      exit; fi
   rsync -avz --progress --stats --rsh=/usr/bin/ssh jbro111@nevil-res3.itss.auckland.ac.nz:/home/jbro111/$SDIR $DDIR  # Retrieve into current directory
   exit

else
  TODAY=`(set \`date +%Y%m%d\`; echo $1)`
  # ` means "run the command and substitute the output"

  echo "Old save file will be $CDIR-save-$TODAY.tgz"

  mv $CDIR-save.tgz $CDIR-save-$TODAY.tgz
  tar zcf $CDIR-save.tgz \
     save.sh Makefile *.c *.h *.py *.rb *.txt \
     *.cls *.sty *.tex *.fig *.eps \
     *.bib *.bbl *.bst  *.txt *.rb *.sh \
     */*.svg */*.eps  */*.jpeg */*.png */*.gpl */*.txt
fi