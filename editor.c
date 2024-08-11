#include "editor.h"

int ReadKey()
{
    #ifdef _WIN32
        int c = GETCH();
        if(c >= 32 && c <= 126)
            return c;
        else if(c == 13)
            return ENTER;
        else if(c == 8)
            return BACKSPACE;
        else if(c == 224)
        {
            c = GETCH();
            switch(c)
            {
                case 75:
                    return LEFTARROW;
                case 77:
                    return RIGHTARROW;
                case 72:
                    return UPARROW;
                case 80:
                    return DOWNARROW;
                default:
                    return UNDEF_CH;
            }
        }
        else if(c == 0)
        {
            c = GETCH();
            switch(c)
            {
                case 71:
                    return HOME;
                case 79:
                    return END;
                case 73:
                    return PGUP;
                case 81:
                    return PGDN;
                default:
                    return UNDEF_CH;
            }
        }
        else if(c == 27)
            return ESC;
        else if(c == 6)
            return CTRL_F;
        else if(c == 17)
            return CTRL_Q;
        else if(c == 19)
            return CTRL_S;
        else
            return UNDEF_CH;
    #else
        int c = GETCH();
        if(c >= 32 && c <= 126)
            return c;
        else if(c == 10)
            return ENTER;
        else if(c == 127)
            return BACKSPACE;
        else if(c == '\033')
        {
            c = GETCH();
            if(c == '[')
            {
                c = GETCH();
                switch(c)
                {
                    case 'D':
                        return LEFTARROW;
                    case 'C':
                        return RIGHTARROW;
                    case 'A':
                        return UPARROW;
                    case 'B':
                        return DOWNARROW;
                    case 'H':
                        return HOME;
                    case 'F':
                        return END;
                    case '1':
                    {
                        c = GETCH();
                        if(c == '~')
                            return HOME;
                        else
                            return UNDEF_CH;
                    }
                    case '4':
                    {
                        c = GETCH();
                        if(c == '~')
                            return END;
                        else
                            return UNDEF_CH;
                    }
                    case '5':
                    {
                        c = GETCH();
                        if(c == '~')
                            return PGUP;
                        else
                            return UNDEF_CH;
                    }
                    case '6':
                    {
                        c = GETCH();
                        if(c == '~')
                            return PGDN;
                        else
                            return UNDEF_CH;
                    }
                    case '7':
                    {
                        c = GETCH();
                        if(c == '~')
                            return HOME;
                        else
                            return UNDEF_CH;
                    }
                    case '8':
                    {
                        c = GETCH();
                        if(c == '~')
                            return END;
                        else
                            return UNDEF_CH;
                    }
                    default :
                        return UNDEF_CH;
                }
            }
            else if(c == 'O')
            {
                c = GETCH();
                switch(c)
                {
                    case 'H':
                        return HOME;
                    case 'F':
                        return END;
                    default :
                        return UNDEF_CH;
                }
            }
            else
                return ESC;
        }
        else if(c == 6)
            return CTRL_F;
        else if(c == 17)
            return CTRL_Q;
        else if(c == 19)
            return CTRL_S;
        else
            return UNDEF_CH;
    #endif
}

CharNode* GetNewCharNode(char c)
{
    CharNode* temp = (CharNode *)malloc(sizeof(CharNode));
    temp -> c = c;
    temp -> prev = NULL;
    temp -> next = NULL;

    return temp;
}

RowNode* GetNewRowNode()
{
    RowNode* temp = (RowNode *)malloc(sizeof(RowNode));
    temp -> str = NULL;
    temp -> prev = NULL;
    temp -> next = NULL;
    temp -> num_char = 0;

    return temp;
}

void InitEditor(Editor *editor)
{
    editor -> node_row = GetNewRowNode();
    editor -> node_col = NULL;
    editor -> node_y = 1;
    editor -> node_x = 1;
    editor -> num_lines = 1;
    editor -> cursor_row = 1;
    editor -> cursor_col = 1;
    editor -> right_move_distance = editor -> cursor_col;
    editor -> console_height = GetConsoleRows();
    editor -> console_width = GetConsoleColumns();
    editor -> filename = NULL;
    editor -> print_head = editor -> node_row;
    editor -> is_saved = 1;

    #ifndef _WIN32
    struct termios newt;
    tcgetattr(STDIN_FILENO, &newt);
    
    newt.c_lflag &= ~(ICANON | ECHO | IEXTEN | ISIG);
    newt.c_iflag &= ~(IXON | IXOFF);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    #endif
}

void InitDisplay(Editor *editor)
{
    system(CLEAR);
    PrintText(editor, editor -> print_head, 1);
    PrintStatusBar(editor);
    PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
    MoveCursor(editor);
}

