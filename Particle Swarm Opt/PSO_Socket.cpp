#include<systemc.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <cmath>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class Node {
public:
	int index;
	double x;
	double y;
	Node() {
		this->index = -1;
		this->x = 0;
		this->y = 0;
	}
	Node(int _index, double _x, double _y) {
		this->index = _index;
		this->x = _x;
		this->y = _y;
	}
	double distance_to(const Node& other) {
		int x = this->x - other.x;
		int y = this->y - other.y;
		double val = std::sqrt((x * x) + (y * y));
		return val;
	}
	bool operator==(const Node& rhs) {
		if (this->index == rhs.index) {
			return true;
		}
		return false;
	}
};

class Velocity {
public:
	int size;
	std::vector<int> from;
	std::vector<int> to;
	Velocity() {
		this->size = 0;
	}
	Velocity(const Velocity& v) {
		this->size = 0;
		for (int i = 0; i < v.size; i++) {
			this->add_transposition(v.from[i], v.to[i]);
		}
	}
	Velocity(std::vector<int> _from, std::vector<int> _to) {
		this->size = 0;
		for (int i = 0; i < _from.size(); i++) {
			this->add_transposition(_from[i], _to[i]);
		}
	}
	void add_transposition(int a, int b) {
		this->size += 1;
		this->from.push_back(a);
		this->to.push_back(b);
	}
	void remove_transposition(int index) {
		this->size -= 1;
		this->from.erase(this->from.begin() + index);
		this->to.erase(this->to.begin() + index);
	}
	std::string to_string() {
		std::stringstream ss;

		ss << "(";
		if (this->size > 0) {
			ss << "(" << this->from[0] << ", " << this->to[0] << ")";
			for (int i = 1; i < this->size; i++) {
				ss << ", (" << this->from[i] << ", " << this->to[i] << ")";
			}
		}
		ss << ")";

		return ss.str();
	}
	Velocity& operator=(const Velocity& rhs) {
		//self assignment check
		if (this != &rhs) {
			//deallocate everything
			this->to.clear();
			this->from.clear();
			this->size = 0;

			//copy values
			for (int i = 0; i < rhs.size; i++) {
				this->add_transposition(rhs.from[i], rhs.to[i]);
			}
		}
		return *this;
	}
	Velocity& operator*=(const double& rhs) {
		double d = rhs;
		//std::cout << "d: " << d << std::endl;
		std::vector<int> from(this->from);
		std::vector<int> to(this->to);

		this->from.clear();
		this->to.clear();
		int size = this->size;
		this->size = 0;

		if (0 != d) {
			if (0 > d) {
				//std::cout << "d: is negative " << std::endl;
				d = d * -1;
				std::reverse(from.begin(), from.end());
				std::reverse(to.begin(), to.end());
			}

			if (0 < d) {
				while (d >= 2) {
					for (int i = 0; i < size; i++) {
						this->add_transposition(from[i], to[i]);
					}
					d--;
					//std::cout << "subbed d: " << d << std::endl;
				}

				if (d >= 1) { d--; }

				if (d > 0) {
					int cv = ceil(d * size);
					//std::cout << "cv: " << cv << std::endl;
					for (int i = 0; i < cv; i++) {
						this->add_transposition(from[i], to[i]);
					}
				}
			}
		}

		return *this;
	}
	Velocity& operator+=(const Velocity& rhs) {
		for (int i = 0; i < rhs.size; i++) {
			this->add_transposition(rhs.from[i], rhs.to[i]);
		}
		return *this;
	}
	Velocity operator*(const double& rhs) {
		Velocity ret(*this);
		ret *= rhs;
		return ret;
	}
	Velocity operator+(const Velocity& rhs) {
		Velocity ret(*this);
		ret += rhs;
		return ret;
	}

};

