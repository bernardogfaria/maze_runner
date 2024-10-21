#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>

// Definição do tipo para o labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma coordenada no labirinto
struct Coord {
    int x, y;
};

// Variáveis globais para armazenar o labirinto e informações auxiliares
Maze lab;
int linhas, colunas;
std::stack<Coord> caminho;
std::mutex pilha_mutex; // Mutex para sincronização do acesso à pilha

// Função para carregar o labirinto a partir de um arquivo
Coord carregar_labirinto(const std::string& nome_arquivo) {
    std::ifstream arquivo(nome_arquivo);
    if (!arquivo) {
        std::cerr << "Erro: Não foi possível abrir o arquivo." << std::endl;
        return {-1, -1};
    }

    arquivo >> linhas >> colunas;
    if (linhas <= 0 || colunas <= 0) {
        std::cerr << "Erro: Linhas ou colunas inválidas." << std::endl;
        return {-1, -1};
    }

    lab.assign(linhas, std::vector<char>(colunas));
    Coord inicio = {-1, -1};

    for (int i = 0; i < linhas; ++i) {
        for (int j = 0; j < colunas; ++j) {
            arquivo >> lab[i][j];
            if (lab[i][j] == 'e') {
                inicio = {i, j};
            }
        }
    }

    if (inicio.x == -1 || inicio.y == -1) {
        std::cerr << "Erro: Posição inicial 'e' não encontrada." << std::endl;
    }

    return inicio;
}

// Função para exibir o labirinto no console
void exibir_labirinto() {
    for (const auto& linha : lab) {
        for (char celula : linha) {
            std::cout << celula;
        }
        std::cout << '\n';
    }
}

// Verifica se uma coordenada é válida para movimento
bool posicao_valida(int x, int y) {
    return x >= 0 && x < linhas && y >= 0 && y < colunas &&
           (lab[x][y] == 'x' || lab[x][y] == 's');
}

// Explora o labirinto recursivamente
void explorar(Coord atual) {
    if (lab[atual.x][atual.y] == 's') {
        std::cout << "Saída encontrada em: (" << atual.x << ", " << atual.y << ")" << std::endl;
        return;
    }

    lab[atual.x][atual.y] = '.'; // Marca a posição como visitada
    exibir_labirinto();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Movimentos possíveis: cima, baixo, esquerda, direita
    Coord movimentos[] = {
        {atual.x - 1, atual.y},
        {atual.x + 1, atual.y},
        {atual.x, atual.y - 1},
        {atual.x, atual.y + 1}};

    std::vector<Coord> proximos;

    for (const auto& mov : movimentos) {
        if (posicao_valida(mov.x, mov.y)) {
            proximos.push_back(mov);
        }
    }

    if (proximos.size() > 1) {
        std::vector<std::thread> threads;
        for (size_t i = 1; i < proximos.size(); ++i) {
            threads.emplace_back(explorar, proximos[i]);
        }

        explorar(proximos[0]);

        for (auto& t : threads) {
            t.join();
        }
    } else if (!proximos.empty()) {
        explorar(proximos[0]);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso correto: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Coord inicio = carregar_labirinto(argv[1]);
    if (inicio.x == -1 || inicio.y == -1) {
        std::cerr << "Erro: Posição inicial inválida." << std::endl;
        return 1;
    }

    explorar(inicio);

    return 0;
}
