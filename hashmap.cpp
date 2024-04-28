#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <chrono>

struct AtomInfo {
    std::string atom_name;
    std::string residue_name;
    float x, y, z;
};

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

std::unordered_map<std::pair<int, int>, AtomInfo, hash_pair> atom_map;
std::vector<std::pair<int, int>> atom_order;

void parse_pdb_file(const std::string& filename) {

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

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

            AtomInfo atom_info = {atom_name, residue_name, x, y, z};
            atom_map[std::make_pair(residue_number, atom_number)] = atom_info;
            atom_order.push_back(std::make_pair(residue_number, atom_number));
        }
    }

    file.close();
}

void perform_substitution_mutation(const std::string& original_residue, const std::string& new_residue, int target_residue_number) {

    auto start = std::chrono::high_resolution_clock::now();

    for (auto& [key, atom_info] : atom_map) {
        int residue_number = key.first;
        if (atom_info.residue_name == original_residue && residue_number == target_residue_number) {
            atom_info.residue_name = new_residue;
            // Update coordinates
            atom_info.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f; 
            atom_info.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f; 
            atom_info.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f; 
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Time taken by perform_substitution_mutation: " << elapsed.count() << " ms" << std::endl;

}

void write_to_file(const std::string& filename) {

    auto start = std::chrono::high_resolution_clock::now();
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cerr << "Failed to create output file: " << filename << std::endl;
        return;
    }

    for (const auto& key : atom_order) {
        const auto& atom_info = atom_map[key];
        int atom_number = key.second;
        int residue_number = key.first;
        outfile << "ATOM " << std::setw(6) << atom_number << " " << std::setw(4) << std::left << atom_info.atom_name << " "
                << std::setw(3) << std::right << atom_info.residue_name << " " << std::setw(4) << residue_number
                << "    " << std::setw(8) << std::fixed << std::setprecision(3) << atom_info.x << " " << std::setw(8) << std::fixed << std::setprecision(3) << atom_info.y
                << " " << std::setw(8) << std::fixed << std::setprecision(3) << atom_info.z << std::endl;
    }

    outfile.close();
    std::cout << "Output written to file: " << filename << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Time taken by write_to_file: " << elapsed.count() << " ms" << std::endl;
}

int main() {
    std::string input_filename;
    std::cout << "Enter the input file name (e.g., data.pdb.txt): ";
    std::cin >> input_filename;

    parse_pdb_file(input_filename);

    std::string original_residue, new_residue;
    int target_residue_number;
    std::cout << "Enter the original residue: ";
    std::cin >> original_residue;
    std::cout << "Enter the new residue: ";
    std::cin >> new_residue;
    std::cout << "Enter the target residue sequence number: ";
    std::cin >> target_residue_number;

    perform_substitution_mutation(original_residue, new_residue, target_residue_number);

    std::string output_filename;
    std::cout << "Enter the output file name (e.g., output.pdb): ";
    std::cin >> output_filename;

   write_to_file(output_filename);

    return 0;
}