!include LogicLib.nsh
!include FileFunc.nsh
!include DetailPrints.nsh
!insertmacro Locate 
!include "Util\MoveFileFolder.nsh"

var LauncherTransferID

!macro StopMessage Message
    MessageBox MB_OK|MB_ICONSTOP "${Message}"
    DetailPrint "Setup quit: ${Message}"
    quit
!macroend

!macro ToLog File Type String
    LogEx::Write "[${Type}]$\t${String}"
!macroend

Function RobloxNotFoundError
    !insertmacro ToLog $LOGFILE "Error" "Pekora client was not found."
    MessageBox MB_OK|MB_ICONEXCLAMATION "Pekora is not installed.$\nPlease install Pekora from https://pekora.zip/ and try again." 
        Abort
FunctionEnd

Function RobloxRunningError
    !insertmacro ToLog $LOGFILE "Error" "Active Pekora process found."
    MessageBox MB_YESNOCANCEL|MB_ICONEXCLAMATION "It is recommended that you close Pekora when Reshade is already installed. Select 'yes' to close Pekora immediately." IDYES yes IDNO no
            Abort
        yes:
        MessageBox MB_OKCANCEL|MB_ICONINFORMATION "Pekora will now be closed." IDCANCEL no
            nsProcess::_KillProcess "${ROBLOXCLIENT}"
            pop $R0
            !insertmacro ToLog $LOGFILE "nsProcess" "KillProcess ${ROBLOXCLIENT} with code: $R0"
            ${switch} $R0
                ${case} 0
                    goto no
                ${case} 601
                    !insertmacro StopMessage "No permission to close Pekora."
                    ${break}
                ${case} 603
                    MessageBox MB_OK|MB_ICONEXCLAMATION "Pekora is not currently running."
                    ${break}
                ${case} 605
                    !insertmacro StopMessage "Could not close Pekora. Unsupported operating system."
                    ${break}
                ${case} 606
                    !insertmacro StopMessage "Could not close Pekora. Unable to load NTDLL.DLL. Please try again."
                    ${break}
                ${case} 609
                    !insertmacro StopMessage "Could not close Pekora. Unable to load KERNEL32.DLL. Please try again."
                    ${break}
                ${caseelse}
                    !insertmacro StopMessage "Unable to close Pekora. Please close it manually and try again."
            ${endswitch}
        no:
FunctionEnd