void Open_File(Editor *editor, int argc, char *argv)
{
    if (argc > 2)
    {
        fputs("(HELP)\n\nRun:                vite\nOpen existing file: vite <filename>\n", stderr);
        exit(1);
    }
    else if(argc == 1)
        return;
    
    FILE *fp = fopen(argv, "r");

    if(fp == NULL)//������ �������� �ʴ´ٸ�
    {
        fputs("File does not exist.\n", stderr);
        exit(1);
    }

    editor -> filename = argv;//���� ���� ������ �̸��� ����

    char fpc;
    while((fpc = fgetc(fp)) != EOF)//������ ���ڵ��� ����Ʈ�� ����
    {
        if(fpc == 10)
        {
            InsertRow(editor);
            SplitLine(editor);
            if(editor -> cursor_row > editor -> console_height - 2)
            {
                editor -> print_head = editor -> print_head -> next;
                editor -> cursor_row--;
            }
        }
        else
        {
            if(editor -> cursor_col > editor -> console_width - 1)
            {
                InsertRow(editor);
                SplitLine(editor);
                if(editor -> cursor_row > editor -> console_height - 2)
                {
                    editor -> print_head = editor -> print_head -> next;
                    editor -> cursor_row--;
                }
            }
            InsertChar(editor, fpc);
        }
    }

    fclose(fp);
}

void SaveFile(Editor *editor)
{
    /*���� �̸� �Է�*/
    if(editor -> filename == NULL)
    {
        PrintMessageBar(editor, "Filename (Enter/Ctrl-q):");

        Editor temp_editor;
        InitEditor(&temp_editor);
        int length = strlen("Filename (Enter/Ctrl-q):");

        int cc;
        while(1)
        {
            cc = ReadKey();
            if((cc >= 32 && cc <= 126) && cc != 47)// '/'�� ������ ����
            {
                if(temp_editor.node_row -> num_char >= (editor -> console_width - length - 1))
                    continue;
                InsertChar(&temp_editor, cc);

                printf("\033[K");

                PrintRowText(temp_editor.node_col);
            }
            else if(cc == BACKSPACE)
            {
                DeleteChar(&temp_editor);

                printf("\033[%d;%dH", editor -> console_height, temp_editor.cursor_col + length);
                printf("\033[K");

                CharNode *temp;
                if(temp_editor.node_col == NULL)
                    temp = temp_editor.node_row -> str;
                else
                    temp = temp_editor.node_col -> next;
                PrintRowText(temp);
            }
            else if(cc == LEFTARROW)
            {
                MovePointerLeft(&temp_editor);
            }
            else if(cc == RIGHTARROW)
            {
                MovePointerRight(&temp_editor);
            }
            else if(cc == CTRL_Q)
            {
                PrintMessageBar(editor, "File has not been saved");
                MoveCursor(editor);
                return;
            }
            else if(cc == ENTER)
            {
                if(temp_editor.node_row -> str == NULL)//�ƹ� ���ڵ� �Է����� ������ ����
                {
                    PrintMessageBar(editor, "File has not been saved");
                    MoveCursor(editor);
                    return;
                }
                
                char* filename = (char *)malloc(sizeof(char) * temp_editor.node_row -> num_char + 1);
                CharNode* temp = temp_editor.node_row -> str;

                int i;
                for(i = 0; i < temp_editor.node_row -> num_char; i++)
                {
                    filename[i] = temp -> c;
                    temp = temp -> next;
                }
                filename[i] = '\0';

                editor -> filename = filename;
                break;
            }
            else
                continue;

            printf("\033[%d;%dH", editor -> console_height, temp_editor.cursor_col + length);
        }
    }

    /*���� ���� ����*/
    FILE *fp = fopen(editor -> filename, "w");

    RowNode *temp1 = editor -> print_head;
    CharNode *temp2;

    while (temp1 -> prev != NULL)
        temp1 = temp1 -> prev;
    
    while(temp1 != NULL)
    {
        temp2 = temp1 -> str;
        while(temp2 != NULL)
        {
            fputc(temp2 -> c, fp);
            temp2 = temp2 -> next;
        }
        fputc('\n', fp);
        temp1 = temp1 -> next;
    }

    editor -> is_saved = 1;

    fclose(fp);

    PrintStatusBar(editor);
    PrintMessageBar(editor, "File has saved");
    MoveCursor(editor);
}

