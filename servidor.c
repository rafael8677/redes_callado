#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Função para determinar o vencedor
const char* determinar_vencedor(char jogada_cliente, char jogada_servidor) {
    if (jogada_cliente == jogada_servidor) {
        return "\nEmpate!";
    }

    if ((jogada_cliente == 'T' && (jogada_servidor == 'P' || jogada_servidor == 'L')) ||
        (jogada_cliente == 'P' && (jogada_servidor == 'R' || jogada_servidor == 'S')) ||
        (jogada_cliente == 'R' && (jogada_servidor == 'L' || jogada_servidor == 'T')) ||
        (jogada_cliente == 'L' && (jogada_servidor == 'S' || jogada_servidor == 'P')) ||
        (jogada_cliente == 'S' && (jogada_servidor == 'T' || jogada_servidor == 'R'))) {
        return "\nVocê ganhou!";
    }

    return "\nServidor ganhou!";
}

int main() {
    srand(time(NULL));

    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char ip_servidor[BUFFER_SIZE];

    printf("\nDigite o IP para o servidor usar (ou pressione Enter para aceitar todas as conexões): ");
    fgets(ip_servidor, BUFFER_SIZE, stdin);
    ip_servidor[strcspn(ip_servidor, "\n")] = 0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("\nFalha ao criar socket!");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("\nFalha ao configurar socket!");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (strlen(ip_servidor) == 0) {
        address.sin_addr.s_addr = INADDR_ANY;
        printf("\nServidor rodando em todas as interfaces...\n");
    } else {
        if (inet_pton(AF_INET, ip_servidor, &address.sin_addr) <= 0) {
            perror("\nIP inválido!");
            exit(EXIT_FAILURE);
        }
        printf("\nServidor rodando no IP: %s\n", ip_servidor);
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("\nErro no bind!");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("\nErro no listen!");
        exit(EXIT_FAILURE);
    }

    printf("\nServidor aguardando conexões na porta %d...\n", PORT);

    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("\nErro ao aceitar conexão!");
            exit(EXIT_FAILURE);
        }

        printf("\nConexão aceita de %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        char buffer[BUFFER_SIZE] = {0};
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_recebidos = read(client_socket, buffer, BUFFER_SIZE);
            if (bytes_recebidos <= 0) {
                printf("\nConexão fechada pelo cliente.\n");
                break;
            }

            char jogada_cliente = buffer[0];
            printf("\nJogada recebida: %c\n", jogada_cliente);

            if (strchr("TPRLS", jogada_cliente) == NULL) {
                char mensagem_erro[] = "\n\nJogada inválida! Escolha entre T, P, R, L, S.\n\n";
                send(client_socket, mensagem_erro, strlen(mensagem_erro), 0);
                continue;
            }

            char jogadas[] = {'T', 'P', 'R', 'L', 'S'};
            char jogada_servidor = jogadas[rand() % 5];

            const char* resultado = determinar_vencedor(jogada_cliente, jogada_servidor);

            char resposta[BUFFER_SIZE];
            snprintf(resposta, BUFFER_SIZE, "Servidor jogou: %c\n%s", jogada_servidor, resultado);

            send(client_socket, resposta, strlen(resposta), 0);
        }

        close(client_socket);
    }

    return 0;
}
