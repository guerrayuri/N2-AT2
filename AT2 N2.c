#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variáveis globais para uso em loops
int i, j; // i e j devem ser declarados fora para o Dev-C++ 5.11

// Estrutura para armazenar os dados do jogador
typedef struct {
    char nome[50];
    int pontosTotais;
    int pontosFase[5]; // Pontuações específicas para 5 fases
} Jogador;

// Estrutura para representar o labirinto
typedef struct {
    int linhas;
    int colunas;
    int **matriz;
    int posInicioX;
    int posInicioY;
    int posDestinoX;
    int posDestinoY;
} Labirinto;

// Função para carregar o labirinto de um arquivo
Labirinto *carregarLabirinto(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s. Verifique o nome e tente novamente.\n", nomeArquivo);
        return NULL;
    }

    Labirinto *lab = (Labirinto *)malloc(sizeof(Labirinto));
    fscanf(arquivo, "%d %d", &lab->linhas, &lab->colunas);

    lab->matriz = (int **)malloc(lab->linhas * sizeof(int *));
    for (i = 0; i < lab->linhas; i++) {
        lab->matriz[i] = (int *)malloc(lab->colunas * sizeof(int));
        for (j = 0; j < lab->colunas; j++) {
            fscanf(arquivo, "%d", &lab->matriz[i][j]);
            if (lab->matriz[i][j] == 2) {
                lab->posInicioX = i;
                lab->posInicioY = j;
            } else if (lab->matriz[i][j] == 3) {
                lab->posDestinoX = i;
                lab->posDestinoY = j;
            }
        }
    }

    fclose(arquivo);
    return lab;
}

// Função para liberar a memória do labirinto
void liberarLabirinto(Labirinto *lab) {
    if (!lab) return;
    for (i = 0; i < lab->linhas; i++) {
        free(lab->matriz[i]);
    }
    free(lab->matriz);
    free(lab);
}

// Função para exibir o labirinto
void exibirLabirinto(Labirinto *lab) {
    for (i = 0; i < lab->linhas; i++) {
        for (j = 0; j < lab->colunas; j++) {
            if (lab->matriz[i][j] == 2)
                printf("S ");
            else if (lab->matriz[i][j] == 3)
                printf("E ");
            else
                printf("%d ", lab->matriz[i][j]);
        }
        printf("\n");
    }
}

// Função para mover o jogador
int moverJogador(Labirinto *lab, int *x, int *y, char movimento) {
    int novoX = *x, novoY = *y;

    if (movimento == 'w') novoX--;
    else if (movimento == 's') novoX++;
    else if (movimento == 'a') novoY--;
    else if (movimento == 'd') novoY++;

    if (novoX >= 0 && novoX < lab->linhas && novoY >= 0 && novoY < lab->colunas && lab->matriz[novoX][novoY] != 1) {
        *x = novoX;
        *y = novoY;
        return lab->matriz[novoX][novoY] == 3;
    }
    return 0;
}

// Função para salvar estatísticas em arquivo
void salvarEstatisticas(Jogador *jogador) {
    FILE *arquivo = fopen("estatisticas.bin", "ab");
    if (!arquivo) {
        printf("Erro ao salvar estatísticas.\n");
        return;
    }
    fwrite(jogador, sizeof(Jogador), 1, arquivo);
    fclose(arquivo);
}

// Função para exibir estatísticas
void exibirEstatisticas() {
    FILE *arquivo = fopen("estatisticas.bin", "rb");
    if (!arquivo) {
        printf("Nenhuma estatística disponível.\n");
        return;
    }

    Jogador jogador;
    printf("=== Estatísticas ===\n");
    while (fread(&jogador, sizeof(Jogador), 1, arquivo)) {
        printf("Jogador: %s - Pontos Totais: %d\n", jogador.nome, jogador.pontosTotais);
        for (i = 0; i < 5; i++) {
            printf("  Fase %d: %d pontos\n", i + 1, jogador.pontosFase[i]);
        }
    }
    fclose(arquivo);
}

// Função principal para jogar uma fase
int jogarFase(const char *arquivoFase, Jogador *jogador, int faseAtual) {
    Labirinto *lab = carregarLabirinto(arquivoFase);
    if (!lab) return 0;

    int x = lab->posInicioX;
    int y = lab->posInicioY;
    char movimento;
    int movimentos = 0;
    const int limiteMovimentos = 20;

    printf("=== Fase %d ===\n", faseAtual + 1);
    printf("Controles: w (cima), s (baixo), a (esquerda), d (direita)\n");

    while (1) {
        exibirLabirinto(lab);
        printf("Sua posição: (%d, %d). Movimentos restantes: %d\n", x, y, limiteMovimentos - movimentos);
        printf("Digite um movimento: ");
        fflush(stdin); // Limpa o buffer para evitar problemas
        scanf(" %c", &movimento);

        if (movimento != 'w' && movimento != 'a' && movimento != 's' && movimento != 'd') {
            printf("Movimento inválido! Use apenas 'w', 'a', 's' ou 'd'.\n");
            continue;
        }

        movimentos++;
        if (movimentos > limiteMovimentos) {
            printf("Você perdeu! Número de movimentos excedido.\n");
            liberarLabirinto(lab);
            return 0;
        }

        if (moverJogador(lab, &x, &y, movimento)) {
            printf("Parabéns! Você chegou ao destino em %d movimentos!\n", movimentos);
            jogador->pontosFase[faseAtual] = 100 - movimentos;
            jogador->pontosTotais += jogador->pontosFase[faseAtual];
            liberarLabirinto(lab);
            return 1;
        }
    }
}

// Função principal
int main() {
    char opcao;
    Jogador jogador = {.pontosTotais = 0};

    printf("Digite seu nome: ");
    scanf("%s", jogador.nome);

    while (1) {
        printf("\n=== Jogo de Labirinto ===\n");
        printf("1. Iniciar Jogo\n");
        printf("2. Estatísticas\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        fflush(stdin); // Limpa o buffer antes da entrada
        scanf(" %c", &opcao);

        if (opcao == '1') {
            char *fases[5] = {"fase1.txt", "fase2.txt", "fase3.txt", "fase4.txt", "fase5.txt"};
            for (i = 0; i < 5; i++) {
                if (!jogarFase(fases[i], &jogador, i)) {
                    printf("Fim de jogo! Pontuação total: %d\n", jogador.pontosTotais);
                    salvarEstatisticas(&jogador);
                    break;
                }
            }
        } else if (opcao == '2') {
            exibirEstatisticas();
        } else if (opcao == '3') {
            printf("Saindo do jogo. Até mais, %s!\n", jogador.nome);
            break;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }

    return 0;
}
