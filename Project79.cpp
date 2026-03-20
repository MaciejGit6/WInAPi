#include <windows.h>
#include <string>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

#define IDC_DISPLAY 100
#define IDC_BTN_BASE 200

HWND hDisplay;
std::wstring current = L"";
double stored = 0;
wchar_t op = 0;

void UpdateDisplay()
{
    SetWindowTextW(hDisplay, current.empty() ? L"0" : current.c_str());
}

void Calculate()
{
    double val = _wtof(current.c_str());

    switch (op)
    {
    case L'+': stored += val; break;
    case L'-': stored -= val; break;
    case L'*': stored *= val; break;
    case L'/': if (val != 0) stored /= val; break;
    }

    current = std::to_wstring(stored);

    // Optional: Trim trailing zeros for a cleaner display
    current.erase(current.find_last_not_of(L'0') + 1, std::wstring::npos);
    if (current.back() == L'.') current.pop_back();

    op = 0;
}

void OnButtonClick(int id)
{
    int index = id - IDC_BTN_BASE;

    // Safety check: ensure the index is within our button array bounds
    if (index < 0 || index >= 18) return;

    const wchar_t* labels[] = {
        L"7",L"8",L"9",L"/",
        L"4",L"5",L"6",L"*",
        L"1",L"2",L"3",L"-",
        L"C",L"0",L".",L"+",
        L"BS",L"="
    };

    std::wstring val = labels[index];

    if (val == L"C")
    {
        current = L"";
        stored = 0;
        op = 0;
    }
    else if (val == L"BS")
    {
        if (!current.empty())
            current.pop_back();
    }
    else if (val == L"=")
    {
        Calculate();
    }
    else if (val == L"+" || val == L"-" || val == L"*" || val == L"/")
    {
        stored = _wtof(current.c_str());
        current = L"";
        op = val[0];
    }
    else
    {
        current += val;
    }

    UpdateDisplay();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        hDisplay = CreateWindowW(L"EDIT", L"0",
            WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_READONLY | WS_BORDER,
            10, 10, 260, 40,
            hwnd, (HMENU)(INT_PTR)IDC_DISPLAY, NULL, NULL);

        const wchar_t* buttons[] = {
            L"7",L"8",L"9",L"/",
            L"4",L"5",L"6",L"*",
            L"1",L"2",L"3",L"-",
            L"C",L"0",L".",L"+",
            L"BS",L"="
        };

        int x = 10, y = 60;
        int w = 60, h = 40;
        int id = IDC_BTN_BASE;

        for (int i = 0; i < 18; i++)
        {
            CreateWindowW(L"BUTTON", buttons[i],
                WS_VISIBLE | WS_CHILD,
                x, y, w, h,
                hwnd, (HMENU)(INT_PTR)(id++), NULL, NULL);

            x += 65;
            if ((i + 1) % 4 == 0)
            {
                x = 10;
                y += 45;
            }
        }
        return 0; // Handled
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);

        // Only process if the ID belongs to our generated buttons
        if (id >= IDC_BTN_BASE && id < IDC_BTN_BASE + 18)
        {
            OnButtonClick(id);
        }
        return 0; // Handled
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0; // Handled
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Changed WinMain to wWinMain for Unicode compatibility
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"CalcClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Added a default background color

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(L"CalcClass", L"DevCalculator",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, 300, 350,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}