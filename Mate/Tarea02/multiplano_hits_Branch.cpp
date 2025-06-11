// Librerías necesarias
#include <TFile.h>       // Para manejar archivos ROOT
#include <TTree.h>       // Para usar árboles (TTree) de ROOT
#include <iostream>      // Para entrada/salida estándar (cout, cin)
#include <fstream>       // Para leer archivos de texto
#include <sstream>       // Para dividir líneas con stringstream
#include <bitset>        // Para operaciones con bits (opcional aquí)
#include <string>        // Para usar strings
#include <vector>        // Para usar vectores dinámicos

// ✅ Función que convierte un número con un solo bit encendido al número de barra correspondiente
int binario_a_barra(unsigned int valor_decimal) {
    // Si el número tiene más de un bit en 1, no es válido → retorna -1
    if (__builtin_popcount(valor_decimal) != 1)
        return -1; // más de un hit

    // Cuenta cuántas veces puede desplazarse a la derecha hasta llegar a cero → eso da el número de barra
    int barra = 1;
    while (valor_decimal >>= 1) barra++;
    return barra;
}

int main() {
    // ✅ Se le pide al usuario que ingrese la fecha del archivo que quiere leer
    std::string fecha;
    std::cout << "Ingrese fecha que quiere leer - Ejemplo: \"2024_09_07\": ";
    std::cin >> fecha;

    // ✅ Abrir los 3 archivos de texto (uno por plano detector)
    std::vector<std::ifstream> archivos(3);         // Vector de archivos de entrada
    std::vector<std::string> nombres(3);            // Vector de nombres de archivo
    for (int i = 0; i < 3; ++i) {
        nombres[i] = fecha + "_06h00_mate-m10" + std::to_string(i + 1) + ".txt";  // Ej: "2024_09_07_06h00_mate-m101.txt"
        archivos[i].open(nombres[i]);                 // Abrir el archivo correspondiente
        if (!archivos[i].is_open()) {                 // Verificar si se abrió correctamente
            std::cerr << "Error al abrir el archivo: " << nombres[i] << "\n";
            return 1;                                 // Terminar programa si falla la apertura
        }
    }

    // ✅ Crear archivo de salida ROOT y el árbol (TTree) donde se almacenan los eventos válidos
    std::string nombre_salida = "salida_" + fecha + "_hits_branch.root";
    TFile *archivo_salida = new TFile(nombre_salida.c_str(), "RECREATE");
    TTree *tree = new TTree("hits_validados_y_hits_Branch", "Eventos válidos con 1 hit por plano");

    // ✅ Variables que guardarán el número de barra para cada lado (A y B) y cada plano (1, 2 y 3)
    int barra_A_1, barra_B_1;
    int barra_A_2, barra_B_2; 
    int barra_A_3, barra_B_3;
    int multi_hits_A_1, multi_hits_B_1;
    int multi_hits_A_2, multi_hits_B_2;
    int multi_hits_A_3, multi_hits_B_3;

    // ✅ Se crean las ramas del árbol para guardar los valores anteriores
    tree->Branch("barra_A_1", &barra_A_1, "barra_A_1/I");
    tree->Branch("barra_B_1", &barra_B_1, "barra_B_1/I");
    tree->Branch("barra_A_2", &barra_A_2, "barra_A_2/I");
    tree->Branch("barra_B_2", &barra_B_2, "barra_B_2/I");
    tree->Branch("barra_A_3", &barra_A_3, "barra_A_3/I");
    tree->Branch("barra_B_3", &barra_B_3, "barra_B_3/I");
    tree->Branch("multi_hits_A_1", &multi_hits_A_1, "multi_hits_A_1/I");
    tree->Branch("multi_hits_B_1", &multi_hits_B_1, "multi_hits_B_1/I");
    tree->Branch("multi_hits_A_2", &multi_hits_A_2, "multi_hits_A_2/I");
    tree->Branch("multi_hits_B_2", &multi_hits_B_2, "multi_hits_B_2/I");
    tree->Branch("multi_hits_A_3", &multi_hits_A_3, "multi_hits_A_3/I");
    tree->Branch("multi_hits_B_3", &multi_hits_B_3, "multi_hits_B_3/I");

    // ✅ Leer simultáneamente una línea de cada archivo (una por plano)
    std::string lineas[3];
    while (std::getline(archivos[0], lineas[0]) &&
           std::getline(archivos[1], lineas[1]) &&
           std::getline(archivos[2], lineas[2])) {

        // Arrays para guardar los números de barra convertidos de cada plano y lado
        int barrasA[3], barrasB[3], mhitsA[3], mhitsB[3];

        // ✅ Recorremos los tres planos
        for (int i = 0; i < 3; ++i) {
            std::stringstream ss(lineas[i]);  // Separamos los valores de la línea actual con ','
            std::string col1, col2, col3, col4, col5, col6;

            // Se extraen 6 columnas del archivo. Si alguna falla, salta a la siguiente iteración del while
            if (!std::getline(ss, col1, ',')) goto continuar;
            if (!std::getline(ss, col2, ',')) goto continuar;
            if (!std::getline(ss, col3, ',')) goto continuar;
            if (!std::getline(ss, col4, ',')) goto continuar;
            if (!std::getline(ss, col5, ',')) goto continuar;
            if (!std::getline(ss, col6, ',')) goto continuar;

            // ✅ Extraer los datos hexadecimales relevantes:
            std::string hex_B = col2 + col3.substr(0, 1);         // Combina parte de col2 y primer carácter de col3
            std::string hex_A = col3.substr(1, 1) + col4;         // Combina segundo carácter de col3 y col4

            // ✅ Convertir los datos hexadecimales a enteros (decimal)
            unsigned int int_B = std::stoul(hex_B, nullptr, 16);
            unsigned int int_A = std::stoul(hex_A, nullptr, 16);

            // ✅ Decodificar qué barra se activó usando la función binario_a_barra
            barrasA[i] = binario_a_barra(int_A);
            barrasB[i] = binario_a_barra(int_B);
            mhitsA[i] = binario_a_barra(int_A);
            mhitsB[i] = binario_a_barra(int_B);

            // Si alguno de los lados tiene más de un hit o está fuera de rango, se descarta este evento
            if (barrasA[i] == -1 || barrasB[i] == -1 ||
                barrasA[i] > 12 || barrasB[i] > 12)
                goto continuar;
        }

        // ✅ Si todo fue válido, se asignan los valores decodificados a las variables que irán al TTree
        barra_A_1 = barrasA[0]; barra_B_1 = barrasB[0];
        barra_A_2 = barrasA[1]; barra_B_2 = barrasB[1];
        barra_A_3 = barrasA[2]; barra_B_3 = barrasB[2];
        multi_hits_A_1 = mhitsA[0]; multi_hits_B_1 = mhitsB[0];
        multi_hits_A_2 = mhitsA[1]; multi_hits_B_2 = mhitsB[1];
        multi_hits_A_3 = mhitsA[2]; multi_hits_B_3 = mhitsB[2];

        // ✅ Guardar el evento en el árbol
        tree->Fill();

    continuar:
        continue;  // Vuelve al siguiente ciclo de lectura
    }

    // ✅ Cerrar todos los archivos de entrada
    for (int i = 0; i < 3; ++i) archivos[i].close();

    // ✅ Escribir y cerrar el archivo ROOT de salida
    tree->Write();
    archivo_salida->Close();

    // ✅ Confirmación al usuario
    std::cout << "Proceso completado. Archivo '" << nombre_salida << "' creado.\n";
    return 0;
}

// ------------------------------------------------------------------------
// NOTA 1:

// Para ejecutar este código, abre la terminal en la carpeta de este archivo
// Activa el entorno rootenv y pega las siguientes líneas de código:

// g++ multiplano_hits_Branch.cpp $(root-config --cflags --libs) -o multi_hitsBranch
// ./multi_hitsBranch

// ------------------------------------------------------------------------
// NOTA 2:

// Para hacer de una el Hist 2d desde root debes hacer lo siguiente:

// Abre la terminal en la carpeta del archivo de salida
// En el entorno rootenv esqcribe root
// una vez dentro de root pega estas líneas de código:

// TFile *f = TFile::Open("salida_2024_09_07_hits_branch.root");  // Usa tu archivo real
// TTree *t = (TTree*)f->Get("hits_validados_y_hits_Branch");
// t->Draw("multi_hits_A_2:multi_hits_B_2 >> h2d(13,0.5,13.5,13,0.5,13.5)", "", "COLZ");
// c1->SaveAs("heatmap_mhA2_vs_mhB2.png");