#include <stdio.h>
#include <string.h>

#define MAX_PECAS 100
#define MAX_LOTES 1000

typedef struct {
    int codigo;
    char descricao[50];
} Peca;

typedef struct {
    int codigoPeca;
    int quantidade;
} Lote;

Peca pecas[MAX_PECAS];
Lote lotes[MAX_LOTES];

int totalPecas = 0;
int totalLotes = 0;

int localizarPeca(int codigo) {
    for (int i = 0; i < totalPecas; i++) {
        if (pecas[i].codigo == codigo)
            return i;
    }
    return -1;
}

void salvarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "w");

    if (arquivo == NULL) {
        printf("ERRO: Nao foi possivel salvar o estoque!\n");
        return;
    }

    fprintf(arquivo, "%d\n", totalPecas);

    for (int i = 0; i < totalPecas; i++) {
        fprintf(arquivo, "%d;%s\n",
                pecas[i].codigo,
                pecas[i].descricao);
    }

    fprintf(arquivo, "%d\n", totalLotes);

    for (int i = 0; i < totalLotes; i++) {
        fprintf(arquivo, "%d;%d\n",
                lotes[i].codigoPeca,
                lotes[i].quantidade);
    }

    fclose(arquivo);
}

void carregarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "r");

    if (arquivo == NULL)
        return;

    if (fscanf(arquivo, "%d\n", &totalPecas) != 1 ||
        totalPecas < 0 || totalPecas > MAX_PECAS) {
        totalPecas = 0;
        fclose(arquivo);
        return;
    }

    for (int i = 0; i < totalPecas; i++) {
        if (fscanf(arquivo, "%d;%49[^\n]\n",
                   &pecas[i].codigo,
                   pecas[i].descricao) != 2) {
            totalPecas = i;
            fclose(arquivo);
            return;
        }
    }

    if (fscanf(arquivo, "%d\n", &totalLotes) != 1 ||
        totalLotes < 0 || totalLotes > MAX_LOTES) {
        totalLotes = 0;
        fclose(arquivo);
        return;
    }

    for (int i = 0; i < totalLotes; i++) {
        if (fscanf(arquivo, "%d;%d\n",
                   &lotes[i].codigoPeca,
                   &lotes[i].quantidade) != 2) {
            totalLotes = i;
            break;
        }
    }

    fclose(arquivo);
}

void cadastrarPeca() {
    int codigo;

    if (totalPecas >= MAX_PECAS) {
        printf("Limite de pecas atingido!\n");
        return;
    }

    printf("Codigo da peca: ");
    scanf("%d", &codigo);

    if (localizarPeca(codigo) != -1) {
        printf("Codigo ja cadastrado!\n");
        return;
    }

    pecas[totalPecas].codigo = codigo;

    printf("Descricao da peca: ");
    scanf(" %49[^\n]", pecas[totalPecas].descricao);

    totalPecas++;

    salvarEstoque();

    printf("Peca cadastrada com sucesso!\n");
}

void registrarEntrada() {
    int codigo, quantidade;

    if (totalLotes >= MAX_LOTES) {
        printf("Limite de lotes atingido!\n");
        return;
    }

    printf("Codigo da peca: ");
    scanf("%d", &codigo);

    if (localizarPeca(codigo) == -1) {
        printf("Peca nao encontrada!\n");
        return;
    }

    printf("Quantidade recebida: ");
    scanf("%d", &quantidade);

    if (quantidade <= 0) {
        printf("Quantidade invalida!\n");
        return;
    }

    lotes[totalLotes].codigoPeca = codigo;
    lotes[totalLotes].quantidade = quantidade;

    totalLotes++;

    salvarEstoque();

    printf("Entrada registrada com sucesso!\n");
}

void registrarSaida() {
    int codigo, quantidade;
    int disponivel = 0;
    int restante;

    printf("Codigo da peca: ");
    scanf("%d", &codigo);

    if (localizarPeca(codigo) == -1) {
        printf("Peca nao encontrada!\n");
        return;
    }

    printf("Quantidade para retirada: ");
    scanf("%d", &quantidade);

    if (quantidade <= 0) {
        printf("Quantidade invalida!\n");
        return;
    }

    for (int i = 0; i < totalLotes; i++) {
        if (lotes[i].codigoPeca == codigo)
            disponivel += lotes[i].quantidade;
    }

    if (quantidade > disponivel) {
        printf("Estoque insuficiente! Disponivel: %d\n",
               disponivel);
        return;
    }

    restante = quantidade;

    /* FIFO: utiliza primeiro o lote mais antigo. */
    for (int i = 0; i < totalLotes && restante > 0; i++) {
        if (lotes[i].codigoPeca == codigo &&
            lotes[i].quantidade > 0) {

            int retirar = lotes[i].quantidade;

            if (retirar > restante)
                retirar = restante;

            lotes[i].quantidade -= retirar;
            restante -= retirar;
        }
    }

    salvarEstoque();

    printf("Saida registrada com sucesso!\n");
    printf("Metodo FIFO aplicado!\n");
}

void consultarEstoque() {
    printf("\n===== ESTOQUE DE PECAS =====\n");

    if (totalPecas == 0) {
        printf("Nenhuma peca cadastrada.\n");
        return;
    }

    for (int i = 0; i < totalPecas; i++) {
        int quantidadeTotal = 0;

        for (int j = 0; j < totalLotes; j++) {
            if (lotes[j].codigoPeca == pecas[i].codigo)
                quantidadeTotal += lotes[j].quantidade;
        }

        printf("\nCodigo: %d\n", pecas[i].codigo);
        printf("Descricao: %s\n", pecas[i].descricao);
        printf("Quantidade disponivel: %d\n",
               quantidadeTotal);
    }
}

int main() {
    int opcao;

    carregarEstoque();

    do {
        printf("\n===== SISTEMA DE CONTROLE DE PECAS - CAOA =====\n");
        printf("1 - Cadastrar peca\n");
        printf("2 - Registrar entrada\n");
        printf("3 - Registrar saida (FIFO)\n");
        printf("4 - Consultar estoque\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            int caractere;
            while ((caractere = getchar()) != '\n' &&
                   caractere != EOF) {}
            opcao = -1;
        }

        switch (opcao) {
            case 1:
                cadastrarPeca();
                break;

            case 2:
                registrarEntrada();
                break;

            case 3:
                registrarSaida();
                break;

            case 4:
                consultarEstoque();
                break;

            case 0:
                salvarEstoque();
                printf("Dados salvos. Sistema encerrado!\n");
                break;

            default:
                printf("Opcao invalida!\n");
        }

    } while (opcao != 0);

    return 0;
}