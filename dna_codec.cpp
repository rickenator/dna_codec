/*
    DNA Codec - DNA-based encoding and decoding of data
    Copyright (C) 2023 rick@aniviza.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

/*
    Theory of Operation:

    This program is designed to encode and decode data into DNA sequences. It is built
    on the concept of using DNA as a storage medium, leveraging its high-density storage
    and stability. The program uses promoter, terminator, and marker sequences modeled
    after those found in Saccharomyces cerevisiae. These sequences guide the plasmid to
    select a site for the vector in the correct gene, ensuring that the DNA is incorporated
    accurately.

    Encoding and decoding are done in strides, taking into account the encoding structure
    of UTF-8 and the necessity of having complete codons in the DNA sequence. Padding is
    employed to ensure that the total number of nucleotides forms a multiple of 3, allowing
    for effective translation in a biological system.
*/



#include <iostream>
#include <string>
#include <bitset>
#include <unordered_map>
#include <cstring> // for strcmp

// Convert binary string to DNA sequence
std::string binaryToNucleotide(const std::string &binaryStr) {
    std::unordered_map<std::string, char> binaryToNucleotide = {
        {"00", 'A'}, {"01", 'C'}, {"10", 'G'}, {"11", 'T'}
    };
    
    std::string dnaSeq;
    for (size_t i = 0; i < binaryStr.length(); i += 2) {
        std::string sub = binaryStr.substr(i, 2);
        dnaSeq += binaryToNucleotide[sub];
    }
    return dnaSeq;
}

// Convert DNA sequence to binary string
std::string nucleotideToBinary(const std::string &dnaSeq) {
    std::unordered_map<char, std::string> nucleotideToBinary = {
        {'A', "00"}, {'C', "01"}, {'G', "10"}, {'T', "11"}
    };
    
    std::string binaryStr;
    for (char ch : dnaSeq) {
        binaryStr += nucleotideToBinary[ch];
    }
    return binaryStr;
}

// Convert message to binary
std::string messageToBinary(const std::string &message) {
    std::string binaryStr;
    std::string paddedMessage = message;

    // Pad with spaces to make the total number of nucleotides a multiple of 3
    while ((4 * paddedMessage.length()) % 3 != 0) {
        paddedMessage += ' ';
    }

    for (char ch : paddedMessage) {
        std::string byte = std::bitset<8>(ch).to_string();
        binaryStr += byte;
    }
    return binaryStr;
}

// Convert binary to message
std::string binaryToMessage(const std::string &binaryStr) {
    std::string message;
    for (size_t i = 0; i < binaryStr.length(); i += 8) {
        std::string byte = binaryStr.substr(i, 8);
        char ch = static_cast<char>(std::bitset<8>(byte).to_ulong());
        message += ch;
    }
    return message;
}


int main(int argc, char *argv[]) {
    std::string promoter = "ATGCATGC";
    std::string terminator = "TTAATTAA";
    std::string marker = "GGCCGGCC";

    if (argc != 3) {
        std::cout << "Usage: ./dna_codec -e <message> OR ./dna_codec -d <dna_sequence>" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "-e") == 0) {
        std::string message = argv[2];
        std::string binaryMessage = messageToBinary(message);
        std::string encoded = binaryToNucleotide(binaryMessage);
        std::string finalEncoded = promoter + encoded + terminator + marker;
        std::cout << "Encoded DNA sequence: " << finalEncoded << std::endl;
    } else if (strcmp(argv[1], "-d") == 0) {
        std::string dnaSeq = argv[2];
        if (dnaSeq.find(promoter) == 0 && dnaSeq.rfind(terminator + marker) == dnaSeq.length() - (terminator.length() + marker.length())) {
            std::string strippedSeq = dnaSeq.substr(promoter.length(), dnaSeq.length() - (promoter.length() + terminator.length() + marker.length()));
            std::string binarySeq = nucleotideToBinary(strippedSeq);
            std::string decoded = binaryToMessage(binarySeq);
            std::cout << "Decoded message: " << decoded << std::endl;
        } else {
            std::cout << "Invalid DNA sequence. Missing promoter, terminator, or marker." << std::endl;
            return 1;
        }
    } else {
        std::cout << "Invalid option. Use -e to encode or -d to decode." << std::endl;
        return 1;
    }

    return 0;
}

