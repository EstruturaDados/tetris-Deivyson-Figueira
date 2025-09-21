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

// Variável global para o ID da peça, garantindo que seja único
int proximoIdPeca = 0;

// --- Protótipos das funções ---

// Funções de inicialização
void inicializarFila(Fila* fila);
void inicializarPilha(Pilha* pilha);

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

// --- Implementação das funções ---

/**
 * @brief Inicializa a fila circular com 5 peças geradas aleatoriamente.
 * * @param fila Ponteiro para a estrutura da fila.
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
        // Adiciona a peça no final da fila
        fila->tras = (fila->tras + 1) % fila->capacidade;
        fila->pecas[fila->tras] = novaPeca;
    }
}

/**
 * @brief Inicializa a pilha, deixando-a vazia.
 * * @param pilha Ponteiro para a estrutura da pilha.
 */
void inicializarPilha(Pilha* pilha) {
    pilha->topo = -1;
    pilha->capacidade = 3;
}

/**
 * @brief Gera uma nova peça com nome aleatório ('I', 'O', 'T', 'L') e ID único.
 * * @return Retorna a nova peça gerada.
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
 * * @param fila Ponteiro para a estrutura da fila.
 * @param peca A peça a ser adicionada.
 */
void enqueue(Fila* fila, Peca peca) {
    // A fila tem tamanho fixo, então sempre haverá espaço para uma nova peça
    // se uma for removida
    fila->tras = (fila->tras + 1) % fila->capacidade;
    fila->pecas[fila->tras] = peca;
}

/**
 * @brief Remove a peça da frente da fila (dequeue).
 * * @param fila Ponteiro para a estrutura da fila.
 * @return Retorna a peça removida.
 */
Peca dequeue(Fila* fila) {
    Peca pecaRemovida = fila->pecas[fila->frente];
    fila->frente = (fila->frente + 1) % fila->capacidade;
    return pecaRemovida;
}

/**
 * @brief Adiciona uma peça no topo da pilha (push).
 * * @param pilha Ponteiro para a estrutura da pilha.
 * @param peca A peça a ser adicionada.
 */
void push(Pilha* pilha, Peca peca) {
    if (pilha->topo < pilha->capacidade - 1) {
        pilha->pecas[++pilha->topo] = peca;
        printf("Peça [ %c %d ] adicionada a pilha.\n", peca.nome, peca.id);
    } else {
        printf("A pilha está cheia. Nãoe possível adicionar mais peças.\n");
    }
}

/**
 * @brief Remove a peça do topo da pilha (pop).
 * * @param pilha Ponteiro para a estrutura da pilha.
 * @return Retorna a peça removida.
 */
Peca pop(Pilha* pilha) {
    if (pilha->topo >= 0) {
        return pilha->pecas[pilha->topo--];
    } else {
        // Retorna uma peça 'nula' ou com valores padrão para indicar erro
        printf("A pilha está vazia. Não há peças para remover.\n");
        return (Peca){.nome = ' ', .id = -1};
    }
}

/**
 * @brief Exibe o estado atual da fila e da pilha.
 * * @param fila Ponteiro para a estrutura da fila.
 * @param pilha Ponteiro para a estrutura da pilha.
 */
void exibirEstado(Fila* fila, Pilha* pilha) {
    printf("\n=======================================================\n");
    printf("|                     Estado atual                    |\n");
    printf("=======================================================\n");
    
    // Exibe a fila
    printf("\n-------------------------------------------------------\n");
    printf("Fila de peças\t");
    for (int i = 0; i < fila->capacidade; i++) {
        int indice = (fila->frente + i) % fila->capacidade;
        printf("[ %c %d ] ", fila->pecas[indice].nome, fila->pecas[indice].id);
    }printf("\n-------------------------------------------------------\n");
    
    // Exibe a pilha
    printf("Pilha de reserva\t(Topo -> Base): ");
    if (pilha->topo < 0) {
        printf("Pilha vazia.");
    } else {
        for (int i = pilha->topo; i >= 0; i--) {
            printf("[ %c %d ] ", pilha->pecas[i].nome, pilha->pecas[i].id);
        
        }    
    }
     printf("\n-------------------------------------------------------\n");
}

/**
 * @brief Exibe o menu de opções para o usuário.
 */
