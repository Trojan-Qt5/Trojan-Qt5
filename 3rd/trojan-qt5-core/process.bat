dumpbin /exports trojan-qt5-core.dll > exports.txt
echo LIBRARY TROJAN-QT5-core > trojan-qt5-core.def
echo EXPORTS >> trojan-qt5-core.def
for /f "skip=19 tokens=4" %%A in (exports.txt) do echo %%A >> trojan-qt5-core.def
lib /def:trojan-qt5-core.def /out:trojan-qt5-core.lib /machine:x86