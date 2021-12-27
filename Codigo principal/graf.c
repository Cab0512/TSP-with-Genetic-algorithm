#include <iostream>
#include <fstream>
using namespace std;

void graf(int gen) {
   TCanvas *c1 = new TCanvas("c1","Distancia",200,10,500,300);
   Double_t x[gen], y[gen];
   Int_t i=0;
   string nombreArchivo = "datos.txt";
   ifstream archivo(nombreArchivo.c_str());
   string linea;
   while (getline(archivo, linea)) {
        // Lo vamos imprimiendo
        x[i] = i;
        y[i]=atoi(linea.c_str());
        i=i+1;}
   TGraph* gr = new TGraph(gen,x,y);
   gr->SetTitle("Distancia");
   gr->Draw("AC");
   c1->SaveAs("histograma.png");
}