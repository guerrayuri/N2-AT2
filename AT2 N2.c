#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Estrutura da cobra
typedef struct SegmentoCobra {
    int posicaoX, posicaoY;
    struct SegmentoCobra* proximo;
} SegmentoCobra;

// Estrutura do alimento
typedef struct {
    int posicaoX, posicaoY;
    int tipoEfeito; // 1: normal, 2: aumento de velocidade, 3: inversão de controles
} Alimento;

// Estrutura do tabuleiro
typedef struct {
    int largura, altura;
    char** matriz;
} Tabuleiro;

// Estrutura de estatísticas
typedef struct {
    char nome[20];
    int pontuacao;
} Estatistica;

// Função para criar o segmento inicial da cobra
SegmentoCobra* criarSegmentoInicial(int x, int y) {
    SegmentoCobra* novoSegmento = (SegmentoCobra*)malloc(sizeof(SegmentoCobra));
    if (novoSegmento == NULL) {
        printf("Erro ao alocar memória para o segmento inicial da cobra.\n");
        exit(1);
    }
    novoSegmento->posicaoX = x;
    novoSegmento->posicaoY = y;
    novoSegmento->proximo = NULL;
    return novoSegmento;
}

// Função para adicionar um novo segmento à cobra (crescimento)
void adicionarSegmentoCobra(SegmentoCobra* cobra) {
    SegmentoCobra* novoSegmento = (SegmentoCobra*)malloc(sizeof(SegmentoCobra));
    if (novoSegmento == NULL) {
        printf("Erro ao alocar memória para um novo segmento da cobra.\n");
        exit(1);
    }
    
    SegmentoCobra* cauda = cobra;
    while (cauda->proximo != NULL) {
        cauda = cauda->proximo;
    }

    novoSegmento->posicaoX = cauda->posicaoX;
    novoSegmento->posicaoY = cauda->posicaoY;
    novoSegmento->proximo = NULL;
    cauda->proximo = novoSegmento;
}

// Função para mover a cobra
int moverCobra(SegmentoCobra* cobra, int direcao, Tabuleiro tabuleiro) {
    int posicaoAnteriorX = cobra->posicaoX;
    int posicaoAnteriorY = cobra->posicaoY;

    switch(direcao) {
        case 0: cobra->posicaoY--; break; // Move para cima
        case 1: cobra->posicaoY++; break; // Move para baixo
        case 2: cobra->posicaoX--; break; // Move para a esquerda
        case 3: cobra->posicaoX++; break; // Move para a direita
    }

    // Verifica se a cobra colidiu com a borda ou um obstáculo
    if (cobra->posicaoX < 0 || cobra->posicaoX >= tabuleiro.largura ||
        cobra->posicaoY < 0 || cobra->posicaoY >= tabuleiro.altura ||
        tabuleiro.matriz[cobra->posicaoY][cobra->posicaoX] == '#') {
        return 1; // Colidiu
    }

    // Move os segmentos da cobra
    SegmentoCobra* atual = cobra->proximo;
    while (atual != NULL) {
        int tempX = atual->posicaoX;
        int tempY = atual->posicaoY;
        atual->posicaoX = posicaoAnteriorX;
        atual->posicaoY = posicaoAnteriorY;
        posicaoAnteriorX = tempX;
        posicaoAnteriorY = tempY;
        atual = atual->proximo;
    }
    return 0;
}

// Funções principais do jogo
Alimento gerarAlimento(int largura, int altura);
Tabuleiro carregarFase(const char* nomeArquivo);
Tabuleiro gerarFaseProcedural(int largura, int altura);
void exibirTabuleiro(SegmentoCobra* cobra, Alimento alimento, Tabuleiro tabuleiro);
int verificarColisao(SegmentoCobra* cobra);
void salvarEstatistica(const char* nomeJogador, int pontuacao);
void exibirMenu();
void fimDeJogo(int pontuacao);

// Função principal do loop de jogo
void iniciarJogo(const char* nomeJogador) {
    int direcao = 3; 
    int pontuacao = 0;
    int fase;

    for (fase = 1; fase <= 6; fase++) {
        Tabuleiro tabuleiro;

        if (fase <= 5) {
            char nomeArquivoFase[20];
            sprintf(nomeArquivoFase, "fase%d.txt", fase);
            tabuleiro = carregarFase(nomeArquivoFase);
        } else {
            tabuleiro = gerarFaseProcedural(20, 10);
        }

        SegmentoCobra* cobra = criarSegmentoInicial(5, 5);
        Alimento alimento = gerarAlimento(tabuleiro.largura, tabuleiro.altura);

        while (1) {
            char tecla = getchar();
            if (tecla == 'w') direcao = 0;
            else if (tecla == 's') direcao = 1;
            else if (tecla == 'a') direcao = 2;
            else if (tecla == 'd') direcao = 3;

            if (moverCobra(cobra, direcao, tabuleiro) || verificarColisao(cobra)) {
                fimDeJogo(pontuacao);
                salvarEstatistica(nomeJogador, pontuacao);
                return;
            }

            if (cobra->posicaoX == alimento.posicaoX && cobra->posicaoY == alimento.posicaoY) {
                adicionarSegmentoCobra(cobra);
                pontuacao += 10;
                alimento = gerarAlimento(tabuleiro.largura, tabuleiro.altura);
            }

            system("clear");
            exibirTabuleiro(cobra, alimento, tabuleiro);
            usleep(200000);
        }
    }
    salvarEstatistica(nomeJogador, pontuacao);
}

