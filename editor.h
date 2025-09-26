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
        char c;                
        struct CharNode *prev;
        struct CharNode *next;
    } CharNode;

    typedef struct RowNode
    {
        struct CharNode *str;
        struct RowNode *prev;
        struct RowNode *next;
        int num_char;
    } RowNode;

    typedef struct Editor
    {
        struct RowNode *print_head;
        struct RowNode *node_row;
        struct CharNode *node_col;
        int node_y;
        int node_x;
        int num_lines;
        int cursor_row;
        int cursor_col;
        int right_move_distance;
        int console_height; 
        int console_width; 
        int is_saved;
        char *filename; 
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