void InsertChar(Editor *editor, char c)
{
    CharNode *temp = GetNewCharNode(c);//�� ���� ��带 ����

    if(editor -> node_row -> str == NULL)//���� ������� ���
    {
        editor -> node_row -> str = temp;
    }
    else if(editor -> node_col == NULL)//Ŀ���� �� �տ� ���� ���
    {
        temp -> next = editor -> node_row -> str;
        editor -> node_row -> str -> prev = temp;
        editor -> node_row -> str = temp;
    }
    else if(editor -> node_col -> next == NULL)//Ŀ���� �� �ڿ� ���� ���
    {
        temp -> prev = editor -> node_col;
        editor -> node_col -> next = temp;
    }
    else//Ŀ���� �߰��� ���� ���
    {
        temp -> next = editor -> node_col -> next;
        temp -> prev = editor -> node_col;
        editor -> node_col -> next -> prev = temp;
        editor -> node_col -> next = temp;
    }

    editor -> node_col = temp;         //������ �̵�
    editor -> node_x++;                //������ ��ǥ�� ����
    editor -> cursor_col++;            //Ŀ�� ��ǥ�� ����
    editor -> node_row -> num_char++;  //���� �� ����
    editor -> right_move_distance = editor -> cursor_col;
    editor -> is_saved = 0;            //������� ����
}

void DeleteChar(Editor *editor)
{
    CharNode *temp = editor -> node_col;//Ŀ���� ����Ű�� �ִ� ���� ����� �ּ�

    if(editor -> node_col == NULL)//Ŀ���� �� �տ� ���� ���
        return;
    else if(editor -> node_col == editor -> node_row -> str)//ù��° ���ڸ� ������ ���
    {
        if(editor -> node_col -> next == NULL)
            editor -> node_row -> str = NULL;
        else
        {
            editor -> node_row -> str = temp -> next;
            temp -> next -> prev = NULL;
        }
        editor -> node_col = NULL;//������ �̵�
    }
    else if(editor -> node_col -> next == NULL)//Ŀ���� �� �ڿ� ���� ���
    {
        editor -> node_col = editor -> node_col -> prev;//������ �̵�
        temp -> prev -> next = NULL;
    }
    else//Ŀ���� �߰��� ���� ���
    {
        editor -> node_col = editor -> node_col -> prev;//������ �̵�
        temp -> next -> prev = temp -> prev;
        temp -> prev -> next = temp -> next;
    }

    free(temp);
    editor -> node_x--;               //������ ��ǥ�� ����
    editor -> cursor_col--;           //Ŀ�� ��ǥ�� ����
    editor -> node_row -> num_char--; //���� �� ����
    editor -> right_move_distance = editor -> cursor_col;
    editor -> is_saved = 0;           //������� ����
}

void InsertRow(Editor *editor)
{
    RowNode *temp = GetNewRowNode();//�� �� ��带 ����

    if(editor -> node_row -> next == NULL)//������ ���� ���
    {
        temp -> prev = editor -> node_row;
        editor -> node_row -> next = temp;
    }
    else
    {
        temp -> next = editor -> node_row -> next;
        temp -> prev = editor -> node_row;
        editor -> node_row -> next -> prev = temp;
        editor -> node_row -> next = temp;
    }

    editor -> node_row = editor -> node_row -> next;//������ �̵�
    editor -> node_y++;                             //������ ��ǥ�� ����
    editor -> cursor_row++;                         //Ŀ�� ��ǥ�� ����
    editor -> num_lines++;                          //��ü ���� �� ����
    editor -> is_saved = 0;                         //������� ����
}

void DeleteRow(Editor *editor)
{
    RowNode *temp = editor -> node_row;//Ŀ���� ����Ű�� �ִ� �� ����� �ּ�

    if(editor -> node_row -> prev == NULL)//ù��° ���� ���
        return;
    else if(editor -> node_row -> next == NULL)//������ ���� ���
    {
        editor -> node_row = editor -> node_row -> prev;//������ �̵�
        temp -> prev -> next = NULL;
    }
    else
    {
        editor -> node_row = editor -> node_row -> prev;//������ �̵�
        temp -> next -> prev = temp -> prev;
        temp -> prev -> next = temp -> next;
    }

    free(temp);
    editor -> node_y--;     //������ ��ǥ�� ����
    editor -> cursor_row--; //Ŀ�� ��ǥ�� ����
    editor -> num_lines--;  //��ü ���� �� ����
    editor -> is_saved = 0; //������� ����
}

