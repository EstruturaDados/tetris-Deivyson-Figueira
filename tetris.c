#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Definição das estruturas de dados e constantes ---

// Atributos de cada peça
typedef struct Peca {
    char nome;
    int id;
} Peca;

// Estrutura para a fila circular
typedef struct Fila {
    Peca pecas[5];
    int frente;
    int tras;
    int capacidade;
} Fila;

// Estrutura para a pilha
typedef struct Pilha {
    Peca pecas[3];
    int topo;
    int capacidade;
} Pilha;

// Tipos de ação para o histórico
typedef enum {
    JOGAR,
    RESERVAR
} TipoAcao;

// Estrutura para registrar as ações (para o 'desfazer')
typedef struct Acao {
    TipoAcao tipo;
    Peca pecaAntiga; // Peça que foi removida da fila
    Peca pecaNova;   // Peça que foi gerada
} Acao;

// Pilha para armazenar o histórico de ações
typedef struct PilhaAcoes {
    Acao acoes[10]; // Capacidade para 10 ações de histórico
    int topo;
} PilhaAcoes;

// Variáveis globais para o ID e histórico de peças
int proximoIdPeca = 0;
PilhaAcoes historico;

// --- Protótipos das funções ---

// Funções de inicialização
void inicializarFila(Fila* fila);
void inicializarPilha(Pilha* pilha);
void inicializarHistorico();

// Funções de manipulação de peças e estruturas
Peca gerarPeca();
void enqueue(Fila* fila, Peca peca);
Peca dequeue(Fila* fila);
void push(Pilha* pilha, Peca peca);
Peca pop(Pilha* pilha);

// Funções de interação com o usuário
void exibirEstado(Fila* fila, Pilha* pilha);
void menu();
void realizarAcao(int opcao, Fila* fila, Pilha* pilha);

// Ações específicas
void jogarPeca(Fila* fila);
void reservarPeca(Fila* fila, Pilha* pilha);
void usarPecaReservada(Pilha* pilha);
void trocarPeca(Fila* fila, Pilha* pilha);
void trocaMultipla(Fila* fila, Pilha* pilha);
void desfazerJogada(Fila* fila, Pilha* pilha);
void inverterFilaComPilha(Fila* fila, Pilha* pilha);

// Funções de histórico
void pushAcao(TipoAcao tipo, Peca pecaAntiga, Peca pecaNova);
Acao popAcao();

// --- Implementação das funções ---

/**
 * @brief Inicializa a fila circular com 5 peças geradas aleatoriamente.
 */
void inicializarFila(Fila* fila) {
    fila->frente = 0;
    fila->tras = -1;
    fila->capacidade = 5;
    
    printf("=======================================================\n");
    printf("|                       Início                        |\n");
    printf("=======================================================\n");
    for (int i = 0; i < 5; i++) {
        Peca novaPeca = gerarPeca();
        fila->tras = (fila->tras + 1) % fila->capacidade;
        fila->pecas[fila->tras] = novaPeca;
    }
}

/**
 * @brief Inicializa a pilha, deixando-a vazia.
 */
void inicializarPilha(Pilha* pilha) {
    pilha->topo = -1;
    pilha->capacidade = 3;
}

/**
 * @brief Inicializa a pilha de histórico.
 */
void inicializarHistorico() {
    historico.topo = -1;
}

/**
 * @brief Gera uma nova peça com nome aleatório ('I', 'O', 'T', 'L') e ID único.
 */
Peca gerarPeca() {
    Peca novaPeca;
    char tipos[] = {'I', 'O', 'T', 'L'};
    novaPeca.nome = tipos[rand() % 4];
    novaPeca.id = proximoIdPeca++;
    return novaPeca;
}

/**
 * @brief Adiciona uma peça ao final da fila (enqueue).
 */
void enqueue(Fila* fila, Peca peca) {
    fila->tras = (fila->tras + 1) % fila->capacidade;
    fila->pecas[fila->tras] = peca;
}

/**
 * @brief Remove a peça da frente da fila (dequeue).
 */
Peca dequeue(Fila* fila) {
    Peca pecaRemovida = fila->pecas[fila->frente];
    fila->frente = (fila->frente + 1) % fila->capacidade;
    return pecaRemovida;
}

