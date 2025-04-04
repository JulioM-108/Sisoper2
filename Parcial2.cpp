#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <iomanip>

// Prototipos de funciones
// executeProcess: Simula la ejecución de un proceso en una cola.
// writeResults: Escribe las métricas de la simulación en un archivo que se llama salida.txt.
// processFile: Lee el archivo de entrada y ejecuta la simulación.
bool executeProcess(std::queue<int> &cola, std::vector<class Process> &processes, int &time, int &completed, int quantum, std::vector<bool> &enCola, std::queue<int> &cola1, std::queue<int> &cola2, std::queue<int> &cola3);
void writeResults(const std::vector<class Process> &processes);
void processFile(const std::string &fileName);

// Se utiliza la clase Process que representa cada proceso con sus atributos y métricas.
class Process {
public:
    std::string label;     
    int burstTime;         
    int arrivalTime;       
    int queueLevel;        
    int priority;          // Prioridad interna (aunque este no se utiliza en los algoritmos de las colas).
    
    // Variables utilizadas durante la simulación
    int TiempoRestante;     // Tiempo restante para ejecutar
    int TiempoInicio;         // Momento en que el proceso recibe la CPU por primera vez
    int completionTime;    // Tiempo de finalización del proceso
    int waitingTime;       // Tiempo total de espera en la cola
    int turnaroundTime;    // Tiempo total en el sistema (CT - AT)
    int responseTime;      // Tiempo en recibir la primera CPU (TiempoInicio - arrivalTime)
    bool iniciado;          // Indica si el proceso ya fue atendido por primera vez

    // Constructor que inicializa el proceso con sus valores y variables de simulación
    Process(const std::string &l, int bt, int at, int q, int pr)
        : label(l), burstTime(bt), arrivalTime(at), queueLevel(q), priority(pr),
          TiempoRestante(bt), TiempoInicio(-1), completionTime(0),
          waitingTime(0), turnaroundTime(0), responseTime(0),
          iniciado(false) {}
};

// Función que por cada línea del archivo de entrada, organiza la información en bloques que pueda ser ejecutado y 
// tal y como "p1;20;0;1;5 y crea un objeto Process, que es la clase de arriba.
// Es de resaltar que esta función ignora líneas vacías o que comienzan con "#".
bool ProcessLine(const std::string &line, Process &proc) {
    if (line.empty() || line[0] == '#')
        return false;
    std::stringstream ss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(ss, token, ';')) {
        token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
        if (!token.empty())
            tokens.push_back(token);
    }
    if (tokens.size() < 5)
        return false;
    // Se crea el proceso utilizando los valores extraídos del archivo de entrada.
    proc = Process(tokens[0], std::stoi(tokens[1]), std::stoi(tokens[2]),
                   std::stoi(tokens[3]), std::stoi(tokens[4]));
    return true;
}

// Esta función revisa los procesos que han llegado y los encola en su respectiva cola, de acuerdo a su nivel (queueLevel).
void checkArrivals(const std::vector<Process> &processes, int time, std::vector<bool> &enCola,
                   std::queue<int> &cola1, std::queue<int> &cola2, std::queue<int> &cola3) {
    int n = processes.size();
    for (int i = 0; i < n; i++) {
        if (!enCola[i] && processes[i].arrivalTime <= time) {
            if (processes[i].queueLevel == 1)
                cola1.push(i);
            else if (processes[i].queueLevel == 2)
                cola2.push(i);
            else
                cola3.push(i);
            enCola[i] = true;
        }
    }
}

// Esta es la función principal que procesa el archivo de entrada y simula la planificación.
// En este se lee el archivo, se estructuran los procesos y se simula la ejecución según MLQ.
// Cola 1: RR (quantum = 3), Cola 2: RR (quantum = 5), Cola 3: FCFS (ejecución completa).
void processFile(const std::string &fileName) {
    std::ifstream inFile(fileName);
    if (!inFile) {
        std::cerr << "Error al abrir el archivo: " << fileName << std::endl;
        return;
    }
    
    std::vector<Process> processes;
    std::string line;
    while (std::getline(inFile, line)) {
        Process proc("", 0, 0, 0, 0);
        if (ProcessLine(line, proc))
            processes.push_back(proc);
    }
    inFile.close();
    
    // Se ordenan los procesos por tiempo de llegada.
    std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.arrivalTime < b.arrivalTime;
    });
    
    // Se declaran las colas para cada nivel.
    std::queue<int> cola1, cola2, cola3;
    int time = 0, completed = 0, n = processes.size();
    std::vector<bool> enCola(n, false);
    
    // Se verifican las llegadas iniciales.
    checkArrivals(processes, time, enCola, cola1, cola2, cola3);
    
    // Se utiliza un bucle de simulación: se ejecuta hasta que todos los procesos finalicen.
    while (completed < n) {
        bool executed = false;
        // Se da preferencia a la cola de mayor prioridad (cola 1 > cola 2 > cola 3).
        if (!cola1.empty())
            executed = executeProcess(cola1, processes, time, completed, 3, enCola, cola1, cola2, cola3);
        else if (!cola2.empty())
            executed = executeProcess(cola2, processes, time, completed, 5, enCola, cola1, cola2, cola3);
        else if (!cola3.empty())
            executed = executeProcess(cola3, processes, time, completed, -1, enCola, cola1, cola2, cola3);
        
        // Si ninguna cola tenía procesos listos, avanzar el tiempo y volver a verificar llegadas.
        if (!executed) {
            time++;
            checkArrivals(processes, time, enCola, cola1, cola2, cola3);
        }
    }
    
    // Se escriben los resultados en el archivo de salida.
    writeResults(processes);
}

