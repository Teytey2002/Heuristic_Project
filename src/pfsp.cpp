#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <string>
#include <climits>
#include <iomanip> // pour setprecision



using namespace std;

// Structure pour stocker les données du problème
struct PFSPInstance {
    int numJobs;
    int numMachines;
    vector<vector<int>> processingTimes;
};

// Fonction pour lire une instance depuis un fichier
PFSPInstance readInstance(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << endl;
        exit(EXIT_FAILURE);
    }

    PFSPInstance instance;
    file >> instance.numJobs >> instance.numMachines;

    instance.processingTimes.resize(instance.numJobs, vector<int>(instance.numMachines));

    for (int i = 0; i < instance.numJobs; ++i) {
        for (int j = 0; j < instance.numMachines; ++j) {
            int machine_id, processing_time;
            file >> machine_id >> processing_time;

            // Optionnel : vérifie que machine_id == j+1
            if (machine_id != j + 1) {
                cerr << "Erreur: identifiant de machine inattendu à la ligne " << i + 2 << endl;
                exit(EXIT_FAILURE);
            }

            instance.processingTimes[i][j] = processing_time;
        }
    }

    return instance;
}

// Génération d'une permutation aléatoire
vector<int> generateRandomPermutation(int n, unsigned int seed = 0) {
    vector<int> permutation(n);
    for (int i = 0; i < n; ++i) {
        permutation[i] = i;
    }

    if (seed == 0) {
        // Graine aléatoire (basée sur l'horloge)
        seed = chrono::system_clock::now().time_since_epoch().count();
    }

    shuffle(permutation.begin(), permutation.end(), default_random_engine(seed));
    return permutation;
}

// Heuristique Simplifiée RZ
vector<int> generateSRZPermutation(const PFSPInstance& instance) {
    int n = instance.numJobs;
    int m = instance.numMachines;

    // Étape 1 : Calcul de la somme des temps pour chaque job
    vector<pair<int, int>> jobSum; // (somme, index)
    for (int i = 0; i < n; ++i) {
        int sum = accumulate(instance.processingTimes[i].begin(), instance.processingTimes[i].end(), 0);
        jobSum.emplace_back(sum, i);
    }

    // Étape 2 : Tri stable des jobs selon leur somme de temps de traitement
    std::stable_sort(jobSum.begin(), jobSum.end());

    // Étape 3 : Construction de la permutation par insertion incrémentale
    vector<int> solution;

    for (const auto& job : jobSum) {
        int jobIndex = job.second;
        int bestPos = 0;
        int bestCompletion = INT_MAX;

        // Tester chaque position possible dans la solution actuelle
        for (size_t pos = 0; pos <= solution.size(); ++pos) {
            vector<int> temp = solution;
            temp.insert(temp.begin() + pos, jobIndex);

            // Calcul du coût (somme des C_i)
            vector<int> machineTime(m, 0);
            int totalCompletion = 0;

            for (size_t i = 0; i < temp.size(); ++i) {
                int jID = temp[i];
                machineTime[0] += instance.processingTimes[jID][0];
                for (int j = 1; j < m; ++j) {
                    machineTime[j] = max(machineTime[j], machineTime[j - 1]) + instance.processingTimes[jID][j];
                }
                totalCompletion += machineTime[m - 1];
            }

            if (totalCompletion < bestCompletion) {
                bestCompletion = totalCompletion;
                bestPos = pos;
            }
        }

        // Insérer définitivement le job à la meilleure position
        solution.insert(solution.begin() + bestPos, jobIndex);
    }

    return solution;
}


