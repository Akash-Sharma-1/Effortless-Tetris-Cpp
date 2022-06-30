// windres -i  block.rc -o block.o
// gcc -mwindows tetris1.cpp block.o -o tetris1
//https://docs.microsoft.com/en-us/windows/win32/api/
//Authored by Akash Sharma

#include <windows.h>

// defining the instance and windows 

HINSTANCE hInstance;
HWND hMainWindow;

HDC hMemDC, hBlockDC;
HBITMAP hMemPrev, hBlockPrev;

// defining a static board for now
// here 0,1,2 ... represent the colors in block.bmp file
int board[12][25];

//position of the top corner of the block in 2d plane
class POSITION {
    public :
        int x;
        int y;
};

//Defining each block as a 4 'blocked' item with the absolute anchor being 0,0
class BLOCK{
    public :
        // block rotation status
        int rotate;
        // Position defining relative to absolute anchor 0,0
        POSITION p[3];
};

// All possible types of blocks (making combinations from rotation and position)
BLOCK block[8] = {
    {1, {{0,  0},{0, 0}, {0 ,0}}},  // NULL - usage for placeholding 
    {2, {{0, -1},{0, 1}, {0 ,2}}},  // Straight line  
    { 4 , {{ 0 , - 1 }, { 0 , 1 }, { 1 , 1 }}},   // L shape 1
    { 4 , {{ 0 , - 1 },{ 0 , 1 }, {- 1 , 1 }}},   // L shape 2
    { 2 , {{ 0 , - 1 },{ 1 , 0 }, { 1 , 1 }}},   // Z shape 1
    {2, {{0, -1},{-1,0}, {-1,1}}},  // Z shape 2
    {1, {{0,  1},{1, 0}, {1 ,1}}},  // Square shape
    {4, {{0, -1},{1, 0}, {-1 ,0}}},  // T shape 
};

//gives the status of the incoming block 
class STATUS {
    public:
        //position coordinates
        int x;
        int y;
        // rotation status
        int rotate;  
        // type of block
        int type;
};

STATUS current;

//randomiser
int random(int max) {
    return (int)(rand() / (RAND_MAX + 1.0) * max);
}


//changing blocks status to given status
bool putBlock(STATUS s, bool action = false) {
    //can't do anything here
    if(board[s.x][s.y] != 0) {
        return  false ;
    }
    //updating the board
    if(action) {
        board[s.x][s.y] = s.type;
    }
    //update the board with other things
    for(int i = 0; i < 3; i++) {
        int dx = block[s.type].p[i].x;
        int dy = block[s.type].p[i].y;
        int r = s.rotate % block[s.type].rotate;
        for(int j = 0; j < r; j++) {
            int nx = dx, ny = dy;
            dx = ny; dy = -nx;
        }
        if(board[s.x + dx][s.y + dy] != 0) {
            return  false ;
        }
        if(action) {
            board[s.x + dx][s.y + dy] = s.type;
        }
    }
    // facing recursion issues here - hence shifted down - TODO:
    if(!action) {
        putBlock(s, true);
    }
    return true;
}

// delete the old block after performing the action
bool deleteBlock(STATUS s) {
    //updating the board
    board[s.x][s.y] = 0;
    //update the board with other things
    for(int i = 0; i < 3; i++) {
        int dx = block[s.type].p[i].x;
        int dy = block[s.type].p[i].y;
        int r = s.rotate % block[s.type].rotate;
        for(int j = 0; j < r; j++) {
            int nx = dx, ny = dy;
            dx = ny; dy = -nx;
        }
        board[s.x + dx][s.y + dy] = 0;
    }

    return true;
}

//board array to the window
void showBoard() {
    for(int x = 1; x <= 10; x++) {
        for(int y = 1; y <= 20; y++) {
            BitBlt(hMemDC, (x - 1) * 24, (20 -y) * 24, 24, 24, hBlockDC, 0, board[x][y] * 24, SRCCOPY);
        }
    }
}

// actions for user inputs
bool processInput() {
    bool ret = false ;

    STATUS n = current;
    // changing status based on key input
    if(GetAsyncKeyState(VK_LEFT)) {
        n.x--;
    } else if(GetAsyncKeyState(VK_RIGHT)) {
        n.x++;
    } else if(GetAsyncKeyState(VK_UP)) {
        n.rotate++;
    } else if(GetAsyncKeyState(VK_DOWN)) {
        n.y--;
        ret = true;
    }

    //change board and other things based on the new block status
    if(n.x != current.x || n.y != current.y || n.rotate != current.rotate) {
        deleteBlock(current);
        if(putBlock(n)) {
            current = n;
        } else {
            putBlock(current);
        }
    }
    
    return ret;
}

