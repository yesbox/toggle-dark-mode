#include <windows.h>

#define REG_PATH L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"

void PrintHelp(void) {
    MessageBoxW(
        NULL,
        L"Toggles between light and dark mode.\n\n"
        L"Options:\n"
        L"-l, --light Set light mode.\n"
        L"-d, --dark  Set dark mode.\n"
        L"-h, --help  Show this help message.\n",
        L"Toggle Dark Mode",
        MB_OK | MB_ICONINFORMATION
    );
}

int ParseThemeArg(int argc, wchar_t** argv) {
    for (int i = 1; i < argc; ++i) {
        if (lstrcmpiW(argv[i], L"-h") == 0 || lstrcmpiW(argv[i], L"--help") == 0)
            return -2;
        if (lstrcmpiW(argv[i], L"-l") == 0 || lstrcmpiW(argv[i], L"--light") == 0)
            return 1;
        if (lstrcmpiW(argv[i], L"-d") == 0 || lstrcmpiW(argv[i], L"--dark") == 0)
            return 0;
    }
    return -1;
}

DWORD GetThemeValue(const wchar_t* valueName, DWORD defaultValue) {
    HKEY  hKey;
    DWORD value = defaultValue;
    DWORD size  = sizeof(value);
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, valueName, NULL, NULL, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }
    return value;
}

int SetThemeValue(const wchar_t* valueName, DWORD desiredValue) {
    if (desiredValue != 0 && desiredValue != 1) {
        return ERROR_INVALID_PARAMETER;
    }

    HKEY hKey;
    int error = RegCreateKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
    if (error == ERROR_SUCCESS) {
        error = RegSetValueExW(hKey, valueName, 0, REG_DWORD, (const BYTE*)&desiredValue, sizeof(desiredValue));
        RegCloseKey(hKey);
    }
    return error;
}

void RefreshTheme(void) {
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 1000, NULL);
    SendNotifyMessageW(HWND_BROADCAST, WM_THEMECHANGED, 0, 0);
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
}

int Main(void) {
    int desiredValue = -1;
    int error = 0;
    int argc = 0;

    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv) {
        desiredValue = ParseThemeArg(argc, argv);
        LocalFree(argv);
    }

    if (desiredValue == -2) {
        PrintHelp();
        return 0;
    }

    if (desiredValue < 0) {
        DWORD current = GetThemeValue(L"AppsUseLightTheme", 1);
        desiredValue = !current;
    }

    error |= SetThemeValue(L"AppsUseLightTheme", (DWORD)desiredValue);
    error |= SetThemeValue(L"SystemUsesLightTheme", (DWORD)desiredValue);

    RefreshTheme();

    return error;
}

void Entry(void) {
    ExitProcess(Main());
}
