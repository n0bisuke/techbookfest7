@echo off
cd /d %~dp0

cd review-starter

:: init
if exist techbookfest7-pdf rd /s/q techbookfest7-pdf
if exist techbookfest7.pdf del techbookfest7.pdf

:: build
:: wsl docker run --rm -v $PWD:/work kauplan/review2.5 /bin/bash -c "cd /work; rake pdf"
for /f %%A in ('wsl wslpath -a '%CD%'') do set WSLCD=%%A
set BASHCD=%WSLCD:/mnt/=/%
docker run --rm -v %BASHCD%:/work kauplan/review2.5 /bin/bash -c "export STARTER_TARGET=ebook; cd /work; rake pdf"

:: move
move /y techbookfest7.pdf ..

:: open
start %~dp0techbookfest7.pdf

pause
