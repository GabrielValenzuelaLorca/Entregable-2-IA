#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

//Parametros
int n_enfermeras, n_dias, n_shifts;
vector<vector<int>>cover, preferencia, sol_inicial, sol_actual;

//Funcion para restriccion dura
int hard(vector<vector<int>> v){
    int pivote,cont,diff=0;
    for (int i = 0; i <n_dias ; i++) {
        for (int j = 0; j < n_shifts; j++) {
            pivote = cover[i][j];

            cont=0;
            for (int l = 0; l < n_enfermeras; l++) {
                cont += v[l][4*i+j];
            }
            if (cont<pivote) {
                diff=diff+(pivote-cont);
            }
        }
    }
    return diff;
}

//Funcion de lectura de archivos a optimizar
void leer_param(){
    //#####################################SOLICITAR NOMBRE ARCHIVO################################
    ifstream arc("1.nsp");
    arc >> n_enfermeras >> n_dias >> n_shifts;

    cover.resize(n_dias);
    preferencia.resize(n_enfermeras);

    for (int i = 0; i < n_dias; i++) {
        cover[i].resize(n_shifts);
        for (int j = 0; j < n_shifts; j++) {
            arc >> cover[i][j];
        }
    }

    for (int i = 0; i < n_enfermeras; i++) {
        preferencia[i].resize(n_shifts*n_dias);
        for (int j = 0; j < n_shifts*n_dias; j++) {
            arc >> preferencia[i][j];
        }
    }

    arc.close();
}

vector<vector<int>> construct(){
    vector<vector<int>> sol_inicial(n_enfermeras);
    int num;
    srand(time(NULL));
    num=1+rand()%(101-1);
    //Probar haciendo alguna heuristica como abarcar los cubrimientos minimos

    for (int i = 0; i < n_enfermeras; i++) {
        sol_inicial[i].resize(n_shifts*n_dias);
        for (int j = 0; j < n_shifts*n_dias; j++) {
            num=1+rand()%(101-1);
            if (num>=60) { //Revisar porcentaje
                sol_inicial[i][j] = 1;
            }
            else{
                sol_inicial[i][j] = 0;
            }

        }
    }
    return sol_inicial;
}

int print_matriz(){
    for (int i = 0; i < n_enfermeras; i++) {
        for (int j = 0; j < n_shifts*n_dias; j++) {
            cout << sol_inicial[i][j];
        }

        cout<<"\n";
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    //Lectura de parametros .nsp
    leer_param();
    //Construir solucion inicial
    sol_inicial = construct();
    //print_matriz();
    //cout << hard(sol_inicial);

    return 0;
}
