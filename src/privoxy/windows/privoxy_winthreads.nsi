;
; File:
;   $Source: /cvsroot/ijbswa/winsetup/privoxy_winthreads.nsi,v $
;
; Purpose:
;   NSIS script to make the Privoxy installer
;
;   This .NSI script is designed for NSIS v2.24+
;
;   Get NSIS from:  http://www.nullsoft.com/free/nsis/
;
; Copyright:
;   Written by and Copyright (C) 2007-2009 the Privoxy team.
;   http://www.privoxy.org/
;
;   This script originally written by and Copyright (C) 2002
;   Jonathan Foster
;
;   This program is free software; you can redistribute it 
;   and/or modify it under the terms of the GNU General
;   Public License as published by the Free Software
;   Foundation; either version 2 of the License, or (at
;   your option) any later version.
;
;   This program is distributed in the hope that it will
;   be useful, but WITHOUT ANY WARRANTY; without even the
;   implied warranty of MERCHANTABILITY or FITNESS FOR A
;   PARTICULAR PURPOSE.  See the GNU General Public
;   License for more details.
;
;   The GNU General Public License should be included with
;   this file.  If not, you can view it at
;   http://www.gnu.org/copyleft/gpl.html
;   or write to the Free Software Foundation, Inc., 59
;   Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;

!include WinMessages.nsh ; to send close message to Privoxy process

!define NL "$\r$\n"

Var /GLOBAL UpdateExisting

;win7 RequestExecutionLevel admin


; Close Privoxy and wait for termination
;
!macro ClosePrivoxy
	;Close privoxy.exe
	FindWindow $R0 "PrivoxyLogOwner"
	IntCmpU $R0 0 NoPrivoxyRunning 0 0
	SendMessage $R0 ${WM_CLOSE} 0 0
	; wait for close
	StrCpy $R1 0
CheckWindowClosed:
	Sleep 200 ; avoid file in use error
	FindWindow $R0 "PrivoxyLogOwner"
	IntCmpU $R0 0 NoPrivoxyRunning 0 0
	IntOp $R1 $R1 + 1
	IntCmp $R1 20 0 CheckWindowClosed 0
	Sleep 200 ; avoid file in use error
NoPrivoxyRunning:
!macroend


; create a backup file if file exists, set $UpdateExisting on existing files
;
!macro BackupOLD Name BackupName
	!define UniqueID ${__LINE__}

	IfFileExists "$INSTDIR\${Name}"  0 skip_${UniqueID}
	StrCpy $UpdateExisting 1
	delete "$INSTDIR\${BackupName}"
	rename "$INSTDIR\${Name}" "$INSTDIR\${BackupName}"
skip_${UniqueID}:

	!undef UniqueID
!macroend


; check file, write new one to different target there is one, set $UpdateExisting on existing files
;
!macro KeepCur Name DefaultName SrcDir
	!define UniqueID ${__LINE__}

	IfFileExists "$INSTDIR\${Name}" 0 new_${UniqueID}
	StrCpy $UpdateExisting 1
	delete "$INSTDIR\${DefaultName}"
	File "/oname=${DefaultName}" "${SrcDir}${Name}"
	goto done_${UniqueID}

new_${UniqueID}:
	File "${SrcDir}${Name}"

done_${UniqueID}:

	!undef UniqueID
!macroend


Name "Privoxy"
OutFile "privoxy_setup.exe"

BGGradient off

; Some default compiler settings (uncomment and change at will):
SetCompress auto ; (can be off or force)
SetCompressor /FINAL /SOLID lzma
SetDatablockOptimize on
CRCCheck on
AutoCloseWindow true ; (can be true for the window go away automatically at end)
ShowInstDetails nevershow ; (can be show to have them shown, or nevershow to disable)
SetDateSave on ; (can be on to have files restored to their orginal date)
; SetOverwrite ifnewer ; (files are only overwritten if the existing file is older than the new file)
SetOverwrite on  ; install package files over-write existing files regardless of date

Icon "privoxy.ico"
UninstallIcon "uninstall_privoxy.ico"

#LicenseText "Privoxy is distributed under the GNU General Public License.  Please read it before you install."
#LicenseData "build/LICENSE.txt"

InstallDir "$PROGRAMFILES\Privoxy"
;InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Privoxy.org\Privoxy" ""
;DirShow show ; (make this hide to not let the user change it)
;DirShow doesn't currently work.
DirText "Select the directory to install Privoxy in:"

ComponentText "Please select how you want to start Privoxy:"

