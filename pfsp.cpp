#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <string>

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
            file >> instance.processingTimes[i][j];
        }
    }

    return instance;
}

// Génération d'une permutation aléatoire
vector<int> generateRandomPermutation(int n) {
    vector<int> permutation(n);
    for (int i = 0; i < n; ++i) {
        permutation[i] = i;
    }
    shuffle(permutation.begin(), permutation.end(), default_random_engine(chrono::system_clock::now().time_since_epoch().count()));
    return permutation;
}

// Heuristique Simplifiée RZ
vector<int> generateSRZPermutation(const PFSPInstance& instance) {
    int n = instance.numJobs;
    vector<pair<int, int>> jobSum; // (somme des temps, index du job)

    // Calcul de la somme des temps de traitement pour chaque job
    for (int i = 0; i < n; ++i) {
        int sum = accumulate(instance.processingTimes[i].begin(), instance.processingTimes[i].end(), 0);
        jobSum.emplace_back(sum, i);
    }

    // Trier les jobs en ordre croissant de leur somme de temps de traitement
    sort(jobSum.begin(), jobSum.end());

    // Construire la solution en insérant chaque job à la meilleure position
    vector<int> solution;
    for (auto& job : jobSum) {
        int jobIndex = job.second;
        int bestPos = 0;
        int bestCompletionTime = INT_MAX;

        for (size_t pos = 0; pos <= solution.size(); ++pos) {
            vector<int> tempSolution = solution;
            tempSolution.insert(tempSolution.begin() + pos, jobIndex);

            int completionTime = 0;
            vector<vector<int>> completion(instance.numJobs, vector<int>(instance.numMachines, 0));

            completion[0][0] = instance.processingTimes[tempSolution[0]][0];
            for (int j = 1; j < instance.numMachines; ++j)
                completion[0][j] = completion[0][j - 1] + instance.processingTimes[tempSolution[0]][j];

            for (size_t i = 1; i < tempSolution.size(); ++i) {
                completion[i][0] = completion[i - 1][0] + instance.processingTimes[tempSolution[i]][0];
                for (int j = 1; j < instance.numMachines; ++j)
                    completion[i][j] = max(completion[i - 1][j], completion[i][j - 1]) + instance.processingTimes[tempSolution[i]][j];
            }

            completionTime = completion[tempSolution.size() - 1][instance.numMachines - 1];

            if (completionTime < bestCompletionTime) {
                bestCompletionTime = completionTime;
                bestPos = pos;
            }
        }

        solution.insert(solution.begin() + bestPos, jobIndex);
    }

    return solution;
}

// Calcul de la fonction coût (somme des temps de fin de traitement)
int computeCompletionTime(const PFSPInstance& instance, const vector<int>& permutation) {
    int numJobs = instance.numJobs;
    int numMachines = instance.numMachines;
    
    vector<vector<int>> completionTime(numJobs, vector<int>(numMachines, 0));

    // Remplissage de la première machine
    completionTime[0][0] = instance.processingTimes[permutation[0]][0];
    for (int j = 1; j < numMachines; ++j) {
        completionTime[0][j] = completionTime[0][j-1] + instance.processingTimes[permutation[0]][j];
    }

    // Remplissage des autres machines
    for (int i = 1; i < numJobs; ++i) {
        completionTime[i][0] = completionTime[i-1][0] + instance.processingTimes[permutation[i]][0];
        for (int j = 1; j < numMachines; ++j) {
            completionTime[i][j] = max(completionTime[i-1][j], completionTime[i][j-1]) + instance.processingTimes[permutation[i]][j];
        }
    }

    return completionTime[numJobs-1][numMachines-1]; // Retourne le temps d'achèvement total
}

// Fonction principale pour tester la lecture et la génération
int main(int argc, char* argv[]) {
    string pivoting_rule = "first";
    string neighborhood = "transpose";
    string init_method = "random";

    // Lire les arguments de la ligne de commande
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--first") pivoting_rule = "first";
        if (arg == "--best") pivoting_rule = "best";
        if (arg == "--transpose") neighborhood = "transpose";
        if (arg == "--exchange") neighborhood = "exchange";
        if (arg == "--insert") neighborhood = "insert";
        if (arg == "--random-init") init_method = "random";
        if (arg == "--srz") init_method = "srz";
    }

    cout << "Configuration choisie : " << endl;
    cout << " - Pivotement : " << pivoting_rule << endl;
    cout << " - Voisinage : " << neighborhood << endl;
    cout << " - Méthode d'initialisation : " << init_method << endl;

    if (argc < 2) {
        cerr << "Utilisation: " << argv[0] << " <fichier_instance>" << endl;
        return EXIT_FAILURE;
    }

    PFSPInstance instance = readInstance(argv[1]);

    cout << "Instance chargée : " << instance.numJobs << " jobs, " << instance.numMachines << " machines." << endl;

    // Génération d'une permutation aléatoire
    vector<int> permutation = generateRandomPermutation(instance.numJobs);
    cout << "Permutation aléatoire : ";
    for (int job : permutation) {
        cout << job << " ";
    }
    cout << endl;

    // Calcul du temps d'achèvement
    int completionTime = computeCompletionTime(instance, permutation);
    cout << "Temps d'achèvement total : " << completionTime << endl;

    return 0;
}
