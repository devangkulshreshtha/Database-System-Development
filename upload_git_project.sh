#!/bin/bash
REPO=$1
git init
git add .
git commit -m "First commit"
git remote add origin https://github.com/devangkulshreshtha/$REPO
git remote -v
git push origin master
