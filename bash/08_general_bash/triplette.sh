#!/bin/bash
for name1 in `ls`; do
     for name2 in `ls`; do
          for name3 in `ls`; do
                echo "(${name1}, ${name2}, ${name3})";
          done
     done
done 