class Position {
public:
	std::vector<Node> nodes;
	Position() {}
	Position(const Position& p) {
		for (int i = 0; i < p.nodes.size(); i++) {
			this->add_node(p.nodes[i]);
		}
	}
	Position& add_node(Node new_n) {
		this->nodes.push_back(new_n);
		return *this;
	}
	std::string to_string() {
		std::stringstream ss;

		ss << "(";
		ss << this->nodes[0].index;
		for (int i = 1; i < this->nodes.size(); i++) {
			ss << ", " << this->nodes[i].index;
		}
		ss << ")";

		return ss.str();
	}
	Position& operator=(const Position& rhs) {
		if (this != &rhs) {
			this->nodes.clear();

			for (int i = 0; i < rhs.nodes.size(); i++) {
				this->add_node(rhs.nodes[i]);
			}
		}
		return *this;
	}
	Position& operator+=(const Velocity& v) {
		//std::cout << "  Inside += position before: " << this->to_string() << std::endl;
		for (int i = 0; i < v.size; i++) {
			Node tmp = nodes[v.from[i]];
			nodes[v.from[i]] = nodes[v.to[i]];
			nodes[v.to[i]] = tmp;
		}
		return *this;
		//std::cout << "  Inside += position after: " << this->to_string() << std::endl;
	}
	Position operator+(const Velocity& v) {
		Position ret(*this);
		ret += v;
		return ret;
	}
	Velocity operator-(const Position& p) {
		Velocity difference;
		Position tmp_pos(p);

		for (int i = 0; i < this->nodes.size(); i++) {
			Node look_for = this->nodes[i];
			int found_at = -1;
			for (int j = 0; j < tmp_pos.nodes.size(); j++) {
				if (tmp_pos.nodes[j] == look_for) {
					found_at = j;

					Node tmp = p.nodes[i];
					tmp_pos.nodes[i] = tmp_pos.nodes[j];
					tmp_pos.nodes[j] = tmp;
					break;
				}
			}

			if (i != found_at && found_at != -1) {
				difference.add_transposition(i, found_at);
			}
		}

		return difference;
	}
};

class Particle {
public:
	Velocity velocity;
	Position position;
	Position best_found;
	double best_value;
	Position best_position;

	//These should add up to 1.0
	double self_trust;
	double past_trust;
	double global_trust;
	Particle(double self_trust, double past_trust, double global_trust) {
		this->self_trust = self_trust;
		this->past_trust = past_trust;
		this->global_trust = global_trust;
		this->best_value = -1;
	}


	double move() { //Move based on velocity, return best value seen
		//std::cout << "Velocity: " << velocity.to_string() << std::endl;
		position += velocity;
		//std::cout << "New position: " << position.to_string() << std::endl;

		double new_value = this->calculate_value();
		//std::cout << new_value << std::endl;
		if (new_value < this->best_value || this->best_value < 0) {
			this->best_value = new_value;
			this->best_position = position;
		}

		return this->best_value;
	}
	double calculate_value()  //Get sum of wieghts of the edges we're using
	{
		double value = 0;

		int node_count = this->position.nodes.size();

		for (int i = 0; i < node_count; i++) {
			Node tmp = this->position.nodes[i];
			Node tmp2;

			if (i + 1 < node_count) {
				tmp2 = this->position.nodes[i + 1];
			}
			else {
				tmp2 = this->position.nodes[0];
			}
			double tmp_val = tmp.distance_to(tmp2);
			value += tmp_val;
		}
		return value;
	}
	void calculate_new_velocity(Position global_best) {
		Velocity a;
		if (this->velocity.size > 0) {
			a = (this->velocity * this->self_trust);
		}
		Velocity b = ((this->best_position - this->position) * this->past_trust);
		Velocity c = ((global_best - this->position) * this->global_trust);

		//std::cout << a.to_string();
		this->velocity = Velocity(a + b + c);
	}
};

class Swarm {
public:
	std::vector<Particle> particles;
	std::vector<Node> nodes;
private:
	double self_trust;
	double past_trust;
	double global_trust;
	int particle_count;
public:
	Swarm(int particle_count, float self_trust, float past_trust, float global_trust) {
		this->particle_count = particle_count;
		this->self_trust = self_trust;
		this->past_trust = past_trust;
		this->global_trust = global_trust;
	}
};

/********************************************************************************************/

Position best_position;
double best_value;

int moves_since_best_changed;
bool best_changed_glob;

// Sum these three must be less than 1
double SELF_TRUST = 0.2;
double PAST_TRUST = 0.3;
double GLOBAL_TRUST = 0.4;

//int PARTICLE_COUNT = 76;
//std::string GRAPH_FILE_PATH("D:\\University\\Hardware Software\\PSO Final\\tests\\eil76.tsp");

