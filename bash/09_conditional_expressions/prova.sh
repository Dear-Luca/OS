#!/bin/bash

exec 3> prova.txt;
for name in vaffa maledetto rospo; do
      echo "ho inserito $name " 1>&3
done



