# DNA Codec

## Introduction

DNA Codec is a program designed for encoding and decoding data into DNA sequences. This project is open-source software created by [rick@aniviza.com](mailto:rick@aniviza.com) in 2023.

## License

This program is released under the GNU General Public License, either version 2 or any later version. You can find the full license text in the [LICENSE](LICENSE) file.

## Theory of Operation

### Overview

DNA Codec utilizes the unique properties of DNA as a storage medium, taking advantage of its high-density storage capacity and stability. The program employs specific promoter, terminator, and marker sequences inspired by those found in Saccharomyces cerevisiae to guide the plasmid to the correct gene site, ensuring accurate DNA incorporation.

### Encoding and Decoding

Data encoding and decoding in DNA Codec are executed in strides. The program considers the encoding structure of UTF-8 and the need for complete codons in the DNA sequence. Padding is applied to ensure that the total number of nucleotides is a multiple of 3, enabling effective translation within a biological system.

For more detailed information and usage instructions, refer to the project's documentation.

## Warranty Disclaimer

This program is distributed without any warranty, either implied or explicit. It is provided "as is" and should be used at your own discretion.

---

Please note that this README is a formatted summary of the comments from the source code. For detailed information and usage instructions, consult the source code or project documentation.

