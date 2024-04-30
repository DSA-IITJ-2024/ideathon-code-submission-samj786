#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>


struct Atom {
    std::string line;
    std::string residue_name;
};

// Function to parse PDB file and store atoms in a vector
std::vector<Atom> parsePDB(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Atom> atoms;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.substr(0, 4) == "ATOM") {
                Atom atom;
                atom.line = line;
                atom.residue_name = line.substr(17, 3);
                atoms.push_back(atom);
            }
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
    }

    return atoms;
}

// Function to write atoms to a new PDB file
void writePDB(const std::string& filename, const std::vector<Atom>& atoms) {
    std::ofstream file(filename);
    if (file.is_open()) {
        int atomNumber = 1; // Start with atom number 1
        for (const Atom& atom : atoms) {
            std::string line = atom.line;
            line.replace(6, 5, std::to_string(atomNumber));
            file << line << '\n';
            atomNumber++;
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file " << filename << " for writing" << std::endl;
    }
}

// Function to find the index of the first atom of the residue with the specified sequence number and residue name
size_t findResidueIndex(const std::vector<Atom>& atoms, int sequenceNumber, const std::string& residueName) {
    for (size_t i = 0; i < atoms.size(); ++i) {
        if (std::stoi(atoms[i].line.substr(22, 4)) == sequenceNumber && atoms[i].residue_name == residueName) {
            return i;
        }
    }
    return atoms.size(); // Return the end index if not found
}

int main() {
    // Parse original PDB file
    std::vector<Atom> atoms = parsePDB("original_data.pdb");

    // Identify and remove rows corresponding to the specified residue and sequence number
    std::string removeResidue;
    int removeSequenceNumber;
    std::cout << "Enter the residue to remove: ";
    std::cin >> removeResidue;
    std::cout << "Enter the sequence number of the residue to remove: ";
    std::cin >> removeSequenceNumber;

    // Find the index of the first atom of the residue to be removed
    size_t deletionIndex = findResidueIndex(atoms, removeSequenceNumber, removeResidue);

    if (deletionIndex == atoms.size()) {
        std::cerr << "Error: Residue " << removeResidue << " with sequence number " << removeSequenceNumber << " not found" << std::endl;
        return 1;
    }

    std::vector<Atom> newAtoms;
    newAtoms.reserve(atoms.size() + 1000); // Reserve some extra space for efficiency

    // Add atoms before the deleted residue
    for (size_t i = 0; i < deletionIndex; ++i) {
        newAtoms.push_back(atoms[i]);
    }

    // Read atoms corresponding to the substitution residue (GLY) from data file and add to newAtoms
    std::ifstream dataFile("gly.txt");
    if (dataFile.is_open()) {
        std::cout << "Adding atoms for residue GLY" << std::endl;
        std::string line;
        int atomNumber = deletionIndex + 1; // Start the atom numbering from the next available number
        while (std::getline(dataFile, line)) {
            if (line.substr(0, 4) == "ATOM") {
                Atom atom;
                atom.line = line.substr(0, 6) + std::to_string(atomNumber) + line.substr(11);
                atom.residue_name = "GLY";
                // Update residue sequence number to match the removed residue
                atom.line.replace(22, 4, std::to_string(removeSequenceNumber));
                newAtoms.push_back(atom);
                atomNumber++;
            }
        }
        dataFile.close();
    } else {
        std::cerr << "Error: Unable to open data file for GLY" << std::endl;
        return 1;
    }

    // Add atoms after the deleted residue
    for (size_t i = deletionIndex; i < atoms.size(); ++i) {
        if (atoms[i].residue_name != removeResidue || std::stoi(atoms[i].line.substr(22, 4)) != removeSequenceNumber) {
            newAtoms.push_back(atoms[i]);
        }
    }

    // Write new PDB file
    writePDB("modified.pdb", newAtoms);

    std::cout << "Modification complete. New PDB file saved as modified.pdb" << std::endl;

    return 0;
}