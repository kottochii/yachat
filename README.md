# YAChat
Yet Another Chat is a very simple chat application that serves as a Hello World for sockets.
The project is targeted for Linux and supported on MacOS. Support for Windows is not guaranteed.

The last version is in [0.0.1](https://github.com/kottochii/yachat/tree/0.0.1).

## prerequisites
* OpenSSL must be present before installing the project. If it does not have includes or libraries in standard directories, they must be referenced in `./makefile` in respective variables (_to be added_) 
* [Rami v0.0.1beta](https://github.com/kottochii/rami/tree/0.0.1beta) is distributed with the project and should be build in `make build`. (_to be added_, _to be implemented_)

There are other prerequisites for clients as they must support WebSockets (_to be implemented_), jQuery, iframes and some other features of JS, CSS and HTML.


## building
All of it is yet _to be added_
In future versions, makefile should include the following:
* `make install` to copy the required (client) files into `/.yachat-kottochii/`.
* `make build` to build everything into `./src/` .so files in `./bin/`
* `make release` to build the release version into `./bin/`. 

__IMPORTANT:__ The program requires entire html to be copied into `~/.yachat-kottochii/html`. It will be done if `make install` is executed.

## TODO
* Reorganise client files on server side. HTML, CSS, JS must be in different files. Folder still must be called `./html`
* Reorganise the JavaScript on client-side. Make it more readable and more upgradable.

## NOT TODO
* Server is now being rewritten to support WebSockets. Do not implement websockets.
