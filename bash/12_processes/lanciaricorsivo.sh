#!/bin/bash

if (( $1 > 0 )); then
      ./lanciaricorsivo.sh $(($1 - 1)) &
      echo $!
      wait $!
fi

