#include "editor.h"

int main(int argc, char *argv[])
{
    Editor editor;
    InitEditor(&editor);
    Open_File(&editor, argc, argv[1]);
    InitDisplay(&editor);

    while(1)
        EditorKeyProcess(&editor);

    return 0;
}