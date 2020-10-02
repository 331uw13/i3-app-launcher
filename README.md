# i3-app-launcher
application launcher for i3 !

uses sfml: https://www.sfml-dev.org/

![](https://raw.githubusercontent.com/331uw13/i3-app-launcher/main/img.png)

Default keybinds:

- W/up: move up

- S/down: move down

- D/ENTER: run command

- Q/ESC: quit


you can add this to your i3 config if you want:

```
bindsym $mod+b exec "if ! ps -e | grep i3-app-launcher ; then exec i3-app-launcher ; fi"
```
