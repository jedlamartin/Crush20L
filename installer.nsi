# ==============================================================================
# NSIS Installer Script for Orange Crush 20L (VST3 & Standalone)
# ==============================================================================

!define APP_NAME "Orange Crush 20L"
!define COMP_NAME "Zephyra"
!define VERSION "1.1.0"
!define VST3_NAME "OrangeCrush20L.vst3"
!define EXE_NAME "OrangeCrush20L.exe"

# Absolute Source Paths based on your build targets
!define SRC_EXE_DIR "C:\Users\zero6575\myplugins\Crush20L\Builds\VisualStudio2022\x64\Release\Standalone Plugin"
!define SRC_VST_DIR "C:\Users\zero6575\myplugins\Crush20L\Builds\VisualStudio2022\x64\Release\VST3\OrangeCrush20L.vst3\Contents\x86_64-win"

# FIXED: Removed the spaces from the installer executable output filename
OutFile "OrangeCrush20L_Setup_v${VERSION}.exe"

# Request admin privileges for writing to Program Files & HKLM Registry
RequestExecutionLevel admin

# Use the Modern User Interface (MUI2)
!include "MUI2.nsh"

# FIXED: This changes the main window caption bar text to exactly what you want
Caption "${APP_NAME} Setup"

# ==============================================================================
# UI Configuration & Custom Pages Layout
# ==============================================================================

# Variables to hold paths from the installation pages
Var StandaloneDir
Var Vst3Dir

# Page 1: Welcome
!insertmacro MUI_PAGE_WELCOME

# Page 2: Component Selection
!define MUI_COMPONENTPAGE_SMALLDESC
!insertmacro MUI_PAGE_COMPONENTS

# Page 3: Custom Destination Page (Standalone Location)
!define MUI_DIRECTORYPAGE_VARIABLE $StandaloneDir
!define MUI_DIRECTORYPAGE_TEXT_TOP "Select the folder where Setup should install the Standalone program files."
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "Standalone location:"
!define MUI_PAGE_CUSTOMFUNCTION_PRE PreStandalonePage
!insertmacro MUI_PAGE_DIRECTORY

# Page 4: Custom Destination Page (VST3 Location)
!define MUI_DIRECTORYPAGE_VARIABLE $Vst3Dir
!define MUI_DIRECTORYPAGE_TEXT_TOP "Select the folder where Setup should install the VST3 plugin files."
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "VST3 location:"
!define MUI_PAGE_CUSTOMFUNCTION_PRE PreVst3Page
!insertmacro MUI_PAGE_DIRECTORY

