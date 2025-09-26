# vite (Visual Text Editor)

`vite` is a lightweight and efficient vi-style text editor implemented in C. It is designed to run on various operating systems, including Windows, Linux, and macOS.

![vite]()


## Main Features

`vite` provides a variety of essential features for text editing.

* **Text Editing**: Supports real-time character input and modification, along with basic editing functions like newlines and backspace.

* **Fast Cursor Movement**: Allows for rapid cursor navigation within the document using the arrow keys, `Home`, `End`, `Page Up`, and `Page Down`. 

* **File Management**: Create a new file by running `vite` from the command line, or open an existing file using the `vite [filename]` format.

* **String Search**: Use `Ctrl + F` to search for specific strings within the document. Found matches are highlighted, and you can navigate between them using the arrow keys.

* **Status Bar**: Displays useful information at the bottom of the editor, such as the current filename, total line count, and cursor position.

* **Message Bar**: Provides help text for major functions like saving, searching, and quitting.


## Build

Since a `makefile` is provided, you can easily build the project in an environment with a C compiler and `make` installed using the command below.

```bash
make
```


### Usage

* **Create a new file**

```bash
./vite
```

* **Open an existing file**

```bash
./vite <filename>
```


## Key Shortcuts

| Key                 | Function                                                       |
|---------------------|----------------------------------------------------------------|
| Ctrl + S            | Saves the current document.                                    |
| Ctrl + Q            | Quits the editor. (If there are unsaved changes, press twice.) |
| Ctrl + F            | Activates the string search feature.                           |
| Arrow Keys          | Moves the cursor up, down, left, or right.                     |
| Home / End          | Moves the cursor to the beginning or end of the current line.  |
| Page Up / Page Down | Moves the cursor by a full page.                               |