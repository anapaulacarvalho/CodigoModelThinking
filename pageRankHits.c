#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char *nome;
    float score;
} Vertice;

typedef struct {
    Vertice* vertices;
    int tamanho;
    int ** matrizadj;
} Grafo;

int ** Aloca(int lin, int col) {
    /*Aloca dinamicamente o vetor de vertices*/
    int i, j;
    int **m = (int**) malloc(lin * sizeof (int*));
    for (i = 0; i < lin; i++) {
        m[i] = (int*) malloc(col * sizeof (int));
        for (j = 0; j < col; j++) {
            m[i][j] = 0;
        }
    }
    return m;
}

float* normalizaVetor(float* vetor, int numero_vertices) {
    /* Funcao que ira normalizar o vetor passado */
    float soma = 0;
    int i;
    //somatorio do conteudo do vetor
    for (i = 0; i < numero_vertices; i++) {
        soma += vetor[i];
    }
    //divide o conteudo de cada posicao pelo somatorio
    for (i = 0; i < numero_vertices; i++) {
        vetor[i] = vetor[i] / soma;
    }
    return vetor;
}

static int cmp(const void *p1, const void *p2) {
    /*Funcao comparadora para o Quick Sort*/
    /*Fonte: https://stackoverflow.com/questions/15824966/trouble-using-bsearch-with-an-array-of-strings*/
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

int myStrCmp(const void *s1, const void *s2) {
    /*Funcao comparadora para a pesquisa binaria*/
    /*Fonte: https://stackoverflow.com/questions/15824966/trouble-using-bsearch-with-an-array-of-strings*/
    const char *key = s1;
    const char * const *arg = s2;
    //  printf("myStrCmp: s1(%p): %s, s2(%p): %s\n", s1, key, s2, *arg); imprime as ocorrencias dos repetidos
    return strcmp(key, *arg);
}

Vertice* ordenaBubbleSort(Vertice *vertice, int tamanho) {
    int i, j;
    float aux;
    char *aux2;
    Vertice *verticeaux;
    verticeaux = malloc(sizeof (Vertice) * tamanho);
    for (i = 0; i < tamanho; i++) {
        verticeaux[i] = vertice[i];
    }

    for (i = 0; i < tamanho; i++) {
        for (j = i + 1; j < tamanho; j++) {
            if (verticeaux[i].score < verticeaux[j].score) //coloca o maior na primeira posicao
            {
                aux = verticeaux[i].score;
                verticeaux[i].score = verticeaux[j].score;
                ;
                verticeaux[j].score = aux;

                aux2 = verticeaux[i].nome;
                verticeaux[i].nome = verticeaux[j].nome;
                verticeaux[j].nome = aux2;
            }
        }
    }
    return verticeaux;
}

void imprimeTopKPageRank(Grafo *grafo, int k) {
    Vertice *aux;
    int i;
    aux = ordenaBubbleSort(grafo->vertices, grafo->tamanho);
    printf("Top %d maior score: \n", k);
    for (i = 0; i < k && i < grafo->tamanho; i++) {
        printf("Posicao %d. Vertice %s - Score: %f\n", i + 1, aux[i].nome, aux[i].score);
    }
}

void obtemVertices(Grafo* grafo, char arquivo[]) {
    /* Funcao que cria um vetor de string que
    ira obter os vertices do arquivo passado por
    parametro e ao final, ira alterar o Grafo*/

    Vertice *vertices;
    char palavra[400];
    char *palavra1;
    char *palavra2;
    char * aux;
    int i, k, j = 0;
    FILE *arq;
    arq = fopen(arquivo, "r"); /*abre arquivo em modo leitura*/
    if (arq == NULL) {
        printf("Erro, nao foi possivel abrir o arquivo\n");
        return;
    }

    /* Declaracao de uma matriz de strings
    que armazena um vertice V em cada posicao*/
    char **nome_vertices;
    nome_vertices = Aloca(70000, 400); /*suporta ate 100 caracteres por vertice e 70000 vertices*/

    /*Declaracao de vetor auxiliar */
    char vertices_auxiliar[2][400];

    /* Leitura: Se A ganhou de B, ha uma aresta de B para A*/
    int linha = 0;
    while (!feof(arq)) /*Enquanto nao encontrar o fim do arquivo..*/ {
        fgets(palavra, 400, arq); /*Pega a linha inteira e armazena na string auxiliar nome*/

        /*Para dividir a string utilizei
        a funcao strtok da biblioteca string.h
        utilizando a virgula como primeiro
        delimitador e o NULL como segundo delimitador*/

        aux = strtok(palavra, ",");
        palavra1 = aux;
        while (aux != NULL) {
            palavra2 = aux;
            aux = strtok(NULL, ",");
        }
        /*Retira a parte null da segunda
        palavra decorrente do strtok*/
        palavra2 = strtok(palavra2, "\n");

        /*Grava as palavras em um vetor auxiliar*/
        sprintf(vertices_auxiliar[0], "%s", palavra1);
        sprintf(vertices_auxiliar[1], "%s", palavra2);
        if (linha % 1000 == 0) {
            printf("Lendo linha: %d\n", linha);
        }
        for (k = 0; k < 2; k++) {
            char * found = (char *) bsearch(vertices_auxiliar[k], nome_vertices, j, sizeof (char *), myStrCmp);
            if (found == NULL) {
                sprintf(nome_vertices[j], "%s", vertices_auxiliar[k]);
                j++;
                qsort(nome_vertices, j, sizeof (char *), cmp);
            }
        }
        linha++;
    }
    vertices = malloc(sizeof (Vertice) * j);
    /*Fecha o arquivo apos as operacoes*/
    for (i = 0; i < j; i++) {
        vertices[i].nome = nome_vertices[i];
    }
    grafo->vertices = vertices;
    grafo->tamanho = j;

    for (i = 0; i < grafo->tamanho; i++) {
        printf("Posicao %d. %s\n", i, grafo->vertices[i].nome); //imprime vetor
    }

    fclose(arq);
    free(nome_vertices);
}

void criaMatrizAdjacencia(Grafo *grafo, char arquivo[]) {
    /* Funcao que cria uma matriz com 0s e 1s.
    Caso o vertice A incida o vertice B, a aresta é 1.
    Ao final, altera-se o Grafo passado
    por parametro */

    char palavra[400];
    char *palavra1;
    char *palavra2;
    char * aux;
    int linha, coluna, i;
    char **nome_vertices;
    FILE *arq;
    arq = fopen(arquivo, "r"); /*abre arquivo em modo leitura*/
    if (arq == NULL) {
        printf("Erro, nao foi possivel abrir o arquivo\n");
        return;
    }

    nome_vertices = Aloca(70000, 400);
    for (i = 0; i < grafo->tamanho; i++) {
        nome_vertices[i] = grafo->vertices[i].nome; //Preenche o vetor auxiliar
    }

    /* Declaracao da Matriz de Adjacencia,
    contendo 0 ou 1 se houve arestas*/
    int **matrizadj;

    /* Com base no numero de elementos,
    criaremos uma matriz de Adjacencia para as arestas */
    matrizadj = Aloca(grafo->tamanho, grafo->tamanho);

    /*Leremos o arquivo novamente para
    estabelecer as arestas */

    while (fgets(palavra, 400, arq) != NULL) /*Enquanto nao
  encontrar o fim do arquivo..*/ {
        /*Para dividir a string utilizei a funcao
        strtok da biblioteca string.h utilizando a
        vÃ­rgula como primeiro delimitador e o
        NULL como segundo delimitador*/

        aux = strtok(palavra, ",");
        palavra1 = aux;
        while (aux != NULL) {
            palavra2 = aux;
            aux = strtok(NULL, ",");
        }
        /*retira a parte null da segunda palavra decorrente do strtok*/
        palavra2 = strtok(palavra2, "\n");

        /* Encontraremos a posicao da primeira palavra
        lida no vetor de vertices e relacionaremos
        essa posicao com a posicao da matriz i.
        Apos ler a segunda palavra, faremos a relacao entre a
        posicao j da segunda palavra e a primeira palavra,
        atribuindo 1 para a aresta i,j lida*/

        /*Procura a palavra no vetor de vetices nome_vertices*/
        char * found = (char *) bsearch(palavra1, nome_vertices, grafo->tamanho, sizeof (char *), myStrCmp);
        char * found2 = (char *) bsearch(palavra2, nome_vertices, grafo->tamanho, sizeof (char *), myStrCmp);

        linha = ((int) found - (int) nome_vertices) / sizeof (char*); //encontra a posiÃ§Ã£o no vetor gerada pelo bsearch
        coluna = ((int) found2 - (int) nome_vertices) / sizeof (char*);

        matrizadj[linha][coluna] = 1;

    }
    grafo->matrizadj = matrizadj;
    fclose(arq);
}

int* getVetorOutdegree(Grafo *grafo) {
    /* Gera, para cada vertice, o seu grau de saida
    (ou seja, o numero de arestas adjacentes).
    Armazena todos no vetor out_degree.
    Em que out_degree[v] e o grau de saida do vertice v. */
    int *out_degree;
    out_degree = malloc(sizeof (int)*grafo->tamanho);
    int i, j;
    int saida = 0;
    for (i = 0; i < grafo->tamanho; i++) {
        for (j = 0; j < grafo->tamanho; j++) {
            if (grafo->matrizadj[i][j] == 1) {
                saida++; //conta quantas ocorrencias saidas do vertices
            }
        }
        out_degree[i] = saida;
        saida = 0;
    }

    for (i = 0; i < grafo->tamanho; i++) {
        printf("i: %d : %d\n", i, out_degree[i]);
    }

    return out_degree;
}

float CalculaPageRankVertice(Grafo *grafo, float*page_rank, int*out_degree, int vertice, float dumping_factor) {
    /*  Calcula a partir do grafo o page rank
    do vertice passado como parametro.
    Tambem sÃo parametros:
    page_rank[u]: o PageRank (anterior) do vertice u.
    out_degree[u]: o grau de saida do vertice u.
    dumping_factor: dumping factor a ser usado.ce */
    int i,j;
    float pageRank = 0;

    for (i = 0; i < grafo->tamanho; i++) { 
        if (grafo->matrizadj[i][vertice] > 0) { //existe aresta i --> vertice
           
            int wij = 0;
            for(j=0;j<grafo->tamanho; j++){ //somatorio dos pesos (pega todos as saidas do vertice i)
                if(grafo->matrizadj[i][j]>0)
                    wij += grafo->matrizadj[i][j];
            }
             /*Soma o pagerank de cada vertice e divide pelo out degre */
           //pageRank += grafo->matrizadj[i][vertice]*page_rank[i] / (float) out_degree[i];
            //matrizadj[i][vertice] é o peso
            pageRank += grafo->matrizadj[i][vertice]*page_rank[i] / (float) wij;
            
        }
    }

    pageRank = pageRank * dumping_factor + (1 - dumping_factor); /*Multiplica a soma encontrada pelo dumping factor*/
    printf("Page Rank vertice %d , soma: %f\n", vertice, pageRank);
    return pageRank;
}

void CalculaPageRank(Grafo*grafo, float dumping_factor) {
    /* Funcao que ira calcular o Page Rank de todos os vertices
    com a partir do CalculaPageRankVertice e ira normalizar os
    resultados ate atingirem a condicao de parada somaDifPR <=0.1 */
    int i, vertice;
    float *vetorPR;
    float *vetorPR_atual;
    vetorPR = malloc(sizeof (int)*grafo->tamanho);

    vetorPR_atual = malloc(sizeof (int)*grafo->tamanho);

    int *out_degree;
    out_degree = malloc(sizeof (int)*grafo->tamanho);

    out_degree = getVetorOutdegree(grafo);


    for (i = 0; i < grafo->tamanho; i++) {
        vetorPR[i] = 1 - dumping_factor; //inicializa o vetor
    }

    float somaDifPR = 0;

    do {

        for (vertice = 0; vertice < grafo->tamanho; vertice++) //para cada vertice associado a linha analisada..
        {
            vetorPR_atual[vertice] = CalculaPageRankVertice(grafo, vetorPR, out_degree, vertice, dumping_factor);

        }

        vetorPR_atual = normalizaVetor(vetorPR_atual, grafo->tamanho);


        somaDifPR = 0;
        for (vertice = 0; vertice < grafo->tamanho; vertice++) {
            somaDifPR += fabs(vetorPR_atual[vertice] - vetorPR[vertice]);
            vetorPR[vertice] = vetorPR_atual[vertice];
        }
    } while (somaDifPR >= 0.0001);

    for (i = 0; i < grafo->tamanho; i++) {
        grafo->vertices[i].score = vetorPR[i];
    }
    for (i = 0; i < grafo->tamanho; i++) {
        printf("Posicao i %d possui page rank de %f\n", i, grafo->vertices[i].score);
    }
    free(vetorPR);
    free(vetorPR_atual);
    free(out_degree);
}

int main() {
    Grafo grafo;
    char arquivo[] = "D:/DisciplinaUFMG/ModelThinking/Trabalho/Oficial/grafo.txt";
    obtemVertices(&grafo, arquivo);
    criaMatrizAdjacencia(&grafo, arquivo);
     /*Testa o pageRank */
    CalculaPageRank(&grafo,0.85);
    imprimeTopKPageRank(&grafo,20);
}