void SplitLine(Editor *editor)
{
    int n = editor -> cursor_col;

    if(editor -> node_col == NULL)//Ŀ���� �� �տ� ���� ���
    {
        editor -> node_row -> str = editor -> node_row -> prev -> str;
        editor -> node_row -> prev -> str = NULL;
    }
    else if(editor -> node_col -> next == NULL)//Ŀ���� �� �ڿ� ���� ���
    {
        editor -> node_col = NULL;
        editor -> node_x = 1;
        editor -> cursor_col = 1;
    }
    else//Ŀ���� �߰��� ���� ���
    {
        editor -> node_row -> str = editor -> node_col -> next;
        editor -> node_col -> next -> prev = NULL;
        editor -> node_col -> next = NULL;
        editor -> node_col = NULL;
        editor -> node_x = 1;
        editor -> cursor_col = 1;
    }

    editor -> right_move_distance = editor -> cursor_col;
    //num_char ������Ʈ
    editor -> node_row -> num_char = editor -> node_row -> prev -> num_char - (n - 1);
    editor -> node_row -> prev -> num_char = n - 1;
}

void MergeLine(Editor *editor)
{
    if(editor -> node_row -> prev == NULL)//���� ���� ù��° ���� ���
        return;
    else if(editor -> node_row -> prev -> str == NULL)//���� ���� ����ִ� ���
    {
        editor -> node_col = NULL;
        editor -> node_x = 1;
        editor -> cursor_col = 1;

        if(editor -> node_row -> str != NULL)//���� ���� ����ִ� ���
            editor -> node_row -> prev -> str = editor -> node_row -> str;
    }
    else
    {
        editor -> node_col = editor -> node_row -> prev -> str;
        editor -> node_x = 2;
        editor -> cursor_col = 2;
        while(editor -> node_col -> next != NULL)
        {
            editor -> node_col = editor -> node_col -> next;
            editor -> node_x++;
            editor -> cursor_col++;
        }
        
        if(editor -> node_row -> str != NULL)//���� ���� ����ִ� ���
        {
            editor -> node_col -> next = editor -> node_row -> str;
            editor -> node_col -> next -> prev = editor -> node_col;
            editor -> node_row -> str = NULL;
        }
    }

    editor -> right_move_distance = editor -> cursor_col;
    //num_char ������Ʈ
    editor -> node_row -> prev -> num_char += editor -> node_row -> num_char;
}

void MovePointerLeft(Editor *editor)
{
    if(editor -> node_col == NULL)//Ŀ���� �� ���� ���
    {
        if(editor -> node_row -> prev == NULL)//���� ��ġ�� ù��° ���� ���
            return;
        else if(editor -> node_row -> prev -> str == NULL)//�� ���� ����� ���
        {
            editor -> node_row = editor -> node_row -> prev;
            editor -> node_y--;
            editor -> cursor_row--;
        }
        else
        {
            /*�� ���� �� ������ �̵�*/
            editor -> node_row = editor -> node_row -> prev;
            editor -> node_y--;
            editor -> cursor_row--;
            editor -> node_col = editor -> node_row -> str;
            editor -> node_x = 2;
            editor -> cursor_col = 2;

            while(editor -> node_col -> next != NULL)
            {
                editor -> node_col = editor -> node_col -> next;
                editor -> node_x++;
                editor -> cursor_col++;
            }
        }
    }
    else
    {
        editor -> node_x--;
        editor -> cursor_col--;
        editor -> node_col = editor -> node_col -> prev;
    }

    editor -> right_move_distance = editor -> cursor_col;
}

void MovePointerRight(Editor *editor)
{
    if(editor -> node_row -> str != NULL && editor -> node_col == NULL)//Ŀ���� �� ���� ���
    {
        editor -> node_x++;
        editor -> cursor_col++;
        editor -> node_col = editor -> node_row -> str;
    }
    else if(editor -> node_row -> str == NULL || editor -> node_col -> next == NULL)//���� ����ų� Ŀ���� �� �ڿ� �ִ� ���
    {
        if(editor -> node_row -> next == NULL)//������ ���� ���
            return;
        else
        {
            /*���� ���� �� ������ �̵�*/
            editor -> node_row = editor -> node_row -> next;
            editor -> node_y++;
            editor -> cursor_row++;
            editor -> node_col = NULL;
            editor -> node_x = 1;
            editor -> cursor_col = 1;
        }
    }
    else
    {
        editor -> node_x++;
        editor -> cursor_col++;
        editor -> node_col = editor -> node_col -> next;
    }

    editor -> right_move_distance = editor -> cursor_col;
}

