#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <string>

using namespace std;

//Parametros
int n_enfermeras, n_dias, n_shifts, puntaje_maximo;
vector<vector<int>>cover, preferencia, sol_inicial;
vector<int> hard_data;

//Restricciones blandas
int min_asig, max_asig, min_consec_shift ,max_consec_shift;
vector<vector<int>> soft,soft_data;


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
vector<int> hard(vector<vector<int>> v){
    int pivote,cont,diff=0,rest=0;
    vector<int> arr;
    for (int i = 0; i <n_dias ; i++) {
        for (int j = 0; j < n_shifts; j++) {
            pivote = cover[i][j];
            cont=0;
            for (int l = 0; l < n_enfermeras; l++) {
                cont += v[l][4*i+j];
            }
            if (cont<pivote) {
                rest++;
                diff=diff+1000*(pivote-cont);
            }
            //Evaluar restriccion blanda de asignaciones por shift
            if(cont<soft[j][0]){
                rest++;
                diff+=(soft[j][0]-cont);
            }
            else if(soft[j][1]<cont){
                rest++;
                diff+=(cont-soft[j][1]);
            }
        }
    }
    arr.push_back(rest);
    arr.push_back(diff);
    return arr;
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

//Funcion para obtener puntaje de restricciones blandas
int soft_score(vector<vector<int>> v){
    int cont=0;
    for (int i = 0; i < n_enfermeras; i++) {
        cont+=v[i][1];
    }
    return cont;
}

//Buscar mejor vecino
bool neighborhood(){
    vector<vector<int>> temp;
    int cont,prev_i=0,prev_j=0,max=0,max_i,max_j;
    bool termino=false;
    for (int i = 0; i < n_enfermeras; i++) {
        for (int j = 0; j < n_shifts*n_dias; j++) {
            if (prev_i!=i || prev_j!=j) {
                sol_inicial[prev_i][prev_j]=abs(sol_inicial[prev_i][prev_j]-1);
                prev_i=i,prev_j=j;
            }
            sol_inicial[i][j]=abs(sol_inicial[i][j]-1);
            cont = soft_score(val_soft(sol_inicial))-hard(sol_inicial)[1];
            if(puntaje_maximo<cont && max<cont){
                max=cont;
                //sol_actual=sol_inicial;
                termino = true;
                max_i=i,max_j=j;
                //cout << "Nuevo puntaje: "<< cont << "\n";
            }
        }
    }
    sol_inicial[prev_i][prev_j]=abs(sol_inicial[prev_i][prev_j]-1);
    if(termino==true){
        sol_inicial[max_i][max_j]=abs(sol_inicial[max_i][max_j]-1);
        puntaje_maximo=max;
    }
    return termino;
}


//borrar porfavor
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
    int iter=0,rest=0;
    //Lectura de parametros .nsp
    leer_param("1.nsp");
    //Lectura de restricciones blandas
    if(leer_soft("1.gen")==1){
        cout<<"Los archivos no son compatibles\n";
        return 1;
    }
    //Construir solucion inicial
    sol_inicial = construct();
    //Setear datos de la solucion inicial
    soft_data=val_soft(sol_inicial);
    hard_data=hard(sol_inicial);
    puntaje_maximo=(soft_score(soft_data)-hard_data[1]);
    //Buscar mejor vecino
    while (neighborhood()) {
        iter++;
    }
    ofstream fs("output.txt");
    fs << "\t";
    for (int i = 0; i < n_shifts*n_dias; i++) {
        fs << "S" << (i%4)+1 << "\t";
    }
    fs << "\n";
    for (int i = 0; i < n_enfermeras; i++) {
        fs << "n" << i+1 << "\t";
        for (int j = 0; j < n_shifts*n_dias; j++) {
            fs << sol_inicial[i][j]<<"\t";
        }
        fs << "\n";
    }
    soft_data=val_soft(sol_inicial);
    hard_data=hard(sol_inicial);
    for (int i = 0; i < n_enfermeras; i++) {
        rest+=soft_data[i][0];
    }
    rest+=hard_data[0];
    fs << rest << "\t";
    for (int i = 0; i < n_enfermeras; i++) {
        fs << soft_data[i][0] << "\t";
    }
    fs << "\n" <<puntaje_maximo<<"\t";
    for (int i = 0; i < n_enfermeras; i++) {
        fs << soft_data[i][1] << "\t";
    }
    fs << "\n";
    fs.close();
    cout<<"Fin!\n";

    return 0;
}
