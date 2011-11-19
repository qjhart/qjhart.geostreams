@echo off
setlocal

call "%~dp0setenv.bat"
set CLAZZPATH=%PROJECT_HOME%\_generated\BUILD\classes
echo CLAZZPATH=%CLAZZPATH%

set JAVA_HOME=\Program Files\Java\jdk1.5.0_06
PATH=%JAVA_HOME%\bin;%PATH%

java -Dswing.defaultlaf=com.sun.java.swing.plaf.windows.WindowsLookAndFeel -classpath "%CLAZZPATH%" geostreams.Client %*

endlocal