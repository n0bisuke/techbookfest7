@echo off
cd /d %~dp0

cd review-starter

if exist techbookfest7-pdf rd /s/q techbookfest7-pdf
if exist techbookfest7.pdf del techbookfest7.pdf

wsl docker run --rm -v $PWD:/work kauplan/review2.5 /bin/bash -c "cd /work; rake pdf"

move /y techbookfest7.pdf ..

pause
