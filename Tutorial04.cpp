// include the basic windows header files and the Direct3D header files
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// global declarations
IDXGISwapChain* swapchain;             // the pointer to the swap chain interface
ID3D11Device* dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext* devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView* backbuffer;    // the pointer to our back buffer
ID3D11InputLayout* pLayout;            // the pointer to the input layout
ID3D11VertexShader* pVS;               // the pointer to the vertex shader
ID3D11PixelShader* pPS;                // the pointer to the pixel shader
ID3D11Buffer* pVBuffer;                // the pointer to the vertex buffer
ID3D11Buffer* pVBuffer2;                // the pointer to the vertex buffer

// a struct to define a single vertex
struct VERTEX { FLOAT X, Y, Z; D3DXCOLOR Color; };

// function prototypes
void InitD3D(HWND hWnd);    // sets up and initializes Direct3D
void RenderFrame(void);     // renders a single frame
void CleanD3D(void);        // closes Direct3D and releases memory
void InitGraphics(void);    // creates the shape to render
void InitGraphics2(void);
void InitPipeline(void);    // loads and prepares the shaders

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

    RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(NULL,
        L"WindowClass",
        L"C093182_이수진 <환타 오렌지맛 먹은 연필>",
        WS_OVERLAPPEDWINDOW,
        300,
        300,
        wr.right - wr.left,
        wr.bottom - wr.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, nCmdShow);

    // set up and initialize Direct3D
    InitD3D(hWnd);

    // enter the main loop:

    MSG msg;

    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        }

        RenderFrame();
    }

    // clean up DirectX and COM
    CleanD3D();

    return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                   // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
    scd.BufferDesc.Width = SCREEN_WIDTH;                   // set the back buffer width
    scd.BufferDesc.Height = SCREEN_HEIGHT;                 // set the back buffer height
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
    scd.OutputWindow = hWnd;                               // the window to be used
    scd.SampleDesc.Count = 4;                              // how many multisamples
    scd.Windowed = TRUE;                                   // windowed/full-screen mode
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &dev,
        NULL,
        &devcon);


    // get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, NULL);


    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    devcon->RSSetViewports(1, &viewport);

    InitPipeline();
    InitGraphics();
    InitGraphics2();
}


// this is the function used to render a single frame
void RenderFrame(void)
{
    // clear the back buffer to a deep blue
    devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));  //배경색 바꾸는 곳

    // select which vertex buffer to display
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

    // select which primtive type we are using
    devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw the vertex buffer to the back buffer
    devcon->Draw(69, 0);

    ///////////////////////
    devcon->IASetVertexBuffers(0, 1, &pVBuffer2, &stride, &offset);

    // select which primtive type we are using
    devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);

    // draw the vertex buffer to the back buffer
    devcon->Draw(20, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}


// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
    swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

    // close and release all existing COM objects
    pLayout->Release();
    pVS->Release();
    pPS->Release();
    pVBuffer->Release();
    pVBuffer2->Release();

    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
}