/**
 * @brief Adiciona uma peça no topo da pilha (push).
 */
void push(Pilha* pilha, Peca peca) {
    if (pilha->topo < pilha->capacidade - 1) {
        pilha->pecas[++pilha->topo] = peca;
    } else {
        printf("A pilha está cheia. Não é possível adicionar mais peças.\n");
    }
}

/**
 * @brief Remove a peça do topo da pilha (pop).
 */
Peca pop(Pilha* pilha) {
    if (pilha->topo >= 0) {
        return pilha->pecas[pilha->topo--];
    }
    printf("A pilha está vazia. Não há peças para remover.\n");
    return (Peca){.nome = ' ', .id = -1};
}

/**
 * @brief Adiciona uma ação ao histórico.
 */
void pushAcao(TipoAcao tipo, Peca pecaAntiga, Peca pecaNova) {
    if (historico.topo < 9) {
        historico.topo++;
        historico.acoes[historico.topo].tipo = tipo;
        historico.acoes[historico.topo].pecaAntiga = pecaAntiga;
        historico.acoes[historico.topo].pecaNova = pecaNova;
    }
}

/**
 * @brief Remove e retorna a última ação do histórico.
 */
Acao popAcao() {
    if (historico.topo >= 0) {
        return historico.acoes[historico.topo--];
    }
    return (Acao){.tipo = -1};
}

/**
 * @brief Exibe o estado atual da fila e da pilha.
 */
void exibirEstado(Fila* fila, Pilha* pilha) {
    printf("\n=======================================================\n");
    printf("|                     Estado atual                    |\n");
    printf("=======================================================\n");
    
    printf("\n-------------------------------------------------------\n");
    printf("Fila de peças\t");
    for (int i = 0; i < fila->capacidade; i++) {
        int indice = (fila->frente + i) % fila->capacidade;
        printf("[ %c %d ] ", fila->pecas[indice].nome, fila->pecas[indice].id);
    }
    printf("\n-------------------------------------------------------\n");
    
    printf("Pilha de reserva\t(Topo -> Base): ");
    if (pilha->topo < 0) {
        printf("Pilha vazia.");
    } else {
        for (int i = pilha->topo; i >= 0; i--) {
            printf("[ %c %d ] ", pilha->pecas[i].nome, pilha->pecas[i].id);
        }
    }
    printf("\n-------------------------------------------------------\n");
    printf("\n");
}

/**
 * @brief Exibe o menu de opções para o usuário.
 */
void menu() {
    printf("\n-------------------------------------------------------\n");
    printf("                           Opções                        \n");
    printf("-------------------------------------------------------\n");
    printf("1 - Jogar peça da frente da fila\n");
    printf("2 - Enviar peça da fila para a pilha de reserva\n");
    printf("3 - Usar peça da pilha de reserva\n");
    printf("4 - Trocar peça da frente da fila com o topo da pilha\n");
    printf("5 - Desfazer última jogada (Jogar/Reservar)\n");
    printf("6 - Inverter fila com pilha\n");
    printf("0 - Sair\n");
    printf("\n-------------------------------------------------------\n");
}

/**
 * @brief Gerencia a ação escolhida pelo usuário.
 */
void realizarAcao(int opcao, Fila* fila, Pilha* pilha) {
    switch (opcao) {
        case 1:
            jogarPeca(fila);
            break;
        case 2:
            reservarPeca(fila, pilha);
            break;
        case 3:
            usarPecaReservada(pilha);
            break;
        case 4:
            trocarPeca(fila, pilha);
            break;
        case 5:
            desfazerJogada(fila, pilha);
            break;
        case 6:
            inverterFilaComPilha(fila, pilha);
            break;
        case 0:
            printf("Encerrando o programa. Até mais!\n");
            break;
        default:
            printf("Opção inválida. Por favor, escolha uma das opções do menu.\n");
    }
}

/**
 * @brief Remove a peça da frente da fila e gera uma nova. Salva a ação no histórico.
 */
