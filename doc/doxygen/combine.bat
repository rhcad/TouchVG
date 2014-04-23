set destpath=..\all.tmp\touchvg

del /Q/S  %destpath%

xcopy ..\..\thirdparty\TouchVGCore\core\include\*.* %destpath%\ /Y/S

xcopy ..\..\thirdparty\TouchVGCore\core\src\geom\*.* %destpath%\geom_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\graph\*.* %destpath%\graph_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\shape\*.* %destpath%\shape_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\cmdbase\*.* %destpath%\cmdbase_src\ /Y

xcopy ..\..\thirdparty\TouchVGCore\core\src\shapedoc\*.* %destpath%\shapedoc_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\cmdbasic\*.* %destpath%\cmdbasic_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\cmdmgr\*.* %destpath%\cmdmgr_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\jsonstorage\*.* %destpath%\jsonstorage_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\export\*.* %destpath%\export_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\record\*.* %destpath%\record_src\ /Y
xcopy ..\..\thirdparty\TouchVGCore\core\src\view\*.* %destpath%\view_src\ /Y

xcopy ..\..\thirdparty\TouchVGPlay\core\*.h %destpath%\play\ /Y
xcopy ..\..\thirdparty\TouchVGPlay\core\*.cpp %destpath%\play_src\ /Y
xcopy ..\..\thirdparty\TouchVGPlay\ios\include\*.* %destpath%\iosplay\ /Y
xcopy ..\..\thirdparty\TouchVGPlay\ios\src\*.* %destpath%\iosplay_src\ /Y

xcopy ..\..\core\democmds\gate\*.* %destpath%\democmds\ /Y
xcopy ..\..\core\democmds\cmds\*.* %destpath%\democmds\ /Y

xcopy ..\..\win32\include\canvas\*.* %destpath%\winview\ /Y
xcopy ..\..\win32\src\canvas\*.* %destpath%\winview_src\ /Y
xcopy ..\..\win32\include\view\*.* %destpath%\winview\ /Y
xcopy ..\..\win32\src\view\*.* %destpath%\winview_src\ /Y

xcopy ..\..\ios\view\include\*.* %destpath%\iosview\ /Y
xcopy ..\..\ios\view\src\*.* %destpath%\iosview_src\ /Y

xcopy ..\..\android\core\src\rhcad\touchvg\*.java %destpath%\andrview\ /Y
xcopy ..\..\android\core\src\rhcad\touchvg\view\*.* %destpath%\andrview\ /Y/S

xcopy ..\..\wpf\touchvglib\view\*.cs %destpath%\wpfview\ /Y