// Esta es la función que simula la ejecución de un proceso.
// Si el quantum es -1, el proceso se ejecuta en modo FCFS (hasta terminar).
// Se simula la ejecución unidad por unidad para poder verificar nuevos procesos durante la ejecución.
bool executeProcess(std::queue<int> &cola, std::vector<Process> &processes, int &time, int &completed, int quantum,
                    std::vector<bool> &enCola, std::queue<int> &cola1, std::queue<int> &cola2, std::queue<int> &cola3) {
    int idx = cola.front(); // Se obtien el índice del proceso al frente de la cola.
    cola.pop();
    Process &proc = processes[idx];
    
    // Si es la primera vez que el proceso recibe CPU, se establece TiempoInicio y responseTime.
    if (!proc.iniciado) {
        proc.TiempoInicio = time;
        proc.responseTime = time - proc.arrivalTime;
        proc.iniciado = true;
    }
    
    // Si el quantum == -1, se ejecuta el proceso hasta finalizar (FCFS).
    if (quantum == -1) {
        while (proc.TiempoRestante > 0) {
            time++;                  // Se avanza una unidad de tiempo.
            proc.TiempoRestante--;    // Se decrementa el tiempo restante del proceso.
            // Se verifica si han llegado nuevos procesos en este nuevo tiempo.
            checkArrivals(processes, time, enCola, cola1, cola2, cola3);
        }
    } else {
        int executedUnits = 0;
        // Se ejecuta hasta que se agote el quantum o el proceso termine.
        while (executedUnits < quantum && proc.TiempoRestante > 0) {
            time++;                  // Se avanza una unidad de tiempo.
            proc.TiempoRestante--;    // Se decrementa el tiempo restante del proceso.
            executedUnits++;         // Se acumula las unidades ejecutadas en este quantum.
            // Se verifica en cada unidad de tiempo si llegan nuevos procesos.
            checkArrivals(processes, time, enCola, cola1, cola2, cola3);
        }
    }
    
    // Si el proceso terminó, se calculan las métricas finales.
    if (proc.TiempoRestante == 0) {
        proc.completionTime = time;
        proc.turnaroundTime = proc.completionTime - proc.arrivalTime;
        proc.waitingTime = proc.turnaroundTime - proc.burstTime;
        completed++;
    } else {
        // Si el proceso no terminó, se vuelve a poner en cola en la misma cola.
        cola.push(idx);
    }
    return true;
}

// Esta es la función que escribe los resultados de la simulación en el archivo "salida.txt".
// Se muestran las métricas de cada proceso y los promedios finales.
void writeResults(const std::vector<Process> &processes) {
    std::ofstream outFile("salida.txt");
    if (!outFile) {
        std::cerr << "Error al crear el archivo de salida." << std::endl;
        return;
    }
    outFile << "# Etiquetas; BT; AT; Q; Pr; WT; CT; RT; TAT" << std::endl;
    double totalWT = 0, totalCT = 0, totalRT = 0, totalTAT = 0;
    int n = processes.size();
    
    // Se escriben los datos de cada proceso en el archivo.
    for (const auto &proc : processes) {
        outFile << proc.label << ";" << proc.burstTime << ";" << proc.arrivalTime << ";" << proc.queueLevel << ";" 
                << proc.priority << ";" << proc.waitingTime << ";" << proc.completionTime << ";" << proc.responseTime << ";"
                << proc.turnaroundTime << std::endl;
        totalWT += proc.waitingTime;
        totalCT += proc.completionTime;
        totalRT += proc.responseTime;
        totalTAT += proc.turnaroundTime;
    }
    // Se calculan y muestran los promedios de las métricas con una precisión de 1 decimal.
    outFile << std::fixed << std::setprecision(1);
    outFile << "\nWT=" << totalWT / n << "; CT=" << totalCT / n
            << "; RT=" << totalRT / n << "; TAT=" << totalTAT / n << ";" << std::endl;
    outFile.close();
    std::cout << "Simulación completada. El resultado puede verse desde el archivo salida.txt." << std::endl;
}

int main() {
    // Se solicita al usuario el nombre del archivo de entrada.
    std::string fileName;
    std::cout << "Ingrese por favor el nombre del archivo de texto de entrada: ";
    std::cin >> fileName;
    processFile(fileName);
    return 0;
}
