# YAChat
Yet Another Chat is a very simple chat application that serves as a Hello World for sockets.
The project is written for Linux and tested only on Debian. It may also be tested on Mac and developed to work on Mac as well. Windows development is not planned but might be added later.

There are a few TODOs around in the files.

## building
It is possible to build everything in `src/` into .so(s) but since the size of the program is very small, it is not required at this stage. Therefore, to build the program, you just need to run `make release`.

__IMPORTANT:__ The program requires entire html to be copied into `~/.yachat/html`. It will be done if `make install` is executed.
## tests
There are no tests yet. Planned to do later.