void MovePointerUp(Editor *editor)
{
    if(editor -> node_row -> prev == NULL)//�� ���� ���� ���
        return;

    /*�� ���� �� ������ �̵�*/
    editor -> node_row = editor -> node_row -> prev;
    editor -> node_y--;
    editor -> cursor_row--;

    /*right_move_distance��ŭ ���������� �̵�*/
    if(editor -> node_row -> str == NULL || editor -> right_move_distance == 1)
    {
        editor -> node_col = NULL;
        editor -> node_x = 1;
        editor -> cursor_col = 1;
    }
    else
    {
        editor -> node_col = editor -> node_row -> str;
        editor -> node_x = 2;
        editor -> cursor_col = 2;

        while(editor -> cursor_col <= editor -> right_move_distance - 1)
        {
            if(editor -> node_col -> next == NULL)
                break;
            editor -> node_col = editor -> node_col -> next;
            editor -> node_x++;
            editor -> cursor_col++;
        }
    }
}

void MovePointerDown(Editor *editor)
{
    if(editor -> node_row -> next == NULL)
        return;

    /*���� ���� �� ������ �̵�*/
    editor -> node_row = editor -> node_row -> next;
    editor -> node_y++;
    editor -> cursor_row++;

    /*right_move_distance��ŭ ���������� �̵�*/
    if(editor -> node_row -> str == NULL || editor -> right_move_distance == 1)
    {
        editor -> node_col = NULL;
        editor -> node_x = 1;
        editor -> cursor_col = 1;
    }
    else
    {
        editor -> node_col = editor -> node_row -> str;
        editor -> node_x = 2;
        editor -> cursor_col = 2;

        while(editor -> cursor_col <= editor -> right_move_distance - 1)
        {
            if(editor -> node_col -> next == NULL)
                break;
            editor -> node_col = editor -> node_col -> next;
            editor -> node_x++;
            editor -> cursor_col++;
        }
    }
}

void MovePointerHome(Editor *editor)
{
    editor -> node_col = NULL;
    editor -> node_x = 1;
    editor -> cursor_col = 1;
    editor -> right_move_distance = editor -> cursor_col;
}

void MovePointerEnd(Editor *editor)
{
    if(editor -> node_row -> str == NULL)
        return;

    if(editor -> node_col == NULL)
    {
        editor -> node_col = editor -> node_row -> str;
        editor -> node_x = 2;
        editor -> cursor_col = 2;
    }

    while(editor -> node_col -> next != NULL)
    {
        editor -> node_col = editor -> node_col -> next;
        editor -> node_x++;
        editor -> cursor_col++;
    }

    editor -> right_move_distance = editor -> cursor_col;
}

void MovePointerPgUp(Editor *editor)
{
    if(editor -> cursor_row == 1)
    {
        /*���� ������ŭ print_head������ �̵�*/
        int i;
        for(i = 0; i < (editor -> console_height - 4); i++)
        {
            MovePointerUp(editor);
            if(editor -> cursor_row < 1 && editor -> print_head -> prev != NULL)
            {
                editor -> print_head = editor -> print_head -> prev;
                editor -> cursor_row++;
            }
        }
        ClearText(editor, 1);
        PrintText(editor, editor -> print_head, 1);
    }
    else
    {
        /*ù��° ������ �̵�*/
        while(editor -> cursor_row != 1)
            MovePointerUp(editor);
    }
}

void MovePointerPgDn(Editor *editor)
{
    if(editor -> cursor_row == editor -> console_height - 2)
    {
        int i;
        for(i = 0; i < (editor -> console_height - 4); i++)
        {
            MovePointerDown(editor);
            if(editor -> cursor_row > editor -> console_height - 2)
            {
                editor -> print_head = editor -> print_head -> next;
                editor -> cursor_row--;
            }
        }
        
        ClearText(editor, 1);
        PrintText(editor, editor -> print_head, 1);
    }
    else
    {
        while(editor -> cursor_row != editor -> console_height - 2)
        {
            if(editor -> node_row -> next == NULL)
                break;
            MovePointerDown(editor);
        }
    }
}

void ExitEditor(Editor *editor)
{
    if(editor -> is_saved == 0)//������� ���� ����
    {
        PrintMessageBar(editor, "!!WARNING!! File has not been saved. Ctrl-q to quit without saving.");

        int c = ReadKey();

        if(c == CTRL_S)
        {
            SaveFile(editor);
            return;
        }
        else if(c == CTRL_Q)
        {
            system(CLEAR);
            exit(0);
        }
        else
        {
            /*�����Ͱ� ���� ���·� ���ư���.*/
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
            return;
        }
    }
    else
    {
        system(CLEAR);
        exit(0);
    }
}

