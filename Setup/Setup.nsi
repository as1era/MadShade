!addplugindir "..\nsis_plugins\x86-unicode"
!include Attributes.nsh
!include ModernUI.nsh
!include DefaultSections.nsh
!include InstallationFiles.nsh

RequestExecutionLevel admin

# Uninstallation
Section Uninstall
    ReadRegStr $R0 HKCU "${SELFREGLOC}" "RobloxPath"
    ${If} ${FileExists} "$R0\${ROBLOXCLIENT}"
        push "$R0"
        Call un.UninstallReshadeFromClient
    ${Else}
        FindFirst $0 $1 "$R0\*"
        !define VERLOOPID ${__LINE__}
        loop_${VERLOOPID}:
            ; Skip files and the special '.' and '..' directories
            StrCmp $1 "" done_${VERLOOPID}
            StrCmp $1 "." next_${VERLOOPID}
            StrCmp $1 ".." next_${VERLOOPID}
            ${If} ${FileExists} "$R0\$1\${ROBLOXCLIENT}"
                push "$R0\$1"
                Call un.UninstallReshadeFromClient
            ${ElseIf} ${FileExists} "$R0\$1\${LEGACYROBLOXCLIENT}"
                push "$R0\$1"
                Call un.UninstallReshadeFromClient
            ${EndIf}
        next_${VERLOOPID}:
            FindNext $0 $1
            GoTo loop_${VERLOOPID}
        done_${VERLOOPID}:
        !undef VERLOOPID
        FindClose $0
    ${EndIf}

    DeleteRegKey HKCU "${SELFREGLOC}"
    RMDir /r "$R0\reshade-shaders"
    RMDir /r $INSTDIR
    RMDir /r "$R0\madshade"
    RMDir /r "$R0\pekoshade"
    RMDir /r "$SMPROGRAMS\${NAME}"
SectionEnd

Function un.UninstallReshadeFromClient
    Exch $R0

    Push $0
    Push $1

    MessageBox MB_YESNO|MB_ICONQUESTION "Uninstall Reshade in $R0?" IDYES continue
        goto done
    continue:

    Delete "$R0\${RENDERAPI}"
    Delete "$R0\Reshade.ini"
    !insertmacro ToLog $LOGFILE "Output" "Uninstalled Reshade from $R0."

    done:
    ; Restore registers in reverse order to leave the stack clean before returning.
    Pop $1
    Pop $0
    Pop $R0
    Return
FunctionEnd


Function un.onInit
    MessageBox MB_YESNO|MB_ICONQUESTION "This will uninstall Reshade from your Pekora client directory. Are you sure you want to continue?" IDYES continue
        quit
    continue:
FunctionEnd

# Installation
!insertmacro PresetFiles ${PRESETSOURCE} ${PRESETTEMPFOLDER}
!insertmacro RequiredFiles ${RESHADESOURCE} $RobloxPath

# Installer Init (this runs first)
Function .onInit
    Var /GLOBAL LOGFILE
    System::Call 'ole32::CoCreateGuid(g .s)'
    pop $LOGFILE
    StrCpy $LOGFILE "${LOGDIRECTORY}\$LOGFILE.log"
    CreateDirectory ${LOGDIRECTORY}
    LogEx::Init "true" $LOGFILE

    # Extract temporary files needed by the installer to a temporary directory
    SetOutPath $PLUGINSDIR
    File "Shaders.ini"
    ; Stage this before preset sections run so their selected PresetPath is retained.
    File "${RESHADESOURCE}\Reshade.ini"
    
    CreateDirectory ${PRESETTEMPFOLDER}
    !insertmacro ToLog $LOGFILE "Output" "$$INSTDIR: $INSTDIR"
    !insertmacro ToLog $LOGFILE "Output" "$$PLUGINSDIR: $PLUGINSDIR"

    ReadRegStr $R1 HKCU "${ROBLOXREGLOC}" "curPlayerVer"
    StrCmp $R1 "" empty_ver path_from_registry

