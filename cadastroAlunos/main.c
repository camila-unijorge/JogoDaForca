#include <stdio.h>
#define ALUNOS_LEN 5

struct Aluno {
    char nome[64];
    int idade;
    float nota1;
    float nota2;
    float media;
};

struct Aluno alunos[ALUNOS_LEN];
int alunos_len;

float notacorte = 7.0f;

int main(void) {

    printf("Quantos alunos desejadas? ");
    scanf(" %d", &alunos_len);

    if (alunos_len > 5) {
        printf("O sistema só permite registros entre 1 e 5!");
    }

    for (int i = 0; i < ALUNOS_LEN; i++) {

        printf("CADA%d\n", i);

    }

    return 0;
}