// this is the function that creates the shape to render
void InitGraphics()
{
    //연필 밑심.
    // 
    // create a triangle using the VERTEX struct
    VERTEX OurVertices[] =
    {
        {-0.25f, -0.37f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)}, //H
        {-0.185f, 0.03, 0.0f,D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //F
        {-0.15f, -0.131f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //Z

        {-0.25f, -0.37f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)}, //H
        {-0.15f, -0.131f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //Z
        {-0.08f, -0.23f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //A1

        {-0.25f, -0.37f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)}, //H
        {-0.08f, -0.23f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)}, //A1
        {0.05f, -0.3f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},   //D
 //연필 밑심.


        {-0.185f, 0.03, 0.0f,D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},   //F
        {0.217f, 0.62f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //G
        {-0.15f, -0.131f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},//Z

        {-0.15f, -0.131f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)}, //Z
        {0.217f, 0.62f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //G
        {0.246f, 0.473f, 0.0f,D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //R

        {-0.15f, -0.131f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)}, //Z
        {0.246f, 0.473f, 0.0f,D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //R
        {-0.08f, -0.23f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //A1

        {-0.08f, -0.23f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)}, //A1
        {0.246f, 0.473f, 0.0f,D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //R
        {0.317f, 0.370f, 0.0f,D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //W

        {-0.08f, -0.23f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)}, //A1
        {0.317f, 0.370f, 0.0f,D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //W
        {0.05f, -0.3f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},   //D

        {0.05f, -0.3f, 0.0f, D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},   //D
        {0.317f, 0.370f, 0.0f,D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},  //W
        {0.45f, 0.3f, 0.0f,D3DXCOLOR(0.760f, 0.329f, 0.078f, 1.0f)},      //E
//연필 몸통.


        {0.217f, 0.62f, 0.0f,D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},    //G
        {0.305f, 0.545f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //T
         {0.246f, 0.473f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)}, //R

         {0.305f, 0.545f, 0.0f,D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //T
         {0.317f, 0.370f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //W
         {0.246f, 0.473f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)}, //R

         {0.305f, 0.545f, 0.0f,D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //T
         {0.390f, 0.425f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //V
         {0.317f, 0.370f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)}, //W

         {0.390f, 0.425f, 0.0f,D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},    //V
        {0.45f, 0.3f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)},  //E
        {0.317f, 0.370f, 0.0f, D3DXCOLOR(0.886f, 0.658f, 0.411f, 1.0f)}, //W
//연필 윗부분


        {0.26f, 0.55f, 0.0f,D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},    //L
        {0.293f, 0.522f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //U
         {0.269f, 0.488f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //S

         {0.269f, 0.488f, 0.0f,D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //S
         {0.293f, 0.522f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //U
         {0.326f, 0.397f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //B1

         {0.326f, 0.397f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //B1
         {0.293f, 0.522f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //U
         {0.359f, 0.439f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //C1

        {0.326f, 0.397f, 0.0f,D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},   //B1
        {0.359f, 0.439f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //C1
        {0.4f, 0.35f, 0.0f,D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},    //N
//연필 윗 심지


         {-0.25f, -0.37f, 0.0f,D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //H
        {-0.231f, -0.253f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //I
         {-0.232f, -0.305f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //K

         {-0.25f, -0.37f, 0.0f,D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //H
         {-0.232f, -0.305f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //K
         {-0.205f, -0.263f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //M

          {-0.25f, -0.37f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //H
         {-0.205f, -0.263f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //M
         {-0.22f, -0.324f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //O

          {-0.25f, -0.37f, 0.0f,D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //H
         {-0.22f, -0.324f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //O
         {-0.176f, -0.296f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //J

          {-0.25f, -0.37f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //H
         {-0.176f, -0.296f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //J
         {-0.206f, -0.340f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //P

         {-0.25f, -0.37f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //H
         {-0.206f, -0.340f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //P
         {-0.151f, -0.347f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //Q
//연필 아래 심
    };

    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bd.ByteWidth = sizeof(VERTEX) * 69;             // size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

    dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer


    // copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
    memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
    devcon->Unmap(pVBuffer, NULL);                                      // unmap the buffer
}

void InitGraphics2()
{
    //연필 밑심.
    // 
    // create a triangle using the VERTEX struct
    VERTEX OurVertices[] =
    {
        {-0.670f, -0.752f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //N1
        {-0.597f, -0.558, 0.0f,D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //M1
        {-0.418f, -0.531f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //L1
        {-0.365f, -0.344f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //01

        {-0.25f, -0.37f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //H
        {-0.185f, 0.03f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},  //F
        {-0.15f, -0.131f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //Z
        {-0.08f, -0.23f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //A1
        {0.05f, -0.3f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},   //D
        {0.45f, 0.3f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //E
        {0.390f, 0.425f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //V
        {0.305f, 0.545f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},   //T
        {0.217f, 0.62f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //G
        {0.246f, 0.473f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //R
        {0.317f, 0.370f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},   //W
        {0.45f, 0.3f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //E
        {0.317f, 0.370f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //W
        {-0.08f, -0.23f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)},   //A1
        {-0.15f, -0.131f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)}, //Z
        {0.246f, 0.473f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)} //R

//연필 아래 심
    };

    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    bd.ByteWidth = sizeof(VERTEX) * 20;             // size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

    dev->CreateBuffer(&bd, NULL, &pVBuffer2);       // create the buffer


    // copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    devcon->Map(pVBuffer2, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
    memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
    devcon->Unmap(pVBuffer2, NULL);                                      // unmap the buffer
}


// this function loads and prepares the shaders
void InitPipeline()
{
    // load and compile the two shaders
    ID3D10Blob* VS, * PS;
    D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
    D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

    // encapsulate both shaders into shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);
}