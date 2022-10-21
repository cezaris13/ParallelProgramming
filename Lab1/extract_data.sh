cat results.txt | grep "laikas" | sed "s/[a-z]//g;s/\s*//g;s/://g;s/^[A-Z]//g;s/\.\~\?$//g" | awk '{ printf "%s", $0; if (NR % 3 == 0) print ""; else printf " " }'

