#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "systemc.h"


//tedad shahr
const int cities_num = 10;
//tedad particle ha
const int particle_num = 20;
//tedad tekrar
const int iter_num = 100;
//sabet ha
const double zarib1 = 1.0;
const double zarib2 = 2.0;
const double inersi = 0.5;
//saghf sorat
const double velo_max = 0.1;

class City {
public:
    //mokhtasat shahr
    double col;
    double row;
    //initial kardan shahr
    City(double x = 0, double y = 0) {
        this->col = x;
        this->row = y;
    }
    //mohasebe fasele to shahr
    double distanceTo(City other) {
        return sqrt((row - other.row)*(row - other.row) + (col - other.col) * (col - other.col));
    }
};

class Particle {
public:
    //fitness jari
    double fitness;
    //behtarin fitness zare
    double bestFitness;
    //sorat dar shahr
    double velocity[cities_num];
    //masir jari
    City route[cities_num];
    //behtarin masir jari
    City bestRoute[cities_num];

    //initial zare
    Particle() {

        for (int i = 0; i < cities_num; i++) {
            route[i] = City(rand() % 100, rand() % 100);
        }
        //sorat avalie 0
        memset(velocity, 0, sizeof(velocity));
        //fitness avalie binahaiat
        fitness = INFINITY;
        //behtarin masir masir jari
        memcpy(bestRoute, route, sizeof(route));
        //behtarin fitnes fitnes jari
        bestFitness = fitness;
        //cout << "particle : ==>> \n";
        //for (int i = 0; i < cities_num; i++) {
         //   cout << i<<"\n";
          //  cout << route[i].col<<"  :  "<<route[i].row << '\n';
            
        //}
    }
};

class Pso {
public:
    Particle particles[particle_num];
    City globalBestRoute[cities_num];
    double globalBestFitness;

    Pso() {

        for (int i = 0; i < particle_num; i++) {
            cout << "particle "<<i<<'\n';
            //define all particle
            particles[i] = Particle();

            //print the all particles city 
            for (int j = 0; j < cities_num; j++) {
                cout << particles[i].route[j].col << "  :  " << particles[i].route[j].row << '\n';
            }
        }
        //behtarin masir masir avalin zare
        memcpy(globalBestRoute, particles[0].route, sizeof(particles[0].route));
        //behtarin fitness global fitness avalin zare
        globalBestFitness = particles[0].fitness;
    }

    //for searching between particles fitness and update global value if they are smaller
    void updateGlobalBest() {
        for (int i = 0; i < particle_num; i++) {
            if (particles[i].fitness < globalBestFitness) {
                //update the route
                memcpy(globalBestRoute, particles[i].route, sizeof(particles[i].route));
                //update fittness
                globalBestFitness = particles[i].fitness;
            }
        }
    }

    void updateParticles() {
        for (int i = 0; i < particle_num; i++) {
            //cout << "particel : " << i << "\n";
            Particle& p = particles[i];
            for (int j = 0; j < cities_num; j++) {
                // tolis random zarib
                double first_round = (double)rand() / RAND_MAX;
                double secound_round = (double)rand() / RAND_MAX;
                //mohasebe sorat badi
                p.velocity[j] = inersi * p.velocity[j] +
                    zarib2 * secound_round * (globalBestRoute[j].col - p.route[j].col)+
                    zarib1 * first_round * (p.bestRoute[j].col - p.route[j].col);
                //mahdood kardan sorat dar soorat bishtar shodan
                if (p.velocity[j] > velo_max || p.velocity[j] < -velo_max) {
                    if (p.velocity[j] > velo_max) {
                        p.velocity[j] = velo_max;
                    }
                    else if (p.velocity[j] < -velo_max) {
                        p.velocity[j] = -velo_max;
                    }
                }
                //jabeja kardan masir
                p.route[j].col += p.velocity[j];
                p.route[j].row += p.velocity[j];
            }
            //mohasebe hazine masir jadid va update kardan fitness
            double fitness = 0;
            for (int j = 0; j < cities_num; j++) {
                int second_city = (j + 1) % cities_num;
                fitness += p.route[j].distanceTo(p.route[second_city]);
            }
            //update kardan fitnes
            p.fitness = fitness;
            //update kardan best fitnes va best route agar fitnes jadid khoob bood
            if (p.fitness < p.bestFitness) {
                memcpy(p.bestRoute, p.route, sizeof(p.route));
                p.bestFitness = p.fitness;
            }
        }
    }

    void run() {
        //run kardan be tedad iteration
        for (int i = 0; i < iter_num; i++) {
            cout << "running with iteration : " << i<<"\n";
            updateParticles();
            updateGlobalBest();
            cout << "global best fittness : " << globalBestFitness<<"\n";
            cout << "global best route : " << endl;
            cout << "[";
            for (int i = 0; i < cities_num; i++) {
                cout << "(" <<globalBestRoute[i].row << "  :  " <<globalBestRoute[i].col<< ") ,";
                    
            }
            cout << ']';
            cout << "\n"<<"\n";
        }
    }
};

// Top level module baraye run kardan narm afzar
SC_MODULE(PSO_TSP) {
    Pso ps1;

    SC_CTOR(PSO_TSP) {
        srand((unsigned int)time(NULL));
        ps1.run();
        cout << "G bfitness : " << ps1.globalBestFitness << endl;
        cout << "G broute:" << endl;
        for (int i = 0; i < cities_num; i++) {
            cout << "(" << ps1.globalBestRoute[i].row << ", " <<
                ps1.globalBestRoute[i].col << ")" << endl;
        }
    }
};
//int sc_main(int argc, char* argv[]) {
//    PSO_TSP pso("PSO_TSP");
//    sc_start();
//    return 0;
//}