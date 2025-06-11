#include <TFile.h>
#include <TTree.h>
#include <TH2I.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>  // <- Agregado
#include <TApplication.h>

TApplication theApp("App", 0, 0);

int main() {
    // Abrir archivo ROOT
    TFile *archivo = new TFile("salida_m102_filtrada.root", "READ");
    if (!archivo || archivo->IsZombie()) {
        std::cerr << "Error al abrir el archivo ROOT.\n";
        return 1;
    }

    // Obtener el árbol
    TTree *tree = (TTree*)archivo->Get("hits_validados");
    if (!tree) {
        std::cerr << "No se encontró el árbol 'hits_validados'.\n";
        return 1;
    }

    // Variables
    int barra_A, barra_B;
    tree->SetBranchAddress("barra_A", &barra_A);
    tree->SetBranchAddress("barra_B", &barra_B);

    // Crear histograma 2D
    TH2I *h2 = new TH2I("h2", "Mapa de calor - Hits en m102 - Sin Casos Especiales", 
                        12, 0.5, 12.5, 12, 0.5, 12.5);

    // Llenar el histograma
    Long64_t nEntradas = tree->GetEntries();
    for (Long64_t i = 0; i < nEntradas; ++i) {
        tree->GetEntry(i);
        h2->Fill(barra_B, barra_A);
    }

    // Configuración de estilo y canvas
    gStyle->SetOptStat(0);  // Quitar stats box
    TCanvas *c1 = new TCanvas("c1", "Mapa de Calor", 800, 600);
    h2->SetStats(0);
    h2->SetOption("COLZ");
    h2->Draw("COLZ");

    // Espera a que se cierre la ventana
    c1->Update();
    c1->SaveAs("heatmap_m102.png"); // (opcional) exportar imagen

    c1->Draw(); // Dibuja el canvas
    theApp.Run(); // Mantiene la GUI viva e interactiva
    return 0;
}

//para activar el código, escribir lo siguiente en la Terminal dentro de la carpeta con los archivos:
// g++ generar_histograma_m102.cpp $(root-config --cflags --libs) -o histograma
// ./histograma
