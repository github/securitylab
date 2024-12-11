#include <filesystem>
#include <vector>
#include <iostream>
#include <fstream>

#include <stdint.h>
#include <unistd.h>

#include <MP4.h>

#include "tree.h"
#include "aux.h"


void print_help(char *argv[]) {

    std::cout << "Usage: " << argv[0] << " <option(s)> -o output_dir" << std::endl;

    std::cout << "\n";
    std::cout << "Options:" << std::endl;
    std::cout << "\t -n num_nodes: number of nodes in the tree. Default: 8" << std::endl;
    std::cout << "\t -c corpus_size: number of testcases to generate. Default: 10" << std::endl;

    std::cout << std::endl;
    std::cout << "\t -o output_dir: output directory" << std::endl;

}
      
int main(int argc, char *argv[]) {

    if(argc < 2){
        print_help(argv);
        exit(EXIT_FAILURE);
    }

    std::string output_dir = "";

    uint32_t num_children = 0;
    uint32_t max_depth = 0;

    uint32_t num_nodes = 8;

    uint32_t corpus_size = 10;

    int ch;
    while ((ch = getopt(argc, argv, "n:c:o:")) != -1) {

        switch (ch) {

        case 'n': {
            num_nodes = std::stoi(optarg);
            break;
        }

        case 'c': {
            corpus_size = std::stoi(optarg);
            break;
        }

        case 'o': {
            output_dir = optarg;
            break;
        }

        default:
            print_help(argv);
            exit(EXIT_FAILURE);
        }
    }

    if(output_dir == ""){
        std::cerr << "Output directory not specified" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::filesystem::path dir = output_dir;
    if(!std::filesystem::exists(dir)){
        std::cerr << "Output directory does not exist" << std::endl;
        exit(EXIT_FAILURE);
    }

   if(num_nodes < 1 || num_nodes > 20){
        std::cerr << "Number of nodes must be between 1 and 20" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Generating " << corpus_size << " testcases with " << num_nodes << " nodes" << std::endl;

    for(int i=0; i < corpus_size; i++){

        RandomTree tree(num_nodes);

        MP4_labeler labeler(&tree);

        #ifdef DEBUG
        std::string dot = tree.dot_format();
        std::cout << dot << std::endl;
        #endif


        std::string file_content = labeler.serialize();

        std::string output_file = output_dir + "/out_" + std::to_string(i);

        if(!write_to_file(file_content, output_file)){
            std::cerr << "Error writing to file" << std::endl;
            exit(EXIT_FAILURE);
        }

    }


}
  