void Search(Editor *editor)
{
    PrintMessageBar(editor, "Search (Enter/Ctrl-q):");

    Editor temp_editor;
    InitEditor(&temp_editor);
    int length = strlen("Search (Enter/Ctrl-q):");

    /*      ���ڿ� �Է�       */
    int cc;
    while(1)
    {
        cc = ReadKey();
        if(cc >= 32 && cc <= 126)
        {
            if(temp_editor.node_row -> num_char >= editor -> console_width - length - 1)
                continue;
            InsertChar(&temp_editor, cc);

            printf("\033[K");

            PrintRowText(temp_editor.node_col);
        }
        else if(cc == BACKSPACE)
        {
            DeleteChar(&temp_editor);

            printf("\033[%d;%dH", editor -> console_height, temp_editor.cursor_col + length);
            printf("\033[K");

            CharNode *temp;
            if(temp_editor.node_col == NULL)
                temp = temp_editor.node_row -> str;
            else
                temp = temp_editor.node_col -> next;
            PrintRowText(temp);
        }
        else if(cc == LEFTARROW)
        {
            MovePointerLeft(&temp_editor);
        }
        else if(cc == RIGHTARROW)
        {
            MovePointerRight(&temp_editor);
        }
        else if(cc == CTRL_Q)
        {
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
            return;
        }
        else if(cc == ENTER)
        {
            if(temp_editor.node_row -> str == NULL)//�ƹ� ���ڵ� �Է����� ������ �׳� ����
            {
                PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
                MoveCursor(editor);
                return;
            }
            break;
        }
        else
            continue;

        printf("\033[%d;%dH", editor -> console_height, temp_editor.cursor_col + length);
    }

    Editor origin_editor = *editor;//Ž���� �����ϱ� ���� ������� ����

    while(editor -> print_head -> prev != NULL)//�ؽ�Ʈ�� �� ó������ �̵�
        editor -> print_head = editor -> print_head -> prev;
    editor -> node_row = editor -> print_head;
    editor -> node_col = NULL;
    editor -> node_y = 1;
    editor -> node_x = 1;
    editor -> cursor_row = 1;
    editor -> cursor_col = 1;

    /*      ���ڿ� Ž��       */
    int count = 0;
    Editor *editor_state = (Editor *)malloc(sizeof(Editor) * 100);//�������� ������¸� �����ϱ� ���� �迭
    while(1)
    {
        MovePointerRight(editor);
        if(editor -> cursor_row > (editor -> console_height / 2))
        {
            editor -> print_head = editor -> print_head -> next;
            editor -> cursor_row--;
        }

        if(editor -> node_col != NULL)
        {
            if(editor -> node_col -> c == temp_editor.node_row -> str -> c)
            {
                if(IsEquals(editor -> node_col, temp_editor.node_row -> str))
                {
                    editor_state[count] = *editor;
                    count++;

                    if(count % 100 == 0)
                        editor_state = (Editor *)realloc(editor_state, sizeof(Editor) * (count + 100));
                }
            }
        }

        if(editor -> node_row -> next == NULL)
        {
            if(editor -> node_col == NULL)
            {
                if(editor -> node_row -> str == NULL)
                    break;
            }
            else 
            {
                if(editor -> node_col -> next == NULL)
                    break;
            }
        }
    }

    if(count != 0)
    {
        PrintMessageBar(editor, "Help: Enter = edit | Arrow = move | Ctrl-q = return");

        *editor = editor_state[0];

        MovePointerLeft(editor);

        ClearText(editor, 1);

        PrintText(editor, editor -> print_head, 1);
        PrintStatusBar(editor);
        MoveCursor(editor);

        printf("\033[7m");//������
        PrintRowText(temp_editor.node_row -> str);
        printf("\033[0m\n");//��Ÿ�� �ʱ�ȭ
        MoveCursor(editor);
    }
    else
    {
        PrintMessageBar(editor, "No results found");
        *editor = origin_editor;
        MoveCursor(editor);
        return;
    }

    /*      Ű �Է� �� �۾� ����       */
    int max_count = count;
    count = 0;
    while(1)
    {
        cc = ReadKey();
        if(cc == LEFTARROW || cc == UPARROW)
        {
            if(count == 0)
            {
                count = max_count - 1;
                *editor = editor_state[count];
            }
            else
            {
                count--;
                *editor = editor_state[count];
            }
        }
        else if(cc == RIGHTARROW || cc == DOWNARROW)
        {
            if(count == max_count - 1)
            {
                count = 0;
                *editor = editor_state[count];
            }
            else
            {
                count++;
                *editor = editor_state[count];
            }
        }
        else if(cc == ENTER)
        {
            ClearText(editor, 1);

            PrintText(editor, editor -> print_head, 1);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);

            free(editor_state);
            return;
        }
        else if(cc == CTRL_Q)
        {
            *editor = origin_editor;//Ž�� ���� ���·� �̵�

            ClearText(editor, 1);

            PrintText(editor, editor -> print_head, 1);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);

            free(editor_state);
            return;
        }
        else
            continue;

        MovePointerLeft(editor);

        ClearText(editor, 1);

        PrintText(editor, editor -> print_head, 1);
        PrintStatusBar(editor);
        MoveCursor(editor);

        printf("\033[7m");//������
        PrintRowText(temp_editor.node_row -> str);
        printf("\033[0m");//��Ÿ�� �ʱ�ȭ
        MoveCursor(editor);
    }
}

