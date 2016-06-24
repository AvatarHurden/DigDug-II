bool commandActive = false;
unsigned char commandObject = 0;
unsigned char commandVariable = 0;

void CommandToggle(){
    if (!commandActive){
        commandActive = true;
        printf("Voce entrou no modo de comando.\n");
        printf("Escolha o objeto que deseja alterar.\n");
        printf("1 - jogador\n");
    } else {
        commandActive = false;
        printf("Voce saiu modo de comando.\n");
    }
}

CommandChangeVariable(unsigned char key){
    if (commandObject == '1'){
        if(commandVariable== '1'){
            if (key == '1') {playerMoveSpeed -= 0.05; printf("movespeed++");}
            if (key == '2') {playerMoveSpeed += 0.05; printf("movespeed--");}
        }
    }
}

void CommandHandleInput(unsigned char key){
    if (key == 'o') CommandToggle();
    if (key < '0' || key > '9')
        return;
    if (commandActive){
        if (commandObject){
            if(commandObject == key){
                printf("Nenhum objeto selecionado.\n");
                commandObject = 0;
            }
            else{
                if(commandVariable){
                    if(commandVariable == key){
                        printf("Nenhuma variável selecionada.\n");
                        commandVariable = 0;
                    }
                    else{
                        CommandChangeVariable(key);
                    }
                }
                else{
                    printf("Variavel: %c\n",key);
                    commandVariable = key;
                    printf("Digite 1 para diminuir e 2 para aumentar.\n");
                }
            }
        }
        else{
            printf("Objeto: %c\n",key);
            commandObject = key;
        }

    }
}