# Page 5: Start Menu Selection
Var StartMenuFolder
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${COMP_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "StartMenuFolder"
!define MUI_PAGE_CUSTOMFUNCTION_PRE PreStartMenuPage
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

# Page 6: Start Menu / Desktop Shortcut Flags (InstFiles automatically follows)
!insertmacro MUI_PAGE_INSTFILES

# Page 7: Finish
!insertmacro MUI_PAGE_FINISH

# Uninstaller Pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

# FIXED: Internal name definition
Name "${APP_NAME}"

# ==============================================================================
# Initialization
# ==============================================================================
Function .onInit
    # Set up default target paths matching your setup layout sequence
    StrCpy $StandaloneDir "$PROGRAMFILES64\${COMP_NAME}\${APP_NAME}"
    StrCpy $Vst3Dir "$PROGRAMFILES64\Common Files\VST3\${COMP_NAME}\${APP_NAME}"
FunctionEnd

# ==============================================================================
# Installation Sections
# ==============================================================================

Section "Standalone" SecStandalone
    # Size estimate in KB (14.3 MB)
    AddSize 14643
    
    SetOutPath "$StandaloneDir"
    
    # Grab the standalone executable directly from your absolute build directory
    File "${SRC_EXE_DIR}\${EXE_NAME}"
    
    # Create Shortcuts based on default user options
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
        CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
        CreateShortcut "$SMPROGRAMS\$StartMenuFolder\${APP_NAME}.lnk" "$StandaloneDir\${EXE_NAME}"
    !insertmacro MUI_STARTMENU_WRITE_END
    
    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "$StandaloneDir\${EXE_NAME}"
SectionEnd

Section "VST3" SecVst3
    # Size estimate in KB (13.1 MB)
    AddSize 13414
    
    SetOutPath "$Vst3Dir"
    
    # Grab the binary file straight from the deep x86_64-win bundle directory
    File "${SRC_VST_DIR}\${VST3_NAME}"
SectionEnd

# ==============================================================================
# Component Selection Verification Functions
# ==============================================================================

# Skip Standalone Directory selection if component isn't chosen
Function PreStandalonePage
    SectionGetFlags ${SecStandalone} $0
    IntOp $0 $0 & ${SF_SELECTED}
    IntCmp $0 ${SF_SELECTED} +2
        Abort
FunctionEnd

# Skip VST3 Directory selection if component isn't chosen
Function PreVst3Page
    SectionGetFlags ${SecVst3} $0
    IntOp $0 $0 & ${SF_SELECTED}
    IntCmp $0 ${SF_SELECTED} +2
        Abort
FunctionEnd

# Skip Start Menu page if Standalone isn't being installed
Function PreStartMenuPage
    SectionGetFlags ${SecStandalone} $0
    IntOp $0 $0 & ${SF_SELECTED}
    IntCmp $0 ${SF_SELECTED} +2
        Abort
FunctionEnd

# Post-install handling section for generating uninstall references
Section "-Post"
    # Establish base uninstaller write space (Prefers standalone folder if available)
    IfFileExists "$StandaloneDir" UseStandalonePath UseVst3Path
    UseStandalonePath:
        SetOutPath "$StandaloneDir"
        Goto WriteKeys
    UseVst3Path:
        SetOutPath "$Vst3Dir"
    
    WriteKeys:
    WriteUninstaller "$OUTDIR\Uninstall_${APP_NAME}.exe"
    
    # Add/Remove Programs validation tracking entries
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" '"$OUTDIR\Uninstall_${APP_NAME}.exe"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayVersion" "${VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "Publisher" "${COMP_NAME}"
    
    # Keep track of installation folders inside the registry for cleaner uninstalls
    WriteRegStr HKLM "Software\${COMP_NAME}\${APP_NAME}" "StandaloneLocation" "$StandaloneDir"
    WriteRegStr HKLM "Software\${COMP_NAME}\${APP_NAME}" "Vst3Location" "$Vst3Dir"
SectionEnd

# ==============================================================================
# Uninstaller Section
# ==============================================================================
Section "Uninstall"
    # Pull installed path locations from the secure registry
    ReadRegStr $0 HKLM "Software\${COMP_NAME}\${APP_NAME}" "StandaloneLocation"
    ReadRegStr $1 HKLM "Software\${COMP_NAME}\${APP_NAME}" "Vst3Location"
    ReadRegStr $StartMenuFolder HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "StartMenuFolder"

    # Remove Standalone assets & shortcuts
    Delete "$0\${EXE_NAME}"
    Delete "$0\Uninstall_${APP_NAME}.exe"
    RMDir "$0"
    Delete "$DESKTOP\${APP_NAME}.lnk"
    
    # Clean up Start Menu directory structures
    Delete "$SMPROGRAMS\$StartMenuFolder\${APP_NAME}.lnk"
    RMDir "$SMPROGRAMS\$StartMenuFolder"

    # Remove VST3 plugin binary bundle
    Delete "$1\${VST3_NAME}"
    Delete "$1\Uninstall_${APP_NAME}.exe"
    RMDir "$1"
    
    # Prune nested folders if they are empty
    RMDir "$PROGRAMFILES64\Common Files\VST3\${COMP_NAME}"
    RMDir "$PROGRAMFILES64\${COMP_NAME}"

    # Remove remaining tracking keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
    DeleteRegKey HKLM "Software\${COMP_NAME}\${APP_NAME}"
SectionEnd