int IsEquals(CharNode *node_col, CharNode *str)
{
    while(str != NULL)
    {
        if(node_col == NULL || (str -> c != node_col -> c))
            return 0;
        
        str = str -> next;
        node_col = node_col -> next;
    }

    return 1;
}

void MoveCursor(Editor *editor)
{
    printf("\033[%d;%dH", editor -> cursor_row, editor -> cursor_col);
}

void PrintText(Editor *editor, RowNode *print_head, int row)//Ư�� ����� ��ü ���
{
    printf("\033[%d;1H", row);
    CharNode *temp;
    
    int i;
    for(i = row; i <= editor -> console_height - 2; i++)
    {
        if(print_head == NULL)
        {
            printf("~\n");
            continue;
        }

        for(temp = print_head -> str; temp != NULL; temp = temp -> next)
        {
            printf("%c", temp -> c);
        }
        printf("\n");
        print_head = print_head -> next;
    }
}

void PrintRowText(CharNode *temp)//Ư�� ������ ���
{
    while(temp != NULL)
    {
        printf("%c", temp -> c);
        temp = temp -> next;
    }
}

void ClearText(Editor *editor, int row)//Ư�� ����� ��ü �����
{
    int i;
    for(i = row; i <= editor -> console_height - 2; i++)
    {
        printf("\033[%d;1H", i);
        printf("\033[K");
    }
}

void PrintStatusBar(Editor *editor)
{
    char *statusbar = (char *)malloc(editor -> console_width + 1);
    int remaining_space;

    if(editor -> filename == NULL)
    {
        remaining_space = editor -> console_width - (strlen("[No Name] -  linesno ft | /") + CountDigits(editor -> num_lines) + CountDigits(editor -> node_y) + CountDigits(editor -> node_x));
        snprintf(statusbar, editor -> console_width + 1, "[No Name] - %d lines%*sno ft | %d/%d", editor -> num_lines, remaining_space, "", editor -> node_y, editor -> node_x);
    }
    else
    {
        remaining_space = editor -> console_width - (strlen(editor -> filename) + strlen("[] -  linesno ft | /") + CountDigits(editor -> num_lines) + CountDigits(editor -> node_y) + CountDigits(editor -> node_x));
        snprintf(statusbar, editor -> console_width + 1, "[%s] - %d lines%*sno ft | %d/%d", editor -> filename, editor -> num_lines, remaining_space, "", editor -> node_y, editor -> node_x);
    }

    printf("\033[%d;1H", editor -> console_height - 1);//���������� �ι�° �ٷ� �̵�
    printf("\033[7m");//������
    printf("%s", statusbar);
    printf("\033[0m");//��Ÿ�� �ʱ�ȭ
    free(statusbar);
}

void PrintMessageBar(Editor *editor, char *string)
{
    printf("\033[%d;1H", editor -> console_height);
    printf("\033[K");//���� Ŀ����ġ���� �� �� �����
    printf("%s", string);
}

int GetConsoleColumns()
{
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
        int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

        return columns;
    #else
        struct winsize size;
        ioctl(0, TIOCGWINSZ, &size);

        int columns = size.ws_col;

        return columns;
    #endif
}

int GetConsoleRows()
{
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    
        int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        return rows;
    #else
        struct winsize size;
        ioctl(0, TIOCGWINSZ, &size);

        int rows = size.ws_row;

        return rows;
    #endif
}

int CountDigits(int number) 
{
    int count = 0;
    while (number != 0)
    {
        number /= 10;
        ++count;
    }
    
    if (count == 0) 
        count = 1;

    return count;
}

