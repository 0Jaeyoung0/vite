#ifndef EDITOR_H
    #define EDITOR_H

    #define UNDEF_CH 1000
    #define ENTER 1001
    #define BACKSPACE 1002
    #define LEFTARROW 1003
    #define RIGHTARROW 1004
    #define UPARROW 1005
    #define DOWNARROW 1006
    #define HOME 1007
    #define END 1008
    #define PGUP 1009
    #define PGDN 1010
    #define CTRL_F 1011
    #define CTRL_Q 1012
    #define CTRL_S 1013
    #define ESC 1014

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #ifdef _WIN32
        #include <windows.h>
        #include <conio.h>
        #define CLEAR "cls"
        #define GETCH() _getch()
    #else
        #include <sys/ioctl.h>
        #include <termios.h>
        #include <unistd.h>
        #define CLEAR "clear"
        #define GETCH() getchar()
    #endif

    typedef struct CharNode
    {
        char c;                //입력된 문자를 저장하는 변수
        struct CharNode *prev; //이전 문자 노드를 가리키는 포인터
        struct CharNode *next; //다음 문자 노드를 가리키는 포인터
    } CharNode;

    typedef struct RowNode
    {
        struct CharNode *str; //문자열의 첫 문자 노드를 가리키는 포인터
        struct RowNode *prev; //이전 행 노드를 가리키는 포인터
        struct RowNode *next; //다음 행 노드를 가리키는 포인터
        int num_char;         //행에 저장된 문자의 개수
    } RowNode;

    typedef struct Editor
    {
        struct RowNode *print_head; //화면의 출력범위를 지정하기 위한 포인터
        struct RowNode *node_row;   //커서가 가리키는 행 노드의 포인터
        struct CharNode *node_col;  //커서가 가리키는 문자 노드의 포인터
        int node_y;                 //데이터 상의 포인터의 y좌표값
        int node_x;                 //데이터 상의 포인터의 x좌표값
        int num_lines;              //전체 라인 수를 저장하는 변수
        int cursor_row;             //화면상의 커서의 현재 y좌표값을 저장하는 변수
        int cursor_col;             //화면상의 커서의 현재 x좌표값을 저장하는 변수
        int right_move_distance;    //커서가 오른쪽으로 이동할 거리를 저장하는 변수
        int console_height;         //실행된 콘솔창의 높이를 저장하는 변수
        int console_width;          //실행된 콘솔창의 넓이를 저장하는 변수
        int is_saved;               //에디터에 저장상태를 나타내는 변수
        char *filename;             //현재 열려있는 파일의 이름을 저장하는 변수
    } Editor;

    CharNode* GetNewCharNode(char c);
    RowNode*  GetNewRowNode();
    int       ReadKey();
    int       GetConsoleColumns();
    int       GetConsoleRows();
    int       CountDigits(int number);
    void      Open_File(Editor *editor, int argc, char *argv);
    void      SaveFile(Editor *editor);
    void      InitEditor(Editor *editor);
    void      InitDisplay(Editor *editor);
    void      InsertChar(Editor *editor, char c);
    void      DeleteChar(Editor *editor);
    void      InsertRow(Editor *editor);
    void      DeleteRow(Editor *editor);
    void      SplitLine(Editor *editor);
    void      MergeLine(Editor *editor);
    void      MovePointerLeft(Editor *editor);
    void      MovePointerRight(Editor *editor);
    void      MovePointerUp(Editor *editor);
    void      MovePointerDown(Editor *editor);
    void      MovePointerHome(Editor *editor);
    void      MovePointerEnd(Editor *editor);
    void      MovePointerPgUp(Editor *editor);
    void      MovePointerPgDn(Editor *editor);
    void      ExitEditor(Editor *editor);
    void      Search(Editor *editor);
    int       IsEquals(CharNode *node_col, CharNode *str);

    void      MoveCursor(Editor *editor);
    void      PrintText(Editor *editor, RowNode *print_head, int row);
    void      PrintRowText(CharNode *temp);
    void      ClearText(Editor *editor, int row);
    void      PrintStatusBar(Editor *editor);
    void      PrintMessageBar(Editor *editor, char *string);

    void      EditorKeyProcess(Editor *editor);
#endif