int PARTICLE_COUNT = 51;
std::string GRAPH_FILE_PATH("D:\\University\\Hardware Software\\PSO Final\\tests\\eil51.tsp");

//int PARTICLE_COUNT = 318;
//std::string GRAPH_FILE_PATH("D:\\University\\Hardware Software\\PSO Final\\tests\\lin318.tsp");

Swarm swarm(PARTICLE_COUNT, SELF_TRUST, PAST_TRUST, GLOBAL_TRUST);

SOCKET sock;

/********************************************************************************************/

SC_MODULE(Hardware_py) {

	void solve() {
		init();

		char buffer[1024] = { 0 };
		std::string message = std::to_string(best_value) + ", "
			+ std::to_string(moves_since_best_changed) + ", "
			+ get_best(); // best_value, moves_since_best_changed, best_changed_glob
		
		// Send Base Data to software
		int iResult = send(sock, message.c_str(), message.length(), 0);
		if (iResult == SOCKET_ERROR) {
			std::cerr << "send failed: " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			return;
		}
		std::cout << "Sent to software: " << message << std::endl;

		// Wait for ACK (ok)
		iResult = recv(sock, buffer, 1024, 0);
		if (iResult > 0) {
			std::cout << "Received from software: " << buffer << std::endl;
		}
		else if (iResult == 0) {
			std::cout << "Connection closed" << std::endl;
			return;
		}
		else {
			std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
			return;
		}

		while (true) {
			
			if (moves_since_best_changed < 2) {
				// Normall Search
				bool best_changed = false;
				double tmp;
				for (int i = 0; i < swarm.particles.size(); i++) {
					swarm.particles[i].calculate_new_velocity(best_position);

					tmp = swarm.particles[i].move();
					if (best_value > tmp) {
						best_value = tmp;
						best_position = swarm.particles[i].position;
						best_changed = true;
					}
				}

				best_changed_glob = best_changed;
			}
			else {
				// lazy descent
				if (moves_since_best_changed < 4) {
					bool best_changed = false;
					int maximum_moves = swarm.nodes.size();

					//Move all particles to their last best position
					particles_back_to_best();

					//Search around their best's, moving slowly.  Stop if best changes.
					int count = 0;
					while (count < maximum_moves && !best_changed) {
						best_changed = move_all_slowly();
						count++;
					}

					best_changed_glob = best_changed;
				}
				else {
					//energetic descent
					bool best_changed = true;
					int maximum_moves = swarm.nodes.size();

					//Move all particles to their last position
					particles_back_to_best();

					//Move slowly around bests as long as we find a better solution in
					  //under maximum_moves
					while (best_changed) {
						best_changed = false;
						for (int i = 0; i < maximum_moves; i++) {
							best_changed = move_all_slowly();
						}
					}

					best_changed_glob = best_changed;
				}
			}
			
			// Send Data
			message = std::to_string(best_value) + ", "
				+ std::to_string(moves_since_best_changed) + ", "
				+ get_best(); // best_value, moves_since_best_changed, best_changed_glob

			// Send Base Data to software
			int iResult = send(sock, message.c_str(), message.length(), 0);
			if (iResult == SOCKET_ERROR) {
				std::cerr << "send failed: " << WSAGetLastError() << std::endl;
				closesocket(sock);
				WSACleanup();
				return;
			}
			std::cout << "Sent to software: " << message << std::endl;

			// Wait for ACK (ok)
			iResult = recv(sock, buffer, 1024, 0);
			if (iResult > 0) {
				std::cout << "Received from software: " << buffer << std::endl;
			}
			else if (iResult == 0) {
				std::cout << "Connection closed" << std::endl;
				return;
			}
			else {
				std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
				return;
			}

			moves_since_best_changed = buffer[3] - '0';
		}

		cleanup();
		cout << "End Hardware" << endl;
	}
	void particles_back_to_best() {
		for (int i = 0; i < swarm.particles.size(); i++) {
			swarm.particles[i].position = swarm.particles[i].best_position;
		}
	}
	bool move_all_slowly() {
		bool best_changed = false;
		for (int i = 0; i < swarm.particles.size(); i++) {
			Velocity v;
			int a = rand() % swarm.nodes.size();
			int b = rand() % swarm.nodes.size();

			v.add_transposition(a, b);
			swarm.particles[i].velocity = v;
			double val = swarm.particles[i].move();

			if (val < best_value) {
				best_value = val;
				best_position = swarm.particles[i].position;
				best_changed = true;
			}
		}
		return best_changed;
	}
	void init() {
		std::ifstream graph_file;
		std::string line;
		int index;
		double real;
		
		swarm.nodes.clear();
		
		graph_file.open(GRAPH_FILE_PATH.c_str());
		if (graph_file.is_open()) {
			while (graph_file.good()) {
				getline(graph_file, line);
				//std:: cout << "  " << line << std::endl;
				//First find NODE_COORD_SECTION
				line = trim(line);
		
				if (line == "NODE_COORD_SECTION") {
					break;
				}
			}
		
			//Read in nodes
			while (graph_file.good()) {
				Node n;
		
				getline(graph_file, line);
		
				std::istringstream iss(line, std::istringstream::in);
				iss >> n.index;
				iss >> n.x;
				iss >> n.y;
		
				//Check for invalid line
				if (n.index > 0) {
					n.index--; //We index from 0, file indexes from 1
		
					swarm.nodes.push_back(n);
				}
			}
		}
		else {
			std::cout << "Could not open file: " << GRAPH_FILE_PATH << std::endl;
			throw(-1);
		}
		
		assign_particle_positions();
	}
	void assign_particle_positions() {
		swarm.particles.clear();
		for (int i = 0; i < PARTICLE_COUNT; i++) {
			swarm.particles.push_back(Particle(SELF_TRUST, PAST_TRUST, GLOBAL_TRUST));
		
			swarm.particles[i].position = shuffle();
		
			double cur_value = swarm.particles[i].calculate_value();
		
			if (i == 0 || best_value > cur_value) {
				best_value = cur_value;
				best_position = swarm.particles[i].position;
			}
		}
	}
	Position shuffle() {
		Position p;
		std::vector<Node> new_vec(swarm.nodes);
		
		//Knuth-Fisher-Yates shuffle
		for (int i = new_vec.size() - 1; i > 0; i--) {
			int n = rand() % (i + 1);
			Node tmp = new_vec[i];
			new_vec[i] = new_vec[n];
			new_vec[n] = tmp;
		}
		
		p.nodes = new_vec;
		return p;
	}
	std::string trim(std::string s) {
		s.erase(0, s.find_first_not_of(" \t"));
		s.erase(s.find_last_not_of(" \t") + 1);
		return s;
	}
	void print_position(Position p) {
		std::cout << "  (" << p.nodes[0].index;
		for (int i = 1; i < p.nodes.size(); i++) {
			std::cout << ", " << p.nodes[i].index;
		}
		std::cout << ")" << std::endl;
	}
	
	void cleanup() {
		// Clean up
		closesocket(sock);
		WSACleanup();
	}

	std::string get_best() {
		if (best_changed_glob == false)
			return "false";
		else
			return "true";
	}

	SC_CTOR(Hardware_py) {
		SC_THREAD(solve);
	}
};

int sc_main(int argc, char* argv[]) {
	srand(time(0));
	
	WSADATA wsaData;
	sock = INVALID_SOCKET;
	struct addrinfo* result = NULL, * ptr = NULL, hints;


	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cerr << "WSAStartup failed: " << iResult << std::endl;
		return -1;
	}

	// Set up the hints structure
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the software address and port
	iResult = getaddrinfo("m-abbasabadi-pc", "12344", &hints, &result);
	if (iResult != 0) {
		std::cerr << "getaddrinfo failed: " << iResult << std::endl;
		WSACleanup();
		return -1;
	}

	// Attempt to connect to the software
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		// Create a socket
		sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sock == INVALID_SOCKET) {
			std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			WSACleanup();
			return -1;
		}

		// Connect to the software
		iResult = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (sock == INVALID_SOCKET) {
		std::cerr << "Unable to connect to software" << std::endl;
		WSACleanup();
		return -1;
	}
	std::string message("ok");
	iResult = send(sock, message.c_str(), message.length(), 0);
	if (iResult == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return(0);
	}

	Hardware_py hw("Hardware");
	
	sc_start(1, SC_SEC);
	return(0);
}