void EditorKeyProcess(Editor *editor)
{
    int c = ReadKey();

    if(c >= 32 && c <= 126)//���ڿ� �����̽���
    {
        //
        if(editor -> cursor_col >= editor -> console_width)
        {
            InsertRow(editor);
            SplitLine(editor);
            if(editor -> cursor_row > editor -> console_height - 2)
            {
                editor -> print_head = editor -> print_head -> next;
                editor -> cursor_row--;

                ClearText(editor, 1);

                PrintText(editor, editor -> print_head, 1);
                PrintStatusBar(editor);
                PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
                MoveCursor(editor);
                return;
            }

            ClearText(editor, editor -> cursor_row - 1);

            PrintText(editor, editor -> node_row -> prev, editor -> cursor_row - 1);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
        }
        else
        {
            if(editor -> node_row -> num_char >= editor -> console_width - 1)
                return;
        }
        //
        InsertChar(editor, c);

        printf("\033[K");

        PrintRowText(editor -> node_col);
        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == ENTER)
    {
        InsertRow(editor);
        SplitLine(editor);
        if(editor -> cursor_row > editor -> console_height - 2)
        {
            editor -> print_head = editor -> print_head -> next;
            editor -> cursor_row--;

            ClearText(editor, 1);

            PrintText(editor, editor -> print_head, 1);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
            return;
        }

        ClearText(editor, editor -> cursor_row - 1);

        PrintText(editor, editor -> node_row -> prev, editor -> cursor_row - 1);
        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == BACKSPACE)
    {
        if(editor -> cursor_col == 1)
        {
            if(editor -> cursor_row == 1 && editor -> print_head -> prev != NULL)
            {
                editor -> print_head = editor -> print_head -> prev;
                editor -> cursor_row++;
                MergeLine(editor);
                DeleteRow(editor);
                //
                if(editor -> node_row -> num_char > editor -> console_width - 1)
                {
                    Editor temp = *editor;
                    while(editor -> cursor_col < editor -> console_width)
                        MovePointerRight(editor);
                    InsertRow(editor);
                    SplitLine(editor);
                    *editor = temp;
                    editor -> num_lines++;
                }
                //
                ClearText(editor, 1);

                PrintText(editor, editor -> print_head, 1);
                PrintStatusBar(editor);
                PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
                MoveCursor(editor);
                return;
            }
            MergeLine(editor);
            DeleteRow(editor);
            //
            if(editor -> node_row -> num_char > editor -> console_width - 1)
            {
                Editor temp = *editor;
                while(editor -> cursor_col < editor -> console_width)
                    MovePointerRight(editor);
                InsertRow(editor);
                SplitLine(editor);
                *editor = temp;
                editor -> num_lines++;
            }
            //
            ClearText(editor, editor -> cursor_row);

            PrintText(editor, editor -> node_row, editor -> cursor_row);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
        }
        else
        {
            DeleteChar(editor);

            MoveCursor(editor);
            printf("\033[K");

            CharNode *temp;
            if(editor -> node_col == NULL)
                temp = editor -> node_row -> str;
            else
                temp = editor -> node_col -> next;
            PrintRowText(temp);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
        }
    }
    else if(c == LEFTARROW)
    {
        MovePointerLeft(editor);
        if(editor -> cursor_row < 1 && editor -> print_head -> prev != NULL)
        {
            editor -> print_head = editor -> print_head -> prev;
            editor -> cursor_row++;
                    
            ClearText(editor, 1);

            PrintText(editor, editor -> print_head, 1);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
            return;
        }

        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == RIGHTARROW)
    {
        MovePointerRight(editor);
        if(editor -> cursor_row > editor -> console_height - 2)
        {
            editor -> print_head = editor -> print_head -> next;
            editor -> cursor_row--;

            ClearText(editor, 1);

            PrintText(editor, editor -> print_head, 1);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
            return;
        }

        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == UPARROW)
    {
        MovePointerUp(editor);
        if(editor -> cursor_row < 1 && editor -> print_head -> prev != NULL)
        {
            editor -> print_head = editor -> print_head -> prev;
            editor -> cursor_row++;

            ClearText(editor, 1);

            PrintText(editor, editor -> print_head, 1);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
            return;
        }

        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == DOWNARROW)
    {
        MovePointerDown(editor);
        if(editor -> cursor_row > editor -> console_height - 2)
        {
            editor -> print_head = editor -> print_head -> next;
            editor -> cursor_row--;

            ClearText(editor, 1);

            PrintText(editor, editor -> print_head, 1);
            PrintStatusBar(editor);
            PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
            MoveCursor(editor);
            return;
        }

        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == HOME)
    {
        MovePointerHome(editor);

        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == END)
    {
        MovePointerEnd(editor);

        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == PGUP)
    {
        MovePointerPgUp(editor);

        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    }
    else if(c == PGDN)
    {
        MovePointerPgDn(editor);

        PrintStatusBar(editor);
        PrintMessageBar(editor, "Help: Ctrl-s = save | Ctrl-q = quit | Ctrl-f = find");
        MoveCursor(editor);
    } 
    else if(c == CTRL_S)
    {
        SaveFile(editor);
    }
    else if(c == CTRL_Q)
    {
        ExitEditor(editor);
    }
    else if(c == CTRL_F)
    {
        Search(editor);
    }
    else
        return;
}