// Funções de verificação de colisão e exibição do tabuleiro
int verificarColisao(SegmentoCobra* cobra) {
    SegmentoCobra* cabeca = cobra;
    SegmentoCobra* atual = cabeca->proximo;
    while (atual != NULL) {
        if (cabeca->posicaoX == atual->posicaoX && cabeca->posicaoY == atual->posicaoY) {
            return 1; // Colidiu com o próprio corpo
        }
        atual = atual->proximo;
    }
    return 0;
}

void exibirTabuleiro(SegmentoCobra* cobra, Alimento alimento, Tabuleiro tabuleiro) {
    int y, x;
    for (y = 0; y < tabuleiro.altura; y++) {
        for (x = 0; x < tabuleiro.largura; x++) {
            int parteCobra = 0;
            SegmentoCobra* atual = cobra;
            while (atual != NULL) {
                if (atual->posicaoX == x && atual->posicaoY == y) {
                    printf("O");
                    parteCobra = 1;
                    break;
                }
                atual = atual->proximo;
            }
            if (!parteCobra) {
                if (alimento.posicaoX == x && alimento.posicaoY == y)
                    printf("A"); // Representação do alimento
                else
                    printf("%c", tabuleiro.matriz[y][x]); // Espaço ou obstáculo
            }
        }
        printf("\n");
    }
}

// Funções para carregar e gerar fases
Tabuleiro carregarFase(const char* nomeArquivo) {
    Tabuleiro tabuleiro;
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo da fase %s\n", nomeArquivo);
        exit(1);
    }

    fscanf(arquivo, "%d %d", &tabuleiro.largura, &tabuleiro.altura);
    tabuleiro.matriz = (char**)malloc(tabuleiro.altura * sizeof(char*));
    for (int i = 0; i < tabuleiro.altura; i++) {
        tabuleiro.matriz[i] = (char*)malloc(tabuleiro.largura * sizeof(char));
        for (int j = 0; j < tabuleiro.largura; j++) {
            fscanf(arquivo, " %c", &tabuleiro.matriz[i][j]);
        }
    }
    fclose(arquivo);
    return tabuleiro;
}

Tabuleiro gerarFaseProcedural(int largura, int altura) {
    Tabuleiro tabuleiro;
    tabuleiro.largura = largura;
    tabuleiro.altura = altura;
    tabuleiro.matriz = (char**)malloc(altura * sizeof(char*));
    for (int i = 0; i < altura; i++) {
        tabuleiro.matriz[i] = (char*)malloc(largura * sizeof(char));
        for (int j = 0; j < largura; j++) {
            if (rand() % 10 < 2) {
                tabuleiro.matriz[i][j] = '#'; // Obstáculo com 20% de chance
            } else {
                tabuleiro.matriz[i][j] = ' ';
            }
        }
    }
    return tabuleiro;
}

// Função para gerar alimento aleatório
Alimento gerarAlimento(int largura, int altura) {
    Alimento alimento;
    alimento.posicaoX = rand() % largura;
    alimento.posicaoY = rand() % altura;
    alimento.tipoEfeito = 1;
    return alimento;
}

// Função para terminar o jogo e exibir pontuação
void fimDeJogo(int pontuacao) {
    printf("Game Over! Pontuação final: %d\n", pontuacao);
    printf("Pressione qualquer tecla para sair...");
    getchar();
}

// Função para salvar estatísticas
void salvarEstatistica(const char* nomeJogador, int pontuacao) {
    FILE* arquivo = fopen("estatisticas.bin", "ab");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de estatísticas\n");
        return;
    }
    Estatistica estat;
    strncpy(estat.nome, nomeJogador, 20);
    estat.pontuacao = pontuacao;
    fwrite(&estat, sizeof(Estatistica), 1, arquivo);
    fclose(arquivo);
}

// Função de exibição do menu
void exibirMenu() {
    printf("1. Iniciar Jogo\n");
    printf("2. Estatísticas\n");
    printf("Escolha uma opção: ");
}

// Função principal
int main() {
    srand(time(NULL));
    exibirMenu();
    int opcao;
    char nomeJogador[20];
    scanf("%d", &opcao);

    if (opcao == 1) {
        printf("Nome do jogador: ");
        scanf("%s", nomeJogador);
        iniciarJogo(nomeJogador);
    } else if (opcao == 2) {
        FILE* arquivo = fopen("estatisticas.bin", "rb");
        if (arquivo) {
            Estatistica estat;
            while (fread(&estat, sizeof(Estatistica), 1, arquivo)) {
                printf("Jogador: %s, Pontuação: %d\n", estat.nome, estat.pontuacao);
            }
            fclose(arquivo);
        } else {
            printf("Nenhuma estatística disponível.\n");
        }
    }
    return 0;
}
