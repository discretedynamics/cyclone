#!/bin/sh

cp $1 project.pds
simFDS project
dot -Tpng -o project-statespace.png project-statespace.dot
