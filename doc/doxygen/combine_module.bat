set destpath=..\all.tmp\touchvg

del /Q/S  %destpath%

xcopy ..\..\thirdparty\TouchVGCore\core\include\*.* %destpath%\ /Y/S
xcopy ..\..\thirdparty\TouchVGCore\core\include\*.* %destpath%\ /Y/S

xcopy ..\..\core\pub_src\geom\*.* %destpath%\geom\ /Y
xcopy ..\..\core\pub_src\graph\*.* %destpath%\graph\ /Y
xcopy ..\..\core\pub_src\shape\*.* %destpath%\shape\ /Y
xcopy ..\..\core\pub_src\cmdbase\*.* %destpath%\cmdbase\ /Y

xcopy ..\..\thirdparty\TouchVGCore\core\src\shapedoc\*.* %destpath%\shapedoc\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\cmdbasic\*.* %destpath%\cmdbasic\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\cmdmgr\*.* %destpath%\cmdmgr\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\json\*.* %destpath%\json\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\export\*.* %destpath%\export\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\record\*.* %destpath%\record\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\view\*.* %destpath%\view\ /Y

xcopy ..\..\core\democmds\gate\*.* %destpath%\democmds\ /Y
xcopy ..\..\core\democmds\cmds\*.* %destpath%\democmds\ /Y

rem del /Q/S %destpath%\cmdobserver
rem del /Q/S %destpath%\canvas
rem del /Q/S %destpath%\storage

xcopy ..\..\win32\include\canvas\*.* %destpath%\winview\ /Y
xcopy ..\..\win32\src\canvas\*.* %destpath%\winview\ /Y
xcopy ..\..\win32\include\view\*.* %destpath%\winview\ /Y
xcopy ..\..\win32\mgr_src\view\*.* %destpath%\winview\ /Y

xcopy ..\..\ios\view\include\*.* %destpath%\iosview\ /Y
xcopy ..\..\ios\view\src\*.* %destpath%\iosview\ /Y

xcopy ..\..\android\core\src\touchvg\view\*.java %destpath%\android\ /Y
xcopy ..\..\wpf\touchvglib\view\*.cs %destpath%\wpf\ /Y
copy dummy.cpp %destpath%\android\
copy dummy.cpp %destpath%\wpf\
