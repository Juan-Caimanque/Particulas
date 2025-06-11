#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>

int binario_a_barra(unsigned int valor_decimal) {
    // Contar cuántos bits están en 1
    if (__builtin_popcount(valor_decimal) != 1)
        return -1; // más de un hit

    // Identificar posición del único bit activado (0-indexado)
    int barra = 1;
    while (valor_decimal >>= 1)
        barra++;

    return barra; // barras van del 1 al 12
}

int main() {
    // Crear archivo ROOT y árbol
    TFile *archivo_salida = new TFile("salida_m103_filtrada.root", "RECREATE");
    TTree *tree = new TTree("hits_validados", "Eventos válidos con 1 hit en A y B");

    int barra_A, barra_B;
    tree->Branch("barra_A", &barra_A, "barra_A/I");
    tree->Branch("barra_B", &barra_B, "barra_B/I");

    // Abrir archivo de entrada
    std::ifstream archivo_txt("2024_09_07_06h00_mate-m103.txt");
    if (!archivo_txt.is_open()) {
        std::cerr << "No se pudo abrir el archivo de entrada.\n";
        return 1;
    }

    std::string linea;
    while (std::getline(archivo_txt, linea)) {
        std::stringstream ss(linea);
        std::string col1, col2, col3, col4, col5, col6;

        // Separar por comas
        if (!std::getline(ss, col1, ',')) continue;
        if (!std::getline(ss, col2, ',')) continue;
        if (!std::getline(ss, col3, ',')) continue;
        if (!std::getline(ss, col4, ',')) continue;
        if (!std::getline(ss, col5, ',')) continue;
        if (!std::getline(ss, col6, ',')) continue;

        // Formar hexadecimales
        std::string hex_B = col2 + col3.substr(0, 1);         // ej. "45a"
        std::string hex_A = col3.substr(1, 1) + col4;         // ej. "40c"

        // Convertir a enteros
        unsigned int int_B = std::stoul(hex_B, nullptr, 16);  // base 16
        unsigned int int_A = std::stoul(hex_A, nullptr, 16);

        // Determinar barra activada
        int barraB = binario_a_barra(int_B);
        int barraA = binario_a_barra(int_A);

        // Guardar solo si ambos tienen exactamente un hit
        if (barraA != -1 && barraB != -1 && barraA <= 12 && barraB <= 12) {
            barra_A = barraA;
            barra_B = barraB;
            tree->Fill();
        }
    }

    // Guardar árbol y cerrar archivo
    tree->Write();
    archivo_salida->Close();

    std::cout << "Proceso terminado. Archivo 'salida_m103_filtrada.root' creado.\n";
    return 0;
}

//para activar el código, escribir lo siguiente en la Terminal dentro de la carpeta con los archivos:
//
// g++ procesar_datos_m103_muones.cpp $(root-config --cflags --libs) -o procesar
// ./procesar
