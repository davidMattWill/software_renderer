#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "geometry.h"

#define Assert(c) do { if (!(c)) __debugbreak(); } while (0)
#define buffer_size 100

#pragma comment (lib, "user32.lib")
#pragma comment (lib, "gdi32.lib")

typedef uint32_t u32;

int ClientWidth;
int ClientHeight;
void *Memory;



void drawLine(vec3d v1, vec3d v2);
void drawTriangle(triangle t);
void drawTriangleLines(triangle t);
void projectTriangle(triangle t, triangle *tProj, float **matrix4x4, int width, int height);
void rotateTriangle(triangle *t, float xTheta, float yTheta, float zTheta);


void DrawPixel(int X, int Y, u32 Color) {
    u32 *Pixel = (u32 *)Memory;
    Pixel += Y * ClientWidth + X;
    *Pixel = Color;
}

void ClearScreen(u32 Color) {
    u32 *Pixel = (u32 *)Memory;
    for(int Index = 0; Index < ClientWidth * ClientHeight; ++Index) {
        *Pixel++ = Color;
    }
}

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
    switch(Message) {
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;
        case WM_KEYDOWN: {
            switch(WParam) {
                case 'O': { 
                    DestroyWindow(Window); 
                } break;
            }
        } break;
        default: {
            return DefWindowProcW(Window, Message, WParam, LParam);
        }
    }
    
    return 0;
}

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR Cmd, int CmdShow) {
    //init vertex data
    mesh cubeMesh;
    createCube(&cubeMesh);

    


    
    // Create a window.
    WNDCLASSW WindowClass = {
        .lpszClassName = L"MyWindowClass",
        .lpfnWndProc = WindowProc,
        .hInstance = Instance,
        .hCursor = LoadCursor(NULL, IDC_CROSS),
    };
    ATOM Atom = RegisterClassW(&WindowClass);
    Assert(Atom && "Failed to register a window");
    
    HWND Window = CreateWindowW(WindowClass.lpszClassName, L"Drawing Pixels", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, Instance, NULL);
    Assert(Window && "Failed to create a window");
    
    ShowWindow(Window, CmdShow);
    
    // Allocate memory.
    
    RECT Rect;
    GetClientRect(Window, &Rect);
    ClientWidth = Rect.right - Rect.left;
    ClientHeight = Rect.bottom - Rect.top;
    
    Memory = VirtualAlloc(0, ClientWidth * ClientHeight * 4, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    // Create BITMAPINFO struct for StretchDIBits().
    
    BITMAPINFO BitmapInfo;
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = ClientWidth;
    // Negative biHeight makes top left as the coordinate system origin. Otherwise it's bottom left.
    BitmapInfo.bmiHeader.biHeight = -ClientHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    HDC DeviceContext = GetDC(Window);

    //create projection matrix and project values
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fov = 90;
    float aspectRatio = (float) ClientHeight/ (float) ClientWidth;

    
    float **projMat = createProjMat(fov, fNear, fFar, aspectRatio);

    float rotX = 0;
    float rotZ = 120;

   
    for(;;) {
        MSG Message;
        if(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            if(Message.message == WM_QUIT) break;
            TranslateMessage(&Message);
            DispatchMessage(&Message);
            continue;
        }    
        
        // Clear screen.
        
        ClearScreen(0x333333);


        //draw
        for(int i = 0; i < cubeMesh.numTris; i++){
            triangle tTrans, tRotX, tRotZ, tProj;
            

            //rotate
            rotateTriangleZ(cubeMesh.tris[i], &tRotZ, rotZ);
            rotateTriangleX(tRotZ, &tRotX, rotX);
            //

            //translate
            tTrans = tRotX;
            tTrans.vertices[0].z += 3;
            tTrans.vertices[1].z += 3;
            tTrans.vertices[2].z += 3;

            //project and draw
            multProjMat(projMat, tTrans, &tProj, ClientWidth, ClientHeight);
            drawTriangle(tProj);
            drawTriangleLines(tProj);
        }

        
        Sleep(60);
        rotX+= 0.1;
        rotZ+= 0.1;

   
    
        // Display image.
        
        StretchDIBits(DeviceContext, 0, 0, ClientWidth, ClientHeight, 0, 0, ClientWidth, ClientHeight, Memory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    }
    
    return 0;
}


bool edgeFunction(vec3d a, vec3d b, vec3d c){
    //check that vertices are 2 dim
    return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x) >= 0);
}

void drawTriangle(triangle t){
    //***POINTS NEED TO CYCLE CLOCKWISE!!*****
    vec3d a,b,c;
    a = t.vertices[0];
    b = t.vertices[1];
    c = t.vertices[2];

    //form a bounding box around the triangle
    int xStart = floor(fmin(a.x, b.x));
    xStart = floor(fmin(xStart, c.x));

    int xEnd = floor(fmax(a.x, b.x));
    xEnd = floor(fmax(xEnd, c.x));
    
    int yStart = floor(fmin(a.y, b.y));
    yStart = floor(fmin(yStart, c.y));

    int yEnd = floor(fmax(a.y, b.y));
    yEnd = floor(fmax(yEnd, c.y));

    for(int yi = yStart; yi < yEnd; yi++){
        for(int xi = xStart; xi < xEnd; xi++){
            //NOT CORRECT, WILL BE WRITTEN OVER!!!
            vec3d pt;
            pt.x = xi;
            pt.y = yi;
            pt.z = 0;
               
            bool inside = true;
            inside &= edgeFunction(a,b,pt);
            inside &= edgeFunction(b,c,pt);
            inside &= edgeFunction(c,a,pt);

            if(inside == true){
                DrawPixel((int) floorf(pt.x), (int)floor(pt.y), 0xFFFFFF);
            }

            //IF YOU REMOVE DRAWTRIANGLELINES, THERE WILL BE MISSING PIXELS!!
        
        }
    }

    
}

void drawLine(vec3d a, vec3d b)
{
   
    int x0, y0, x1, y1;
    x0 = (int)a.x; y0 = (int)a.y;
    x1 = (int)b.x; y1 = (int)b.y;


    int dx = x1 - x0; 
    int dy = y1 - y0; 
  
    // calculate steps required for generating pixels 
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy); 
  
    // calculate increment in x & y for each steps 
    float Xinc = dx / (float)steps; 
    float Yinc = dy / (float)steps; 
  
    // Put pixel for each step 
    float X = x0; 
    float Y = y0; 
    for (int i = 0; i <= steps; i++) { 
        DrawPixel(round(X), round(Y), 0xFFFFFF); 
        X += Xinc; 
        Y += Yinc;
         
                    
    } 
    
 

}

void drawTriangleLines(triangle t){

    vec3d a,b,c;
    a = t.vertices[0];
    b = t.vertices[1];
    c = t.vertices[2];

    if(
        a.x > ClientWidth || a.x < 0 || a.y > ClientHeight || a.y < 0 || 
        b.x > ClientWidth || b.x < 0 ||b.y > ClientHeight || b.y < 0 || 
        c.x > ClientWidth || c.x < 0 || c.y > ClientHeight || c.y < 0){
        
        return;
        
    }

    drawLine(a,b);
    drawLine(b,c);
    drawLine(a,c);
}







   
