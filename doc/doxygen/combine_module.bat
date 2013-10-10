set destpath=..\all.tmp\touchvg

del /Q/S  %destpath%

xcopy ..\..\core\pub_inc\*.* %destpath%\ /Y/S
xcopy ..\..\core\mgr_inc\*.* %destpath%\ /Y/S

xcopy ..\..\core\pub_src\geom\*.* %destpath%\geom\ /Y
xcopy ..\..\core\pub_src\graph\*.* %destpath%\graph\ /Y
xcopy ..\..\core\pub_src\shape\*.* %destpath%\shape\ /Y
xcopy ..\..\core\pub_src\cmdbase\*.* %destpath%\cmdbase\ /Y

xcopy ..\..\core\mgr_src\shapedoc\*.* %destpath%\shapedoc\ /Y
xcopy ..\..\core\mgr_src\cmdbasic\*.* %destpath%\cmdbasic\ /Y
xcopy ..\..\core\mgr_src\cmdmgr\*.* %destpath%\cmdmgr\ /Y
xcopy ..\..\core\mgr_src\json\*.* %destpath%\json\ /Y
xcopy ..\..\core\mgr_src\view\*.* %destpath%\view\ /Y

xcopy ..\..\core\democmds\gate\*.* %destpath%\democmds\ /Y
xcopy ..\..\core\democmds\cmds\*.* %destpath%\democmds\ /Y

rem del /Q/S %destpath%\cmdobserver
rem del /Q/S %destpath%\canvas
rem del /Q/S %destpath%\storage

rem xcopy ..\..\win\include\canvas\*.* %destpath%\winview\ /Y
rem xcopy ..\..\win\src\canvas\*.* %destpath%\winview\ /Y
rem xcopy ..\..\win\include\view\*.* %destpath%\winview\ /Y
rem xcopy ..\..\win\mgr_src\view\*.* %destpath%\winview\ /Y

xcopy ..\..\ios\view\include\*.* %destpath%\iosview\ /Y
xcopy ..\..\ios\view\src\*.* %destpath%\iosview\ /Y

xcopy ..\..\android\core\src\touchvg\view\*.java %destpath%\android\ /Y
xcopy ..\..\wpf\touchvglib\view\*.cs %destpath%\wpf\ /Y
copy dummy.cpp %destpath%\android\
copy dummy.cpp %destpath%\wpf\
