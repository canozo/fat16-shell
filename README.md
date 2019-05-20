# FAT16 Shell

Para compilar se necesita el **Developer Command Prompt for Visual Studio**:
```
cd fat16-shell
cl /EHsc src/main.cpp src/dir_entry.cpp src/ls.cpp src/cat.cpp src/cd.cpp src/fat_utils.cpp src/mkdir.cpp src/cat2.cpp
main.exe
```

Comandos soportados:
```
ls -l
cat file.txt
cat > file.txt
mkdir dir
cd dir
exit
```
