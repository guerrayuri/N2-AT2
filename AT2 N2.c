#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <unistd.h>

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

// Funções principais do jogo
SegmentoCobra* criarSegmentoInicial(int x, int y);
void adicionarSegmentoCobra(SegmentoCobra* cobra);
Alimento gerarAlimento(int largura, int altura);
Tabuleiro carregarFase(const char* nomeArquivo);
Tabuleiro gerarFaseProcedural(int largura, int altura);
void exibirTabuleiro(SegmentoCobra* cobra, Alimento alimento, Tabuleiro tabuleiro);
int moverCobra(SegmentoCobra* cobra, int direcao, Tabuleiro tabuleiro);
int verificarColisao(SegmentoCobra* cobra);
void salvarEstatistica(const char* nomeJogador, int pontuacao);
void exibirMenu();
void fimDeJogo(int pontuacao);

// Função principal do loop de jogo
void iniciarJogo(const char* nomeJogador) {
    int direcao = 3; 
    int pontuacao = 0;
    int fase; // Declaração da variável fora do loop

    for (fase = 1; fase <= 6; fase++) {
        Tabuleiro tabuleiro;

        // Separando o carregamento de fase com if para garantir compatibilidade
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
            if (_kbhit()) {
                char tecla = _getch();
                if (tecla == 'w') direcao = 0;
                else if (tecla == 's') direcao = 1;
                else if (tecla == 'a') direcao = 2;
                else if (tecla == 'd') direcao = 3;
            }

            if (moverCobra(cobra, direcao, tabuleiro) || verificarColisao(cobra)) {
                fimDeJogo(pontuacao);
                return;
            }

            if (cobra->posicaoX == alimento.posicaoX && cobra->posicaoY == alimento.posicaoY) {
                adicionarSegmentoCobra(cobra);
                pontuacao += 10;
                alimento = gerarAlimento(tabuleiro.largura, tabuleiro.altura);
            }

            system("cls");
            exibirTabuleiro(cobra, alimento, tabuleiro);
            usleep(200000);
        }
    }
    salvarEstatistica(nomeJogador, pontuacao);
}

// Funções de movimentação e verificação de colisão
int moverCobra(SegmentoCobra* cobra, int direcao, Tabuleiro tabuleiro);
int verificarColisao(SegmentoCobra* cobra);
void exibirTabuleiro(SegmentoCobra* cobra, Alimento alimento, Tabuleiro tabuleiro);
void fimDeJogo(int pontuacao);
Tabuleiro carregarFase(const char* nomeArquivo);
Tabuleiro gerarFaseProcedural(int largura, int altura);
Alimento gerarAlimento(int largura, int altura);

// Função de exibição do menu
void exibirMenu() {
    printf("1. Iniciar Jogo\n");
    printf("2. Estatísticas\n");
    printf("Escolha uma opção: ");
}

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