Section "" ; (default section)
	StrCpy $UpdateExisting 0

	; Close privoxy.exe if it is running (user is upgrading) to prevent in-use errors.
	!insertmacro ClosePrivoxy

	; add files / whatever that need to be installed here.
	SetOutPath "$INSTDIR"

	; save files the user might have changed
	;   config.txt  match-all.action  trust.txt
	;
	!insertmacro BackupOLD "config.txt" "old_config.txt"
	!insertmacro BackupOLD "match-all.action" "old_match-all.action";
	!insertmacro BackupOLD "trust.txt" "old_trust.txt"

	;File /r build\*.*

	; leave user.action and user.filter alone if they exist
	;
	!insertmacro KeepCur "user.action" "clean_user.action" "build\"
	!insertmacro KeepCur "user.filter" "clean_user.filter" "build\"
	; exclude all files handled by KeepCur
	File /r /x CVS /x user.action /x user.filter build\*.*

	;WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Privoxy.org\Privoxy" "" "$INSTDIR"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "DisplayName" "Privoxy (remove only)"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "UninstallString" '"$INSTDIR\privoxy_uninstall.exe"'

	WriteRegStr HKEY_CLASSES_ROOT "PrivoxyActionFile\shell\open\command" "" 'Notepad.exe "%1"'
	WriteRegStr HKEY_CLASSES_ROOT ".action" "" "PrivoxyActionFile"
	WriteRegStr HKEY_CLASSES_ROOT "PrivoxyFilterFile\shell\open\command" "" 'Notepad.exe "%1"'
	WriteRegStr HKEY_CLASSES_ROOT ".filter" "" "PrivoxyFilterFile"

	WriteUninstaller "privoxy_uninstall.exe"
SectionEnd

Section "Add to Start Menu"
	SetShellVarContext all ; (Add to "All Users" Start Menu if possible)
	RMDir /r "$SMPROGRAMS\Privoxy"
	CreateDirectory "$SMPROGRAMS\Privoxy"
	CreateShortCut "$SMPROGRAMS\Privoxy\Privoxy.lnk" "$INSTDIR\privoxy.exe"
	WriteINIStr "$SMPROGRAMS\Privoxy\Web-based Configuration.url" "InternetShortcut" "URL" "http://config.privoxy.org/"
	CreateShortCut "$SMPROGRAMS\Privoxy\Web-based Feedback.lnk" "$INSTDIR\doc\user-manual\contact.html"
	CreateDirectory "$SMPROGRAMS\Privoxy\Edit Config"
	CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Main Configuration.lnk" "Notepad.exe" '"$INSTDIR\config.txt"'
	CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Default Actions.lnk" "Notepad.exe" '"$INSTDIR\default.action"'
	CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\User Actions.lnk" "Notepad.exe" '"$INSTDIR\user.action"'
	CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Filters.lnk" "Notepad.exe" '"$INSTDIR\default.filter"'
	CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Trust list.lnk" "Notepad.exe" '"$INSTDIR\trust.txt"'
	CreateDirectory "$SMPROGRAMS\Privoxy\Documentation"
	CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\User Manual.lnk" "$INSTDIR\doc\user-manual\index.html"
	CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\Frequently Asked Questions.lnk" "$INSTDIR\doc\faq\index.html"
	CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\Credits.lnk" "Notepad.exe" '"$INSTDIR\AUTHORS.txt"'
	CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\License.lnk" "Notepad.exe" '"$INSTDIR\LICENSE.txt"'
	CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\ReadMe file.lnk" "Notepad.exe" '"$INSTDIR\README.txt"'
	WriteINIStr "$SMPROGRAMS\Privoxy\Documentation\Web Site.url" "InternetShortcut" "URL" "http://privoxy.org/"
	CreateShortCut "$SMPROGRAMS\Privoxy\Uninstall Privoxy.lnk" "$INSTDIR\privoxy_uninstall.exe"
SectionEnd


Section "Run automatically at startup"
	CreateShortCut "$SMSTARTUP\Privoxy.lnk" "$INSTDIR\privoxy.exe" "" "" 0 SW_SHOWMINIMIZED
SectionEnd


Function .onInstSuccess
	; on successful install, show message then start it
	IntCmp $UpdateExisting 0  0 updated updated
	MessageBox MB_YESNO|MB_DEFBUTTON1|MB_ICONQUESTION "Privoxy has been installed.${NL}${NL}Start Privoxy now?" /SD IDNO IDYES execprivoxy IDNO done
	goto done
updated:
	MessageBox MB_YESNO|MB_DEFBUTTON1|MB_ICONEXCLAMATION "Privoxy has been updated.${NL}Don't forget to convert configuration from the 'old*' files!${NL}${NL}Start Privoxy now?" /SD IDNO IDYES execprivoxy IDNO done
	goto done
execprivoxy:
	; run privoxy after installation
	SetOutPath "$INSTDIR"

	Exec "$INSTDIR\privoxy.exe"
done:
FunctionEnd


; begin uninstall settings/section. The UninstallText line must be before the Section header.
;
UninstallText "This will uninstall Privoxy from your system"
Section Uninstall
	SetShellVarContext all ; (Remove from "All Users" Start Menu if possible)
	
	;DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Privoxy.org\Privoxy"
	DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy"

	DeleteRegKey HKEY_CLASSES_ROOT ".action"
	DeleteRegKey HKEY_CLASSES_ROOT "PrivoxyActionFile"
	DeleteRegKey HKEY_CLASSES_ROOT ".filter"
	DeleteRegKey HKEY_CLASSES_ROOT "PrivoxyFilterFile"

	Delete "$SMSTARTUP\Privoxy.lnk"

	!insertmacro ClosePrivoxy

	RMDir /r "$SMPROGRAMS\Privoxy"
	RMDir /r "$INSTDIR"
SectionEnd

; eof
