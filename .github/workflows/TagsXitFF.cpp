// C++17 · TagsXitFF.cpp · Windows 10/11 x64 · MSVC or MinGW-w64
// Standalone settings dashboard. No game process access, input automation, memory
// inspection, injection, anti-cheat evasion, or screenshot hiding.
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#pragma comment(lib, "Comctl32.lib")

static HINSTANCE gInst;
static HWND gMain, gPage, gStatus;
static int gTab = 0, gFov = 120;
static bool gEspLine = true, gEspBox = true, gEspHealth = true, gEspSkeleton = false, gEspName = true;
static COLORREF gAccent = RGB(0, 190, 255);
static const wchar_t* tabs[] = {L"AIM SETTINGS", L"ESP SETTINGS", L"COLORS", L"GENERAL"};

void SetStatus(const wchar_t* s) { SetWindowTextW(gStatus, s); }
HWND AddLabel(HWND parent, const wchar_t* text, int x, int y, int w=280, int h=24) {
    return CreateWindowW(L"STATIC", text, WS_CHILD|WS_VISIBLE, x,y,w,h,parent,nullptr,gInst,nullptr);
}
HWND AddCheck(HWND parent, const wchar_t* text, int id, int x, int y, bool checked) {
    HWND h=CreateWindowW(L"BUTTON",text,WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,x,y,260,25,parent,(HMENU)(INT_PTR)id,gInst,nullptr);
    SendMessageW(h,BM_SETCHECK,checked?BST_CHECKED:BST_UNCHECKED,0); return h;
}
void AddButton(HWND parent,const wchar_t* text,int id,int x,int y,int w=130) {
    CreateWindowW(L"BUTTON",text,WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,x,y,w,30,parent,(HMENU)(INT_PTR)id,gInst,nullptr);
}
void ShowPage();
void ClearPage() {
    for(HWND c=GetWindow(gPage,GW_CHILD);c;c=GetWindow(gPage,GW_CHILD)) DestroyWindow(c);
}
void SaveConfig() {
    std::ofstream f("TagsXitFF.ini");
    f<<"fov="<<gFov<<"\nline="<<gEspLine<<"\nbox="<<gEspBox<<"\nhealth="<<gEspHealth
     <<"\nskeleton="<<gEspSkeleton<<"\nname="<<gEspName<<"\naccent="<<gAccent<<"\n";
    SetStatus(L"Configurações salvas em TagsXitFF.ini");
}
void LoadConfig() {
    std::ifstream f("TagsXitFF.ini"); std::string line;
    while(std::getline(f,line)) { auto p=line.find('='); if(p==std::string::npos) continue;
        auto k=line.substr(0,p),v=line.substr(p+1); try {
            if(k=="fov") gFov=std::clamp(std::stoi(v),30,500);
            else if(k=="line") gEspLine=std::stoi(v)!=0; else if(k=="box") gEspBox=std::stoi(v)!=0;
            else if(k=="health") gEspHealth=std::stoi(v)!=0; else if(k=="skeleton") gEspSkeleton=std::stoi(v)!=0;
            else if(k=="name") gEspName=std::stoi(v)!=0; else if(k=="accent") gAccent=(COLORREF)std::stoul(v);
        } catch(...) {} }
    SetStatus(L"Configurações carregadas."); ShowPage();
}
void ShowPage() {
    ClearPage();
    if(gTab==0) {
        AddLabel(gPage,L"AIM SETTINGS  |  Painel de configuração visual",22,18,500);
        AddLabel(gPage,L"FOV visual (30–500 px):",22,64);
        HWND s=CreateWindowW(TRACKBAR_CLASSW,L"",WS_CHILD|WS_VISIBLE|TBS_AUTOTICKS,22,92,390,38,gPage,(HMENU)101,gInst,nullptr);
        SendMessageW(s,TBM_SETRANGE,TRUE,MAKELONG(30,500)); SendMessageW(s,TBM_SETPOS,TRUE,gFov);
        AddLabel(gPage,L"O círculo é apenas uma referência visual nesta janela.",22,140,500);
        AddLabel(gPage,L"Legit / Rage / Silent Aim: indisponíveis (sem automação de jogo).",22,174,560);
    } else if(gTab==1) {
        AddLabel(gPage,L"ESP SETTINGS  |  Preferências de elementos visuais",22,18,500);
        AddCheck(gPage,L"Line",201,22,60,gEspLine); AddCheck(gPage,L"Box",202,22,94,gEspBox);
        AddCheck(gPage,L"Health bar",203,22,128,gEspHealth); AddCheck(gPage,L"Skeleton",204,22,162,gEspSkeleton);
        AddCheck(gPage,L"Name",205,22,196,gEspName);
        AddLabel(gPage,L"Estas opções armazenam preferências; não leem nem desenham dados do Free Fire.",22,244,590,42);
    } else if(gTab==2) {
        AddLabel(gPage,L"COLORS  |  Cor de destaque da interface",22,18,500);
        AddButton(gPage,L"Escolher cor",301,22,65,150);
        AddLabel(gPage,L"A cor selecionada será usada nos detalhes da interface.",22,112,500);
    } else {
        AddLabel(gPage,L"GENERAL  |  Configurações do aplicativo",22,18,500);
        AddButton(gPage,L"Salvar config",401,22,65,150); AddButton(gPage,L"Carregar config",402,190,65,150);
        AddButton(gPage,L"Minimizar",403,22,112,150); AddButton(gPage,L"Unload / Sair",404,190,112,150);
        AddLabel(gPage,L"Aplicativo independente: sem injeção, hooks, leitura de memória ou bypass.",22,174,590,40);
    }
    InvalidateRect(gMain,nullptr,TRUE);
}
LRESULT CALLBACK WndProc(HWND h,UINT m,WPARAM w,LPARAM l) {
    switch(m) {
    case WM_CREATE: {
        gMain=h;
        for(int i=0;i<4;i++) CreateWindowW(L"BUTTON",tabs[i],WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,18+i*145,18,137,32,h,(HMENU)(INT_PTR)(500+i),gInst,nullptr);
        gPage=CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE|WS_BORDER,18,62,590,300,h,nullptr,gInst,nullptr);
        gStatus=CreateWindowW(L"STATIC",L"Pronto.",WS_CHILD|WS_VISIBLE,18,374,590,24,h,nullptr,gInst,nullptr);
        ShowPage(); return 0; }
    case WM_HSCROLL: if((HWND)l && GetDlgCtrlID((HWND)l)==101) { gFov=(int)SendMessageW((HWND)l,TBM_GETPOS,0,0); InvalidateRect(gMain,nullptr,TRUE); } return 0;
    case WM_COMMAND: {
        int id=LOWORD(w);
        if(id>=500&&id<=503) { gTab=id-500; ShowPage(); }
        else if(id>=201&&id<=205) { bool v=SendMessageW((HWND)l,BM_GETCHECK,0,0)==BST_CHECKED; if(id==201)gEspLine=v;if(id==202)gEspBox=v;if(id==203)gEspHealth=v;if(id==204)gEspSkeleton=v;if(id==205)gEspName=v; }
        else if(id==301) { CHOOSECOLORW cc{}; COLORREF custom[16]{}; cc.lStructSize=sizeof(cc);cc.hwndOwner=h;cc.lpCustColors=custom;cc.rgbResult=gAccent;cc.Flags=CC_FULLOPEN|CC_RGBINIT;if(ChooseColorW(&cc)){gAccent=cc.rgbResult;SetStatus(L"Cor atualizada.");} }
        else if(id==401) SaveConfig(); else if(id==402) LoadConfig(); else if(id==403) ShowWindow(h,SW_MINIMIZE); else if(id==404) DestroyWindow(h);
        return 0; }
    case WM_PAINT: { PAINTSTRUCT ps; HDC dc=BeginPaint(h,&ps); RECT r;GetClientRect(h,&r);
        HBRUSH bg=CreateSolidBrush(RGB(19,22,29));FillRect(dc,&r,bg);DeleteObject(bg);
        SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(235,240,248));
        HPEN pen=CreatePen(PS_SOLID,2,gAccent);auto old=(HPEN)SelectObject(dc,pen);
        int cx=520,cy=220;Ellipse(dc,cx-gFov/5,cy-gFov/5,cx+gFov/5,cy+gFov/5);
        SelectObject(dc,old);DeleteObject(pen);EndPaint(h,&ps);return 0; }
    case WM_DESTROY: PostQuitMessage(0); return 0;
    } return DefWindowProcW(h,m,w,l);
}
int WINAPI wWinMain(HINSTANCE inst,HINSTANCE, PWSTR,int show) {
    gInst=inst; INITCOMMONCONTROLSEX ic{sizeof(ic),ICC_BAR_CLASSES};InitCommonControlsEx(&ic);
    WNDCLASSW wc{};wc.lpfnWndProc=WndProc;wc.hInstance=inst;wc.lpszClassName=L"TagsXitFFWindow";wc.hCursor=LoadCursor(nullptr,IDC_ARROW);wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);RegisterClassW(&wc);
    HWND h=CreateWindowW(wc.lpszClassName,L"TagsXit FF | Windows x64",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT,CW_USEDEFAULT,640,455,nullptr,nullptr,inst,nullptr);
    if(!h)return 1;ShowWindow(h,show);UpdateWindow(h);MSG msg;while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}return (int)msg.wParam;
}