void jogarPeca(Fila* fila) {
    Peca pecaAntiga = fila->pecas[fila->frente];
    Peca novaPeca = gerarPeca();

    dequeue(fila);
    enqueue(fila, novaPeca);

    pushAcao(JOGAR, pecaAntiga, novaPeca);
    printf("\n-------------------------------------------------------\n");
    printf("                         Ação");
    printf("\n-------------------------------------------------------\n");
    printf("A peça [ %c %d ] foi jogada. Uma nova peça [ %c %d ] foi adicionada.", pecaAntiga.nome, pecaAntiga.id, novaPeca.nome, novaPeca.id);
    printf("\n-------------------------------------------------------\n");
}

/**
 * @brief Move a peça da frente da fila para a pilha de reserva. Salva a ação no histórico.
 */
void reservarPeca(Fila* fila, Pilha* pilha) {
    if (pilha->topo < pilha->capacidade - 1) {
        Peca pecaAntiga = fila->pecas[fila->frente];
        Peca novaPeca = gerarPeca();

        dequeue(fila);
        enqueue(fila, novaPeca);
        push(pilha, pecaAntiga);

        pushAcao(RESERVAR, pecaAntiga, novaPeca);
        printf("\n-------------------------------------------------------\n");
        printf("                         Ação");
        printf("\n-------------------------------------------------------\n");
        printf("A peça [ %c %d ] foi movida da fila para a pilha.\n", pecaAntiga.nome, pecaAntiga.id);
        printf("Uma nova peça [ %c %d ] foi adicionada ao final da fila.", novaPeca.nome, novaPeca.id);
        printf("\n-------------------------------------------------------\n");
    } else {
        printf("\n-------------------------------------------------------\n");
        printf("                         Ação");
        printf("\n-------------------------------------------------------\n");
        printf("Não foi possível reservar. A pilha está cheia.");
        printf("\n-------------------------------------------------------\n");
    }
}

/**
 * @brief Remove a peça do topo da pilha de reserva.
 */
void usarPecaReservada(Pilha* pilha) {
    if (pilha->topo >= 0) {
        Peca pecaUsada = pop(pilha);
        printf("\n-------------------------------------------------------\n");
        printf("                         Ação");
        printf("\n-------------------------------------------------------\n");
        printf("A peça reservada [ %c %d ] foi usada (removida da pilha).", pecaUsada.nome, pecaUsada.id);
        printf("\n-------------------------------------------------------\n");
    } else {
        printf("\n-------------------------------------------------------\n");
        printf("                         Ação");
        printf("\n-------------------------------------------------------\n");
        printf("A pilha de reserva está vazia.");
        printf("\n-------------------------------------------------------\n");
    }
}

/**
 * @brief Troca a peça da frente da fila com a do topo da pilha.
 */
void trocarPeca(Fila* fila, Pilha* pilha) {
    if (pilha->topo >= 0) {
        Peca tempPilha = pop(pilha);
        Peca tempFila = dequeue(fila);

        push(pilha, tempFila);
        enqueue(fila, tempPilha);
        
        printf("\n-------------------------------------------------------\n");
        printf("                         Ação");
        printf("\n-------------------------------------------------------\n");
        printf("Troca realizada entre a peça [ %c %d ] da fila e a peça [ %c %d ] da pilha.", tempFila.nome, tempFila.id, tempPilha.nome, tempPilha.id);
        printf("\n-------------------------------------------------------\n");
    } else {
        printf("\n-------------------------------------------------------\n");
        printf("                         Ação");
        printf("\n-------------------------------------------------------\n");
        printf("A pilha está vazia. Não foi possível realizar a troca.");
        printf("\n-------------------------------------------------------\n");
    }
}

/**
 * @brief Troca as 3 primeiras peças da fila com as 3 peças da pilha, se houverem.
 */