void menu() {
    printf("\n-------------------------------------------------------\n");
    printf("                           Opções                        ");
    printf("\n-------------------------------------------------------\n");
    printf("1 - Jogar peça da frente da fila\n");
    printf("2 - Enviar peça da fila para a pilha de reserva\n");
    printf("3 - Usar peça da pilha de reserva\n");
    printf("4 - Trocar peça da frente da fila com o topo da pilha\n");
    printf("5 - Trocar os 3 primeiros da fila com as 3 peças da pilha\n");
    printf("0 - Sair");
    printf("\n-------------------------------------------------------\n");
}

/**
 * @brief Gerencia a ação escolhida pelo usuário.
 * * @param opcao Opção numérica escolhida.
 * @param fila Ponteiro para a estrutura da fila.
 * @param pilha Ponteiro para a estrutura da pilha.
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
            trocaMultipla(fila, pilha);
            break;
        case 0:
            printf("Encerrando o programa. Até mais!\n");
            break;
        default:
            printf("Opção inválida. Por favor, escolha uma das opções do menu.\n");
    }
}

/**
 * @brief Remove a peça da frente da fila e gera uma nova para manter o tamanho.
 * * @param fila Ponteiro para a estrutura da fila.
 */
void jogarPeca(Fila* fila) {
    Peca pecaJogada = dequeue(fila);
    printf("\n-------------------------------------------------------\n");
    printf("                         Ação");
    printf("\n-------------------------------------------------------\n");
    printf("A peça [ %c %d ] foi jogada.\n", pecaJogada.nome, pecaJogada.id);

    // Gera uma nova peça para manter a fila cheia
    Peca novaPeca = gerarPeca();
    enqueue(fila, novaPeca);
    printf("A peça [ %c %d ] foi adicionada ao final da fila.", novaPeca.nome, novaPeca.id);
    printf("\n-------------------------------------------------------\n");
}

/**
 * @brief Move a peça da frente da fila para a pilha de reserva.
 * * @param fila Ponteiro para a estrutura da fila.
 * @param pilha Ponteiro para a estrutura da pilha.
 */
void reservarPeca(Fila* fila, Pilha* pilha) {
    if (pilha->topo < pilha->capacidade - 1) {
        Peca pecaReservada = dequeue(fila);
        push(pilha, pecaReservada);
        printf("\n-------------------------------------------------------\n");
        printf("                         Ação");
        printf("\n-------------------------------------------------------\n");
        printf("A peça [ %c %d ] foi movida da fila para a pilha.\n", pecaReservada.nome, pecaReservada.id);
        
        // Gera uma nova peça para manter a fila cheia
        Peca novaPeca = gerarPeca();
        enqueue(fila, novaPeca);
        printf("Uma nova peça [ %c %d ] foi adicionada ao final da fila.", novaPeca.nome, novaPeca.id);
        printf("\n-------------------------------------------------------\n");
    } else {
        printf("Não foi possível reservar. A pilha está cheia.");
        printf("\n-------------------------------------------------------\n");
    }
}

/**
 * @brief Remove a peça do topo da pilha de reserva.
 * * @param pilha Ponteiro para a estrutura da pilha.
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
 * * @param fila Ponteiro para a estrutura da fila.
 * @param pilha Ponteiro para a estrutura da pilha.
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
        printf("Troca realizada entre a peça [ %c %d ] da fila \nE a peça [ %c %d ] da pilha.", tempFila.nome, tempFila.id, tempPilha.nome, tempPilha.id);
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
 * * @param fila Ponteiro para a estrutura da fila.
 * @param pilha Ponteiro para a estrutura da pilha.
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
        printf("                          Ação");
        printf("\n-------------------------------------------------------\n");
        printf("Não foi possível realizar a troca. A pilha não tem 3 pecas.");
        printf("\n-------------------------------------------------------\n");
    }
}

// --- Função principal (main) ---

int main() {
    // Inicializa a semente para a função rand()
    srand(time(NULL));
    
    Fila filaDePecas;
    Pilha pilhaDeReserva;
    int opcao;

    // Inicializa as estruturas de dados
    inicializarFila(&filaDePecas);
    inicializarPilha(&pilhaDeReserva);

    // Loop principal do programa
    do {
        exibirEstado(&filaDePecas, &pilhaDeReserva);
        menu();
        printf("Sua escolha: ");
        scanf("%d", &opcao);
        printf("\n");

        realizarAcao(opcao, &filaDePecas, &pilhaDeReserva);

    } while (opcao != 0);

    return 0;
}