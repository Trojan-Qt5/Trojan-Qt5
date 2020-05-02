dumpbin /exports trojan-qt5-libs.dll > exports.txt
echo LIBRARY TROJAN-QT5-LIBS > trojan-qt5-libs.def
echo EXPORTS >> trojan-qt5-libs.def
for /f "skip=19 tokens=4" %%A in (exports.txt) do echo %%A >> trojan-qt5-libs.def
lib /def:trojan-qt5-libs.def /out:trojan-qt5-libs.lib /machine:x86