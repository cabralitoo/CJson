#include <iostream>
#include <fstream>
#include "json.hpp"
#include <string>
#include <filesystem>
#include <vector>
#include <windows.h> // Para usar GetAsyncKeyState

using namespace std;
using json = nlohmann::json;

void add(json &obj, const string &nome, int idade)
{
    obj[nome] = idade;
}

void save(const string &filename)
{
    ofstream configfile("config/last.txt");
    if (configfile.is_open())
    {
        configfile << filename;
        configfile.close();
    }
    else
    {
        cout << "Erro ao salvar arquivo de configuração" << endl;
    }
}

bool isEmptyOrInvalidJson(const string &filename)
{
    ifstream jsonFile(filename);
    if (!jsonFile.is_open())
    {
        return true;
    }

    string content((istreambuf_iterator<char>(jsonFile)), istreambuf_iterator<char>());
    return content.empty() || content == "{}";
}

bool load(json &arch, string &filename)
{
    ifstream configFile("config/last.txt");

    if (configFile.is_open())
    {
        getline(configFile, filename);
        configFile.close();

        if (!filename.empty())
        {
            if (!filename.ends_with(".json"))
            {
                filename += ".json";
            }

            if (filesystem::exists(filename))
            {
                // Verifica se o JSON está vazio ou inválido
                if (isEmptyOrInvalidJson(filename))
                {
                    cout << "Arquivo JSON vazio ou inválido. Criando novo arquivo com '{}'." << endl;
                    ofstream jsonFile(filename);
                    jsonFile << "{}";
                    jsonFile.close();
                }

                ifstream jsonFile(filename);
                if (jsonFile.is_open())
                {
                    jsonFile >> arch;
                    jsonFile.close();
                    cout << "Arquivo JSON carregado: " << filename << endl;
                    return true;
                }
            }
        }
        cout << "Arquivo JSON inválido ou inexistente." << endl;
    }
    else
    {
        cout << "Arquivo de configuração não encontrado." << endl;
    }
    return false;
}

int main()
{
    // Criar diretório caso não exista
    std::filesystem::create_directory("config");

    json arch;
    string escolha;
    string filename;

    cout << "Seja bem-vindo(a) ao Norbe" << endl;

    if (!load(arch, filename))
    {
        vector<string> jsonFiles;

        // Procura por arquivos .json
        for (const auto &entry : filesystem::directory_iterator("."))
        {
            if (entry.path().extension() == ".json")
            {
                jsonFiles.push_back(entry.path().filename().string());
            }
        }

        if (jsonFiles.empty())
        {
            cout << "Nenhum arquivo .json encontrado. Criando um novo arquivo..." << endl;
            cout << "Digite o nome do arquivo JSON (sem a extensão .json): ";
            getline(cin, filename);
            if (!filename.ends_with(".json"))
            {
                filename += ".json";
            }
            ofstream newfile(filename);
            if (newfile.is_open())
            {
                newfile << "{}";
                newfile.close();
                save(filename);
            }
            else
            {
                cout << "Ocorreu um erro ao salvar o arquivo JSON. Tente novamente mais tarde." << endl;
                return 1;
            }
        }
        else if (jsonFiles.size() == 1)
        {
            filename = jsonFiles[0];
            cout << "Arquivo JSON encontrado: " << filename << endl;
            ifstream jsonFile(filename);
            string content((istreambuf_iterator<char>(jsonFile)),
                            istreambuf_iterator<char>());
            jsonFile.close();

            // Se o conteúdo for inválido, substitui por {}
            if (content.empty() || content == "{}")
            {
                ofstream newfile(filename);
                if (newfile.is_open())
                {
                    newfile << "{}"; // Cria um arquivo JSON vazio
                    newfile.close();
                }
            }

            // Agora tenta carregar o arquivo
            ifstream file(filename);
            file >> arch;
            file.close();
            save(filename);
        }
        else
        {
            cout << "Arquivos .json encontrados:" << endl;

            // Exibe a lista inicial de arquivos
            int selection = 0;
            for (size_t i = 0; i < jsonFiles.size(); ++i)
            {
                if (i == selection)
                {
                    cout << ">> ";
                }
                else
                {
                    cout << "   ";
                }
                cout << jsonFiles[i] << endl;
            }

            // Permite seleção com teclas
            bool done = false;
            while (!done)
            {
                if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState(VK_RETURN) & 0x8000 || GetAsyncKeyState(VK_ESCAPE) & 0x8000)
                {
                    system("cls");
                    cout << "Arquivos .json encontrados:" << endl;

                    if (GetAsyncKeyState(VK_UP) & 0x8000)
                    {
                        if (selection > 0)
                            selection--;
                    }
                    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
                    {
                        if (selection < jsonFiles.size() - 1)
                            selection++;
                    }
                    else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
                    {
                        filename = jsonFiles[selection];
                        cout << "Arquivo selecionado: " << filename << endl;

                        // Verifica se o arquivo JSON é válido
                        if (isEmptyOrInvalidJson(filename))
                        {
                            cout << "Arquivo JSON vazio ou inválido. Criando novo arquivo com '{}'." << endl;
                            ofstream jsonFile(filename);
                            jsonFile << "{}";
                            jsonFile.close();
                        }

                        done = true;

                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
                    {
                        done = true;
                    }

                    for (size_t i = 0; i < jsonFiles.size(); ++i)
                    {
                        if (i == selection)
                        {
                            cout << ">> ";
                        }
                        else
                        {
                            cout << "   ";
                        }
                        cout << jsonFiles[i] << endl;
                    }
                    Sleep(100);
                }
            }

            if (!filename.empty())
            {
                ifstream jsonFile(filename);
                jsonFile >> arch;
                jsonFile.close();
                save(filename);
            }
        }
    }

    while (true)
    {
        cout << "Comandos: 'new' para adicionar usuário, 'tab' para listar usuários, 'sair' para encerrar" << endl;
        getline(cin, escolha);
        if (escolha == "new")
        {
            string nome;
            int idade;
            cout << "Nome do usuário: ";
            getline(cin, nome);
            cout << "Idade do usuário: ";
            cin >> idade;
            cin.ignore();
            add(arch, nome, idade);
            ofstream arquivo(filename);
            if (arquivo.is_open())
            {
                arquivo << arch.dump(4);
                arquivo.close();
            }
            else
            {
                cout << "Houve um erro ao salvar o usuário." << endl;
            }
        }
        else if (escolha == "tab")
        {
            if (arch.empty())
            {
                cout << "Nenhum usuário cadastrado" << endl;
            }
            else
            {
                cout << "--- Lista de usuários ---" << endl;
                for (auto &item : arch.items())
                {
                    cout << item.key() << ": " << item.value() << endl;
                }
            }
        }
        else if (escolha == "sair")
        {
            break;
        }
        else
        {
            cout << "Comando não reconhecido" << endl;
        }
    }

    return 0;
}