// Calcul de la fonction coût (somme des temps de fin de traitement)
int computeCompletionTime(const PFSPInstance& instance, const vector<int>& permutation) {
    int n = instance.numJobs;
    int m = instance.numMachines;

    vector<int> machineTime(m, 0);  // machineTime[j] = temps sur la machine j après le dernier job
    int totalCompletionTime = 0;

    for (int i = 0; i < n; ++i) {
        int job = permutation[i];

        // première machine
        machineTime[0] += instance.processingTimes[job][0];

        // les autres machines
        for (int j = 1; j < m; ++j) {
            machineTime[j] = max(machineTime[j], machineTime[j - 1]) + instance.processingTimes[job][j];
        }

        // le C_i du job i est le temps sur la dernière machine
        totalCompletionTime += machineTime[m - 1];
    }

    return totalCompletionTime;
}


//localSearch_first_improvement() avec gestion dynamique du voisinage (transpose, exchange, insert)
vector<int> localSearch_first_improvement(const PFSPInstance& instance, vector<int> permutation, const string& neighborhood) {
    int n = instance.numJobs;
    bool improved = true;

    while (improved) {
        improved = false;
        int currentCost = computeCompletionTime(instance, permutation);

        for (int i = 0; i < n && !improved; ++i) {
            for (int j = 0; j < n && !improved; ++j) {
                if (i == j) continue; //évite les comparaisons inutiles.

                vector<int> neighbor = permutation;

                if (neighborhood == "exchange" && i < j) {  // échange les jobs i et j
                    swap(neighbor[i], neighbor[j]);
                } 
                else if (neighborhood == "transpose" && j == i + 1) { // échange deux jobs adjacents i et i+1
                    swap(neighbor[i], neighbor[j]);
                } 
                else if (neighborhood == "insert") {    // déplace un job i vers une autre position j
                    int job = neighbor[i];
                    neighbor.erase(neighbor.begin() + i);
                    neighbor.insert(neighbor.begin() + j, job);
                } 
                else {
                    continue; // on ignore les paires invalides selon le voisinage
                }

                int newCost = computeCompletionTime(instance, neighbor);
                if (newCost < currentCost) {
                    permutation = neighbor;
                    improved = true;
                }
            }
        }
    }

    return permutation;
}


vector<int> localSearch_best_improvement(const PFSPInstance& instance, vector<int> permutation, const string& neighborhood) {
    int n = instance.numJobs;
    bool improved = true;

    while (improved) {
        improved = false;
        int currentCost = computeCompletionTime(instance, permutation);

        vector<int> bestNeighbor = permutation;
        int bestCost = currentCost;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i == j) continue;

                vector<int> neighbor = permutation;

                if (neighborhood == "exchange" && i < j) {
                    swap(neighbor[i], neighbor[j]);
                } 
                else if (neighborhood == "transpose" && j == i + 1) {
                    swap(neighbor[i], neighbor[j]);
                } 
                else if (neighborhood == "insert") {
                    int job = neighbor[i];
                    neighbor.erase(neighbor.begin() + i);
                    neighbor.insert(neighbor.begin() + j, job);
                } 
                else {
                    continue;
                }

                int newCost = computeCompletionTime(instance, neighbor);
                if (newCost < bestCost) {
                    bestCost = newCost;
                    bestNeighbor = neighbor;
                }
            }
        }

        // Si on a trouvé un voisin meilleur, on l'adopte
        if (bestCost < currentCost) {
            permutation = bestNeighbor;
            improved = true;
        }
    }

    return permutation;
}

// Fonction de recherche locale VND (Variable Neighborhood Descent)
vector<int> localSearchVND(const PFSPInstance& instance, const vector<int>& initialSolution, const vector<string>& neighborhoods) {
    vector<int> current = initialSolution;
    bool improvement = true;

    while (improvement) {
        improvement = false;

        for (const string& neigh : neighborhoods) {
            vector<int> newSol = localSearch_first_improvement(instance, current, neigh);

            if (computeCompletionTime(instance, newSol) < computeCompletionTime(instance, current)) {
                current = newSol;
                improvement = true;
                break; // on retourne au début de la séquence des voisinages
            }
        }
    }

    return current;
}


