IF EXIST "%ProgramFiles(x86)%\MSBuild\14.0\Bin\MSBuild.exe" (
"%ProgramFiles(x86)%\MSBuild\14.0\Bin\MSBuild.exe" Project64.sln /p:Platform=Win32 /p:Configuration=Debug /p:PlatformToolset=v140_xp
) ELSE IF EXIST "%ProgramFiles(x86)%\MSBuild\15.0\Bin\MSBuild.exe" (
"%ProgramFiles(x86)%\MSBuild\15.0\Bin\MSBuild.exe" Project64.sln /p:Platform=Win32 /p:Configuration=Debug /p:PlatformToolset=v140_xp
)
cmd /k