empty_ver:
    ${If} ${FileExists} "$LOCALAPPDATA\Pekora\clients"
        FindFirst $0 $1 "$LOCALAPPDATA\Pekora\clients\*"
        loop:
            StrCmp $1 "" done_loop
            StrCmp $1 "." next_entry
            StrCmp $1 ".." next_entry
            ${If} ${FileExists} "$LOCALAPPDATA\Pekora\clients\$1\${ROBLOXCLIENT}"
                StrCpy $RobloxPath "$LOCALAPPDATA\Pekora\clients\$1"
                FindClose $0
                Goto path_found
            ${ElseIf} ${FileExists} "$LOCALAPPDATA\Pekora\clients\$1\${LEGACYROBLOXCLIENT}"
                StrCpy $RobloxPath "$LOCALAPPDATA\Pekora\clients\$1"
                FindClose $0
                Goto path_found
            ${EndIf}
        next_entry:
            FindNext $0 $1
            Goto loop
        done_loop:
            FindClose $0
    ${EndIf}
    Goto not_found

path_from_registry:
    ${If} ${FileExists} "$LOCALAPPDATA\Pekora\clients\$R1\${ROBLOXCLIENT}"
        StrCpy $RobloxPath "$LOCALAPPDATA\Pekora\clients\$R1"
        Goto path_found
    ${ElseIf} ${FileExists} "$LOCALAPPDATA\Pekora\clients\$R1\${LEGACYROBLOXCLIENT}"
        StrCpy $RobloxPath "$LOCALAPPDATA\Pekora\clients\$R1"
        Goto path_found
    ${ElseIf} ${FileExists} "$LOCALAPPDATA\ProjectX\Versions\$R1\${ROBLOXCLIENT}"
        StrCpy $RobloxPath "$LOCALAPPDATA\ProjectX\Versions\$R1"
        Goto path_found
    ${ElseIf} ${FileExists} "$LOCALAPPDATA\ProjectX\Versions\$R1\${LEGACYROBLOXCLIENT}"
        StrCpy $RobloxPath "$LOCALAPPDATA\ProjectX\Versions\$R1"
        Goto path_found
    ${EndIf}

not_found:
    call RobloxNotFoundError

path_found:

    ; ReadRegStr $R0 HKCU ${SELFREGLOC} "Version"
    ${GetSectionNames} ${SHADERSINI} DefineRepositories
    !insertmacro ToLog $LOGFILE "Output" "Repositories: $Repositories"

    # check if client still running
    nsProcess::_FindProcess "${ROBLOXCLIENT}"
    pop $R0
    !insertmacro ToLog $LOGFILE "nsProcess" "FindProcess ${ROBLOXCLIENT} with code: $R0"
    StrCmp $R0 0 0 +2
    Call RobloxRunningError

    SectionSetSize ${ReshadeSection} 36860
FunctionEnd

Function DefineRepositories
    ReadINIStr $R1 $0 $9 "repo"
    StrCmp $R1 "" skip 0
    StrCpy $R7 "$TEMP\$9-techniques.json"
    ${If} ${FileExists} $R7
    ${AndIf} $R0 == ${VERSION}
        Goto installed
    ${EndIf}

    ReadINIStr $R2 $0 $9 "search"
    ReadINIStr $R9 $0 $9 "branch"
    StrCmp $R9 "" 0 +2
    StrCpy $R9 "master"
    StrCpy $R9 "?ref=$R9"
    StrCmp $R2 "" +2
    StrCpy $R2 "/$R2"
    NScurl::http GET "https://api.github.com/repos/$R1/contents/Shaders$R2$R9" $R7 /END
    pop $R3
    !insertmacro ToLog $LOGFILE "NScurl" "https://api.github.com/repos/$R1/contents/Shaders$R2$R9: $R3"
    StrCmp $R3 "OK" +4 0
    Delete "$TEMP\$9-techniques.json"
    WriteINIStr $0 $9 "alwaysinstall" "true"
    GoTo loopend

    installed:
    nsJSON::Set /tree $R1 /file $R7
    nsJSON::Get /tree $R1 /count /end
    pop $R2
    StrCpy $R3 -1
    StrCpy $R4 ""
    StrCpy $R5 ""
    loop:
        StrCmp $R3 $R2 loopend
        nsJSON::Get /tree $R1 /index $R3 "name"
        pop $R4
        StrCmp $R4 "" loop
        StrCpy $R5 "$R5$R4,"
        IntOp $R3 $R3 + 1
        Goto loop
    loopend:
    ReadINIStr $R6 $0 $9 "alwaysinstall"
    StrCmp $R6 "true" +2
    WriteINIStr $0 $9 "techniques" $R5
    StrCpy $Repositories "$Repositories$9@$R1,"
    skip:
    Push $0
FunctionEnd