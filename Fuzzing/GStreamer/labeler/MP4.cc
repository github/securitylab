#include <aux.h>

#include "MP4.h"


std::string MP4_labeler::traverse(Node &node){

    std::string output;

    for(int i=0; i < node.children().size(); i++){

        Node &child = tree->get_node(node.children()[i]);

        output += traverse(child);
    }


    uint32_t size;
            
    if(node.get_id() == 0){
        size = 20;
    }else{
        size = node.get_label().size() + output.size() + 4;
    }

    std::string label = node.get_label();
    uint32_t label_size = label.size();

    output = uint32_to_string_BE(size) + label + output;

    return output;
}



MP4_labeler::MP4_labeler(RandomTree *in_tree) {

    this->tree = in_tree;

    priv_name = "MP4";

    Node &root = this->tree->get_node(0);

    std::string root_label = "ftyp";
    root_label += "dash";
    root_label += "AAAABBBB";

    root.set_label(root_label);

    for(int i=1; i < this->tree->size(); i++){

        Node &node = this->tree->get_node(i);


        uint32_t fourcc;

        uint32_t padding;

        uint32_t random_data;

                
        if(node.children().size() == 0){
                    
            //LEAF

            uint32_t random = rand_uint32(0, FOURCC_LIST_SIZE-1);

            fourcc = FOURCC_LIST[random].fourcc;

            padding = FOURCC_LIST[random].min_size;

            random_data = rand_uint32(4, 16);
                    

        }else{

            //CONTAINER

            uint32_t random = rand_uint32(0, CONTAINER_LIST_SIZE-1);

            fourcc = CONTAINER_LIST[random].fourcc;

            padding = CONTAINER_LIST[random].min_size;

            random_data = 0;

        }

        std::string label = uint32_to_string(fourcc);

        label += std::string(padding, '\x00');

        label += std::string(random_data, '\x41');

        node.set_label(label);
            
    }
}
                



std::string MP4_labeler::serialize(){

    std::string output;

    Node &root = tree->get_node(0);

    output = traverse(root);

    return output;

}

