@ECHO OFF
SET build_dir=".\build"
SET current_dir="%CD%"
IF NOT EXIST %build_dir% (
	mkdir %build_dir%
)
pushd "%build_dir%"
cmake "%1" "%current_dir%" && msbuild Project.sln /p:Configuration=Debug /p:Platform="Win32"
popd