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
    and stability. The program uses PROMOTER, TERMINATOR, and MARKER sequences modeled
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
#include <cstring>
#include <fstream>

#define VERSION 				1.1
#define PROMOTER 				"ATGCATGC"
#define TERMINATOR				"TTAATTAA"
#define MARKER 					"GGCCGGCC"


using namespace std;

// codec processing
string binaryToNucleotide(const string &binaryStr);
string nucleotideToBinary(const string &dnaSeq);
string messageToBinary(const string &message);
string binaryToMessage(const string &binaryStr);

// file check
bool openFile(const string &fileName, string &contents, ios_base::openmode mode);

// command line option handlers
bool doStringEncode(const string& message); 	// -e
bool doStringDecode(const string& encodedMsg); 	// -d
bool doFileEncode(const string& filename); 		// -i
bool doFileDecode(const string& filename);		// -o



int main(int argc, char *argv[]) {

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " [-e | -d | -i | -o] <argument>" << endl;
        return 1;
    }
    
    string arg = argv[2];

    // Encoding message to DNA sequence
    if (strcmp(argv[1], "-e") == 0) {
    	doStringEncode(arg);
    // Encoding file to DNA sequence .dna file
    } else if (strcmp(argv[1], "-i") == 0) {
    	doFileEncode(arg);
    // Decoding from .dna file to original content
    } else if (strcmp(argv[1], "-o") == 0) {
    	doFileDecode(arg);
    // Decoding DNA sequence to STRING message
    } else if (strcmp(argv[1], "-d") == 0) {
    	doStringDecode(arg);
    }

    return 0;
}

bool doStringEncode(const string& message) {
    string binaryMessage = messageToBinary("STRING:" + message);
    string encoded = binaryToNucleotide(binaryMessage);
    string finalEncoded = PROMOTER + encoded + TERMINATOR + MARKER;
    cout << VERSION << " || Encoded: " << finalEncoded << endl;
    return true;
}

bool doStringDecode(const string& dnaSeq) {
	string decodedBinary = nucleotideToBinary(dnaSeq.substr(string(PROMOTER).length(), dnaSeq.length() - string(PROMOTER).length() - string(TERMINATOR).length() - string(MARKER).length()));
	string decoded = binaryToMessage(decodedBinary);

	if (decoded.rfind("STRING:", 0) == 0) {
		string messageContent = decoded.substr(7);
		cout << "Decoded: " << messageContent << endl;
	}
	return true;
}

bool doFileEncode(const string& fileName) {
	ifstream inFile(fileName, ios::binary);
	string fileContents((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
	inFile.close();
	string header = "FILE:" + fileName + ":";

	string binaryFileContents = messageToBinary(header + fileContents);
	string encoded = binaryToNucleotide(binaryFileContents);
	string finalEncoded = PROMOTER + encoded + TERMINATOR + MARKER;

	ofstream outFile(fileName + ".dna", ios::binary);
	outFile << finalEncoded;
	outFile.close();
	return true;
}

bool doFileDecode(const string& dnaFileName) {
    if (dnaFileName.substr(dnaFileName.find_last_of(".") + 1) != "dna") {
        cerr << "Invalid file suffix, expecting .dna file." << endl;
        return 1;
    }

    string dnaContents;
    if (!openFile(dnaFileName, dnaContents, ios::binary)) {
        cerr << "Could not open file: " << dnaFileName << endl;
        return 1;
    }

    // Remove PROMOTER, TERMINATOR, and MARKER
    string cleanDNA = dnaContents.substr(string(PROMOTER).length(), dnaContents.length() - string(PROMOTER).length() - string(TERMINATOR).length() - string(MARKER).length());

    string decodedBinary = nucleotideToBinary(cleanDNA);
    string decoded = binaryToMessage(decodedBinary);

    if (decoded.rfind("FILE:", 0) == 0) {
        size_t firstColon = decoded.find(":", 5);
        size_t secondColon = decoded.find(":", firstColon + 1);
        string originalFileName = decoded.substr(firstColon + 1, secondColon - firstColon - 1);
        string fileContent = decoded.substr(secondColon + 1);

        // Correcting header misinterpretation
        if (originalFileName.empty() || fileContent.empty()) {
            cerr << "Invalid DNA content header or content." << endl;
            return 1;
        }

        ofstream outFile(originalFileName, ios::binary);
        if (!outFile.is_open()) {
            cerr << "Could not create output file." << endl;
            return 1;
        }

        outFile << fileContent;
        outFile.close();
        cout << "Decoded to file: " << originalFileName << endl;
    } else {
        cerr << "Invalid DNA content header." << endl;
        return 1;
    }
	return true;
}

// Convert binary string to DNA sequence
string binaryToNucleotide(const string &binaryStr) {
    unordered_map<string, char> binaryToNucleotide = { // @suppress("Invalid template argument")
        {"00", 'A'}, {"01", 'C'}, {"10", 'G'}, {"11", 'T'}
    };

    string dnaSeq;
    for (size_t i = 0; i < binaryStr.length(); i += 2) {
        string sub = binaryStr.substr(i, 2);
        dnaSeq += binaryToNucleotide[sub];
    }
    return dnaSeq;
}

// Convert DNA sequence to binary string
string nucleotideToBinary(const string &dnaSeq) {
    unordered_map<char, string> nucleotideToBinary = { // @suppress("Invalid template argument")
        {'A', "00"}, {'C', "01"}, {'G', "10"}, {'T', "11"}
    };

    string binaryStr;
    for (char ch : dnaSeq) {
        binaryStr += nucleotideToBinary[ch];
    }
    return binaryStr;
}

// Convert message to binary
string messageToBinary(const string &message) {
    string binaryStr;
    string paddedMessage = message;

    // Pad with spaces to make the total number of nucleotides a multiple of 3
    while ((4 * paddedMessage.length()) % 3 != 0) {
        paddedMessage += ' ';
    }

    for (char ch : paddedMessage) {
        string byte = bitset<8>(ch).to_string();
        binaryStr += byte;
    }
    return binaryStr;
}

// Convert binary to message
string binaryToMessage(const string &binaryStr) {
    string message;
    for (size_t i = 0; i < binaryStr.length(); i += 8) {
        string byte = binaryStr.substr(i, 8);
        char ch = static_cast<char>(bitset<8>(byte).to_ulong()); // @suppress("Symbol is not resolved")
        message += ch;
    }
    return message;
}

// Check if valid file
bool openFile(const string &fileName, string &contents, ios_base::openmode mode) {
    ifstream inFile(fileName, mode);
    if (!inFile.is_open()) return false;

    contents.assign((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    return true;
}

