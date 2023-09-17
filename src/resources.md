Resources for linux and compilers

Git pushing
>> git add .
>> git commit -m "message"
>> git push origin <<branch name>>
>> path: /workspaces/ReaverCompiler/

Dockerfile fuckshit
>> git remote -v
>> ssh -T git@github.com

Starting Docker
>> sudo service docker start
>> docker ps // shows running containers

Fix Docker daemon socket permission
>> sudo usermod -aG docker user1
>> sudo chmod 666 /var/run/docker.sock


Langproc Github
https://github.com/LangProc

Simon Staal's Odyssey C Compiler (C90 to MIPS)
https://github.com/simon-staal/Odyssey_C_Compiler/tree/master

ADMB-Project (compiler guide pdf)
https://www.admb-project.org/tools/flex/compiler.pdf

TCLER
Source: https://wiki.tcl-lang.org/page/Scripted+Compiler
Lexing: https://wiki.tcl-lang.org/page/Lexing+C
Parsing: https://wiki.tcl-lang.org/page/Parsing+C