void trocaMultipla(Fila* fila, Pilha* pilha) {
    if (pilha->topo == pilha->capacidade - 1) { // Verifica se a pilha está cheia (3 peças)
        Peca tempFila[3];
        Peca tempPilha[3];

        // Retira as 3 primeiras da fila
        for(int i = 0; i < 3; i++){
            tempFila[i] = dequeue(fila);
        }

        // Retira as 3 da pilha
        for(int i = 0; i < 3; i++){
            tempPilha[i] = pop(pilha);
        }
        
        // Adiciona as 3 da pilha na fila
        for(int i = 0; i < 3; i++){
            enqueue(fila, tempPilha[i]);
        }
        
        // Adiciona as 3 da fila na pilha
        for(int i = 2; i >= 0; i--){ // Adiciona na ordem correta para a pilha
            push(pilha, tempFila[i]);
        }
        printf("\n-------------------------------------------------------\n");
        printf("                          Ação");
        printf("\n-------------------------------------------------------\n");
        printf("Troca realizada entre os 3 primeiros da fila e os 3 da pilha.");
        printf("\n-------------------------------------------------------\n");
    } else {
        printf("\n-------------------------------------------------------\n");
        printf("                          Ação");
        printf("\n-------------------------------------------------------\n");
        printf("Não foi possível realizar a troca. A pilha não tem 3 pecas.");
        printf("\n-------------------------------------------------------\n");
    }
}

/**
 * @brief Desfaz a última ação de Jogar ou Reservar.
 */
void desfazerJogada(Fila* fila, Pilha* pilha) {
    Acao ultimaAcao = popAcao();

    if (ultimaAcao.tipo == JOGAR) {
        printf("\nDesfazendo a última ação 'Jogar'...\n");
        // Remove a peca nova que foi adicionada e coloca a peca antiga de volta
        fila->tras = (fila->tras - 1 + fila->capacidade) % fila->capacidade;
        enqueue(fila, ultimaAcao.pecaAntiga);

    } else if (ultimaAcao.tipo == RESERVAR) {
        printf("\nDesfazendo a última ação 'Reservar'...\n");
        // Remove a peca nova da fila e a peca do topo da pilha, colocando a peca antiga de volta na fila
        fila->tras = (fila->tras - 1 + fila->capacidade) % fila->capacidade;
        pop(pilha);
        enqueue(fila, ultimaAcao.pecaAntiga);

    } else {
        printf("\n-------------------------------------------------------\n");
        printf("                          Ação");
        printf("\n-------------------------------------------------------\n");
        printf("Não há ações para desfazer ou a ação não é reversível.");
        printf("\n-------------------------------------------------------\n");
    }
}

/**
 * @brief Troca o conteúdo total da fila com o conteúdo da pilha.
 */
// Lógica para a função Inverter
void inverterFilaComPilha(Fila* fila, Pilha* pilha) {
    Peca tempFilaArray[5];
    Peca tempPilhaArray[3];
    int i;
    
    // 1. Move a fila para um array temporário
    for(i = 0; i < fila->capacidade; i++) {
        tempFilaArray[i] = dequeue(fila);
    }
    
    // 2. Move a pilha para um array temporário
    for(i = 0; i < pilha->capacidade; i++) {
        tempPilhaArray[i] = pop(pilha);
    }
    
    // 3. Move o conteúdo da pilha para a fila
    for(i = 0; i < pilha->capacidade; i++) {
        enqueue(fila, tempPilhaArray[i]);
    }
    
    // 4. Move o conteúdo da fila para a pilha (pegando apenas os 3 primeiros elementos)
    for(i = 2; i >= 0; i--) {
        push(pilha, tempFilaArray[i]);
    }
    
    printf("\n-------------------------------------------------------\n");
    printf("                          Ação");
    printf("\n-------------------------------------------------------\n");
    printf("Conteúdo da fila e da pilha foram invertidos!");
    printf("\n-------------------------------------------------------\n");
}
// --- Função principal (main) ---

int main() {
    srand(time(NULL));
    
    Fila filaDePecas;
    Pilha pilhaDeReserva;
    int opcao;

    inicializarFila(&filaDePecas);
    inicializarPilha(&pilhaDeReserva);
    inicializarHistorico();

    do {
        exibirEstado(&filaDePecas, &pilhaDeReserva);
        menu();
        printf("Sua escolha: ");
        scanf("%d", &opcao);
        
        realizarAcao(opcao, &filaDePecas, &pilhaDeReserva);

    } while (opcao != 0);

    return 0;
}