//ending the game 
void gameOver() {
    //kill timer
    KillTimer(hMainWindow, 100);

    //make all the marked blocks as red
    for(int x = 1; x <= 10;x++) {
        for(int y = 1; y <= 20; y++) {
            if(board[x][y] != 0) {
                board[x][y] = 1;
            }
        }
    }

    // invalidate the window
    InvalidateRect(hMainWindow, NULL, false);
}

//making the height shorter 
void deleteLine() {
    for(int y = 1; y < 23; y++) {
        bool flag = true;
        for(int x = 1;x <= 10; x++) {
            if(board[x][y] == 0) {
                flag = false ;
            }
        }
        
        if(flag) {
            for(int j = y; j < 23; j++) {
                for(int i = 1; i <= 10; i++) {
                    board[i][j] = board[i][j + 1];
                }
            }
            y--;
        }
    }
}

void blockDown() {
    deleteBlock(current);
    current.y--;
    if(!putBlock(current)) {
        current.y++;
        putBlock(current);
        
        deleteLine();
        
        current.x = 5;
        current.y = 21;
        current.type = random(7) + 1;
        current.rotate = random(4);
        if(!putBlock(current)) {
            gameOver();
        }
    }
}

// callback function to process messages sent to a window
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            for(int x = 0; x < 12; x++) {
                for(int y = 0; y < 25; y++) {
                    if(x == 0 || x == 11 || y == 0) {
                        board[x][y] = 1;
                    } else {
                        board[x][y] = 0;
                    }
                }
            }
            current.x = 5;
            current.y = 21;
            current.type = random(7) + 1;
            current.rotate = random(4);
            putBlock(current);
            HDC hdc = GetDC(hWnd);
            hMemDC = CreateCompatibleDC(hdc);
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 24 * 10, 24 * 20);
            hMemPrev = (HBITMAP)SelectObject(hMemDC, hBitmap);
            hBlockDC = CreateCompatibleDC(hdc);
            hBitmap = LoadBitmap(hInstance, "BLOCKS");
            hBlockPrev = (HBITMAP)SelectObject(hBlockDC, hBitmap);
            // debug
            BitBlt(hMemDC, 0, 0, 24, 24, hBlockDC, 0, 0, SRCCOPY);
            ReleaseDC(hWnd, hdc);
            break;
        }
        case WM_TIMER: {
            static int w = 0;
            if(w % 2 == 0) {
                if(processInput()) {
                    w = 0;
                }
            }
            if(w % 5 == 0) {
                blockDown();
            }
            w++;
            
            InvalidateRect(hWnd, NULL, false);
            break;
        }
        case WM_PAINT: {
            showBoard();
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            BitBlt(hdc, 0, 0, 24 * 10, 24 * 20, hMemDC, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_DESTROY: {
            HBITMAP hBitmap = (HBITMAP)SelectObject(hMemDC, hMemPrev);
            DeleteObject(hBitmap);
            DeleteObject(hMemDC);
            hBitmap = (HBITMAP)SelectObject(hBlockDC, hBlockPrev);
            DeleteObject(hBitmap);
            DeleteObject(hBlockDC);
            
            PostQuitMessage ( 0 );
            break;
        }
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {
    
    //handle to an Instance
    hInstance = hInst;

    // To register a window class
    WNDCLASSEX wc;

    // defining the properties of the window class
    static LPCTSTR pszClassName = "Effortless Tetris @Akashomatics" ;   
    wc.hInstance      = hInst;    
    wc.hIcon          = NULL ;     
    wc.hIconSm        = NULL ;   
    wc.hCursor        = LoadCursor ( NULL , IDC_ARROW);                                                                                     
    wc.cbSize         = sizeof (WNDCLASSEX);      
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1 );                    
    wc.style          = CS_HREDRAW | CS_VREDRAW;           
    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.lpszClassName = pszClassName;                        
    wc.cbClsExtra     = 0 ;                                 
    wc.cbWndExtra     = 0 ;                                 
    wc.lpszMenuName   = NULL ;                             
    if (! RegisterClassEx (& wc)) return  FALSE ;              

    // creating a rectangle
    RECT rect;
    rect.left = rect.top = 0;
    rect.right = 24 * 10;
    rect.bottom = 24 * 20;
    AdjustWindowRectEx(&rect, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION, false, 0);

    // creating the window
    hMainWindow = CreateWindow(pszClassName, "Effortless Tetris @Akashomatics", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, hInst, NULL);
    
    // display the window
    ShowWindow(hMainWindow, SW_SHOW);

    // Timer 
    SetTimer (hMainWindow , 100 , 1000/30 , NULL ) ;
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // end game
    KillTimer(hMainWindow, 100);

    return 0;
}