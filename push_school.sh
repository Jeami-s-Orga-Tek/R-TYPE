#!/bin/bash

make test
if [ $? -ne 0 ]; then
    echo "Tests échoués, push vers l'école annulé."
    exit 1
fi

git push school main
echo "Push vers l'école effectué avec succès."
