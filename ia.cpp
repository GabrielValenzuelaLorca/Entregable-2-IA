#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <string>

using namespace std;

//Parametros
int n_enfermeras, n_dias, n_shifts;
vector<vector<int>>cover, preferencia, sol_inicial, sol_actual;

//Restricciones blandas
int min_asig, max_asig, min_consec_shift ,max_consec_shift;
vector<vector<int>> soft,puntajes;


//Funcion de lectura de archivos de parametros
void leer_param(string nom){
    ifstream arc(nom);
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

//Funcion de lectura de archivos de soluciones blandas
int leer_soft(string nom){
    int temp_dias,temp_shifts;
    ifstream arc(nom);
    arc >> temp_dias  >> temp_shifts;
    if (temp_dias!=n_dias || temp_shifts!=n_shifts) {
        return 1;
    }
    arc >> min_asig  >> max_asig;
    arc >> min_consec_shift  >> max_consec_shift;

    soft.resize(temp_shifts);
    for (int i = 0; i < temp_shifts; i++) {
        soft[i].resize(4);
        arc >> soft[i][0] >> soft[i][1] >> soft[i][2] >> soft[i][3];
    }
    arc.close();
    return 0;
}

//Funcion para evaluar restriccion dura
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

//Funcion para evaluar restricciones blandas
vector<vector<int>> val_soft(vector<vector<int>> v){
    vector<vector<int>> soft_data(n_enfermeras);
    int a,assignments,exced,exced_dias,rupturas,puntaje;
    bool estado;

    for (int i = 0; i < n_enfermeras; i++) {
        //Enfermera i
        assignments=0,exced=0,rupturas=0,puntaje=0,estado=false;
        for (int j = 0; j < n_shifts*n_dias; j++) {
            a=v[i][j];
            //Sumar beneficio de preferencia
            puntaje+=a*preferencia[i][j];
            assignments+=a;
            if(a==1 && !estado){
                estado=true;
            }
            else if(a==1 && estado){
                exced+=1;
            }
            else{
                estado=false;
            }
        }
        //Shifts consecutivos por dias
        for (int j = 0; j < n_shifts; j++) {
            exced_dias=0,estado=false;
            for (int l = 0; l < n_dias; l++) {
                a=v[i][j+l*4];
                if (a==1 && !estado) {
                    estado=true;
                }
                else if(a==1 && estado){
                    exced_dias+=1;
                }
                else{
                    estado=false;
                }
            }
            //Puntajes por shift
            if (exced_dias<soft[j][0]) {
                rupturas++;
                puntaje-=(soft[j][0]-exced_dias);
            }
            else if (soft[j][1]<exced_dias){
                rupturas++;
                puntaje-=(exced_dias-soft[j][1]);
            }
        }
        //Asignacion de puntajes
        if (assignments<min_asig) {
            rupturas++;
            puntaje-=(min_asig-assignments);
        }
        else if (max_asig<assignments){
            rupturas++;
            puntaje-=(assignments-max_asig);
        }
        if (exced<min_consec_shift) {
            rupturas++;
            puntaje-=(min_consec_shift-exced);
        }
        else if (max_consec_shift<exced){
            rupturas++;
            puntaje-=(exced-max_consec_shift);
        }
        soft_data[i].push_back(rupturas);
        soft_data[i].push_back(puntaje);
    }
    return soft_data;
}

//Funcion para construir una solucion inicial
vector<vector<int>> construct(){
    vector<vector<int>> sol_inicial(n_enfermeras);
    int num;
    srand(time(NULL));
    num=1+rand()%(101-1);

    for (int i = 0; i < n_enfermeras; i++) {
        sol_inicial[i].resize(n_shifts*n_dias);
        for (int j = 0; j < n_shifts*n_dias; j++) {
            num=1+rand()%(101-1);
            if (num>=50) {
                sol_inicial[i][j] = 1;
            }
            else{
                sol_inicial[i][j] = 0;
            }

        }
    }
    return sol_inicial;
}

//Funcion para evaluar la calidad de la solucion


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
    leer_param("1.nsp");
    //Lectura de restricciones blandas
    if(leer_soft("1.gen")==1){
        cout<<"Los archivos no son compatibles\n";
        return 1;
    }
    //Construir solucion inicial
    sol_inicial = construct();
    puntajes=val_soft(sol_inicial);
    for (int i = 0; i < n_enfermeras; i++) {
        cout <<"Enfermera "<<i<<"-Restricciones rotas: "<< puntajes[i][0]<<", puntaje: "<<puntajes[i][1]<<"\n";
    }
    //print_matriz();
    //cout << hard(sol_inicial);

    return 0;
}
