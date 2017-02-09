@echo off
for %%d in (record-usage limit-time) do (
    pushd %%d
    for /f %%f in ('dir /b *.cpp *.h') do clang-format.exe -i %%f
    popd
)