// Fonction principale pour tester la lecture et la génération
int main(int argc, char* argv[]) {
    unsigned int seed = 0;
    bool saveResults = false;
    string pivoting_rule = "";
    string neighborhood = "";
    string init_method = "";
    string filename = "";

    // Parcourir tous les arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--first") pivoting_rule = "first";
        else if (arg == "--best") pivoting_rule = "best";
        else if (arg == "--vnd1") pivoting_rule = "VND1";   // VND1 = transpose, exchange, insert
        else if (arg == "--vnd2") pivoting_rule = "VND2";   // VND2 = transpose, insert, exchange
        else if (arg == "--transpose") neighborhood = "transpose";
        else if (arg == "--exchange") neighborhood = "exchange";
        else if (arg == "--insert") neighborhood = "insert";
        else if (arg == "--random-init") init_method = "random";
        else if (arg == "--srz") init_method = "srz";
        else if (arg.rfind("--seed=", 0) == 0) {
            seed = stoi(arg.substr(7)); // extrait la valeur après --seed=
        }
        else if (arg == "--save") saveResults = true;
        else if (arg[0] != '-') filename = arg; // Si ce n’est pas une option, on suppose que c’est le fichier d’instance

        else {
            cerr << "Erreur: Argument inconnu " << arg << endl;
            return EXIT_FAILURE;
        }
    }

    // Vérifier que le fichier a bien été spécifié
    if (filename.empty()) {
        cerr << "Erreur: Veuillez spécifier un fichier d'instance." << endl;
        return EXIT_FAILURE;
    }
    
    // Afficher la configuration
    cout << "Configuration choisie : " << endl;
    cout << " - Fichier instance : " << filename << endl;
    cout << " - Pivotement : " << pivoting_rule << endl;
    cout << " - Voisinage : " << neighborhood << endl;
    cout << " - Méthode d'initialisation : " << init_method << endl;
    cout << " - Graine : " << seed << endl;

    //PFSPInstance instance = readInstance(argv[1]);
    PFSPInstance instance = readInstance(filename);


    cout << "Instance chargée : " << instance.numJobs << " jobs, " << instance.numMachines << " machines." << endl;

    vector<int> permutation;
    if (init_method == "random") {
        permutation = generateRandomPermutation(instance.numJobs);
    } else if (init_method == "srz") {
        permutation = generateSRZPermutation(instance);
    } else {
        cerr << "Erreur: méthode d'initialisation inconnue." << endl;
        return EXIT_FAILURE;
    }

    // Local Search 
    auto start = chrono::high_resolution_clock::now();  //Start timer
    if (pivoting_rule == "first") {
        permutation = localSearch_first_improvement(instance, permutation, neighborhood);
    } else if (pivoting_rule == "best") {
        permutation = localSearch_best_improvement(instance, permutation, neighborhood);
    } else if (pivoting_rule == "VND1") {
        vector<string> vnd1_order = {"transpose", "exchange", "insert"};
        permutation = localSearchVND(instance, permutation, vnd1_order);
    } else if (pivoting_rule == "VND2") {
        vector<string> vnd2_order = {"transpose", "insert", "exchange"};
        permutation = localSearchVND(instance, permutation, vnd2_order);
    }
    
    auto end = chrono::high_resolution_clock::now();    //End timer
    chrono::duration<double> elapsed = end - start;
    double elapsedTime = elapsed.count(); // en secondes

    // Calcul du temps d'achèvement
    int completionTime = computeCompletionTime(instance, permutation);
    cout << "Temps d'achèvement total : " << completionTime << endl;

    if (saveResults) {  // Put all the results in a CSV file
        ofstream fout("./results/resultscoucou.csv", ios::app); // mode append
        fout << filename << ","
             << instance.numJobs << ","
             << pivoting_rule << ","
             << neighborhood << ","
             << init_method << ","
             << seed << ","
             << completionTime << ","
             << fixed << setprecision(6) << elapsedTime << endl;
    }

    return 0;
}
