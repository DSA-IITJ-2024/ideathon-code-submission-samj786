#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>

struct Node {
    int atom_number;
    std::string atom_name;
    std::string residue_name;
    int residue_number;
    float x, y, z;
    Node* next;

    Node(int atom_number, const std::string& atom_name, const std::string& residue_name, int residue_number, float x, float y, float z)
        : atom_number(atom_number), atom_name(atom_name), residue_name(residue_name), residue_number(residue_number), x(x), y(y), z(z), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;

public:
    LinkedList() : head(nullptr) {}

    void append(int atom_number, const std::string& atom_name, const std::string& residue_name, int residue_number, float x, float y, float z) {

        Node* new_node = new Node(atom_number, atom_name, residue_name, residue_number, x, y, z);
        if (head == nullptr) {
            head = new_node;
        } else {
            Node* current = head;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = new_node;
        }
    }

void perform_substitution_mutation(const std::string& original_residue, const std::string& new_residue, int target_residue_number) {

        auto start = std::chrono::high_resolution_clock::now();

        Node* current = head;
        bool mutated = false;
        while (current != nullptr) {
            if (current->residue_name == original_residue && current->residue_number == target_residue_number) {
                current->residue_name = new_residue;
                current->x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;
                current->y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;
                current->z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;
                mutated = true;
            }
            current = current->next;
        }
        if (!mutated) {
            std::cerr << "Residue with number " << target_residue_number << " and name " << original_residue << " not found." << std::endl;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Time taken by perform_substitution_mutation: " << elapsed.count() << " ms" << std::endl;
    }

    void print_list() {
        Node* current = head;
        while (current != nullptr) {
            std::cout << "ATOM " << std::setw(6) << current->atom_number << " " << std::setw(4) << std::left << current->atom_name << " "
                      << std::setw(3) << std::right << current->residue_name << " " << std::setw(4) << current->residue_number
                      << "    " << std::setw(8) << std::fixed << std::setprecision(3) << current->x << " " << std::setw(8) << std::fixed << std::setprecision(3) << current->y
                      << " " << std::setw(8) << std::fixed << std::setprecision(3) << current->z << std::endl;
            current = current->next;
        }
    }

    void write_to_file(const std::string& filename) {

        auto start = std::chrono::high_resolution_clock::now();

        std::ofstream outfile(filename);
        if (!outfile) {
            std::cerr << "Failed to create output file: " << filename << std::endl;
            return;
        }

        Node* current = head;
        while (current != nullptr) {
            outfile << "ATOM " << std::setw(6) << current->atom_number << " " << std::setw(4) << std::left << current->atom_name << " "
                    << std::setw(3) << std::right << current->residue_name << " " << std::setw(4) << current->residue_number
                    << "    " << std::setw(8) << std::fixed << std::setprecision(3) << current->x << " " << std::setw(8) << std::fixed << std::setprecision(3) << current->y
                    << " " << std::setw(8) << std::fixed << std::setprecision(3) << current->z << std::endl;
            current = current->next;
        }

        outfile.close();
        std::cout << "Output written to file: " << filename << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Time taken by write_to_file: " << elapsed.count() << " ms" << std::endl;
    }

    ~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
};

int main() {

    //to seed the random number generator with the current time 
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::ifstream file("original_data.pdb");
    if (!file) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    LinkedList linked_list;
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 4) == "ATOM") {
            int atom_number = std::stoi(line.substr(6, 5));
            std::string atom_name = line.substr(12, 4);
            std::string residue_name = line.substr(17, 3);
            int residue_number = std::stoi(line.substr(22, 4));
            float x = std::stof(line.substr(30, 8));
            float y = std::stof(line.substr(38, 8));
            float z = std::stof(line.substr(46, 8));
            linked_list.append(atom_number, atom_name, residue_name, residue_number, x, y, z);
        }
    }

    std::string original_residue, new_residue;
    int target_residue_number;
    std::cout << "Enter the original residue: ";
    std::cin >> original_residue;
    std::cout << "Enter the new residue: ";
    std::cin >> new_residue;
    std::cout << "Enter the target residue sequence number: ";
    std::cin >> target_residue_number;

    linked_list.perform_substitution_mutation(original_residue, new_residue, target_residue_number);

    ///linked_list.print_list();

    std::string output_filename;
    std::cout << "Enter the output file name (e.g., output.pdb): ";
    std::cin >> output_filename;

    linked_list.write_to_file(output_filename);

    return 0;
}