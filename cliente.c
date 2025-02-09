#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void exibir_opcoes() 
{
    printf("\nEscolha uma jogada:\n\n");
    printf("T - Tesoura\n");
    printf("P - Papel\n");
    printf("R - Pedra\n");
    printf("L - Lagarto\n");
    printf("S - Spock\n\n");
    
    printf("Digite sua escolha: ");
}

char coletar_resposta() 
{
    char resposta;

    while (1) 
    {
        printf("Deseja continuar? (S/N): ");
        scanf(" %c", &resposta);

        if (resposta == 'S' || resposta == 's' || resposta == 'N' || resposta == 'n') 
        {
            return resposta;
        } 
        else 
        {
            printf("\n\nResposta inválida! Digite S para continuar ou N para sair.\n\n");
        }
    }
}

int main() 
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = { 0 };
    char ip_servidor[BUFFER_SIZE];

    printf("\nDigite o IP do servidor: ");
    fgets(ip_servidor, BUFFER_SIZE, stdin);
    
    ip_servidor[strcspn(ip_servidor, "\n")] = 0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        printf("\nErro ao criar o socket!\n\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip_servidor, &serv_addr.sin_addr) <= 0) 
    {
        printf("\nEndereço inválido ou não suportado!\n\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("\nFalha ao conectar ao servidor!\n\n");
        return -1;
    }

    printf("\nConectado ao servidor em %s. Vamos jogar Pedra-Papel-Tesoura-Lagarto-Spock?\n\n", ip_servidor);

    while (1) 
    {
        exibir_opcoes();

        char jogada;
        scanf(" %c", &jogada);

        if (strchr("TPRLS", jogada) == NULL) 
        {
            printf("\n\nJogada inválida! Escolha entre T, P, R, L, S.\n\n");
            continue;
        }

        send(sock, &jogada, sizeof(jogada), 0);

        memset(buffer, 0, BUFFER_SIZE);

        int bytes_recebidos = read(sock, buffer, BUFFER_SIZE);

        if (bytes_recebidos <= 0) 
        {
            printf("\nConexão com o servidor perdida.\n\n");
            break;
        }

        printf("\n%s\n\n", buffer);

        char resposta = coletar_resposta();

        if (resposta == 'N' || resposta == 'n') 
        {
            printf("\nJogo encerrado.\n\n");
            break;
        }
    }

    close(sock);

    printf("Conexão encerrada.\n\n");

    return 0;
}

