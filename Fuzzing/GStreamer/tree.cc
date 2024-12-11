#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include <stdint.h>

#include "tree.h"
#include "aux.h"


Node::Node(uint32_t in_id, int32_t in_parent_id, uint32_t in_depth) {

    this->id = in_id;

    this->parent_id = in_parent_id;

    this->depth = in_depth;
}

const std::vector<int32_t>& Node::children() const{

    return this->prv_children;
}

std::string Node::get_label() const{

    return this->label;
}

uint32_t Node::get_id() const{

    return this->id;
}

void Node::set_label(const std::string &in_label){

    this->label = in_label;
}




uint32_t RandomTree::new_node(int32_t parent_id, uint32_t depth){

    uint32_t new_node_id = this->num_nodes;

    this->nodes.emplace_back(new_node_id, parent_id, depth);

    if(parent_id != -1){
        this->nodes[parent_id].prv_children.emplace_back(new_node_id);
    }

    if(this->levels.size() <= depth){
        this->levels.resize(depth+1);
        this->tree_depth = depth;
    }

    this->levels[depth].emplace_back(new_node_id);

    this->num_nodes++;

    return new_node_id;

}
    


RandomTree::RandomTree(uint32_t total_nodes){

    uint32_t curr_level = 0;

    //Root node
    new_node(-1, curr_level);

    curr_level++;

    uint32_t rem_nodes = total_nodes - 1;

    uint32_t current_node = 0;

    while(rem_nodes > 0){

        uint32_t num_children = rand_uint32(1, rem_nodes);

        uint32_t min_value = this->levels[curr_level-1].front();
        uint32_t max_value = this->levels[curr_level-1].back();

        for(int i=0; i<num_children; i++){

            uint32_t parent_id = rand_uint32(min_value, max_value);

            new_node(parent_id, curr_level);
        }

        curr_level++;

        rem_nodes -= num_children;
    }
    
}



Node & RandomTree::get_node(uint32_t node_id){

    return nodes[node_id];
}

size_t RandomTree::size() const {

    return nodes.size();
}

std::string RandomTree::dot_format() const {

    std::stringstream output;

    output << "digraph Tree {\n";
    output << "    node [shape=circle];\n";

    for (const Node& node : this->nodes) {
        
        output << "    " << node.id << " [label=\"" << node.label << "\"];\n";
    
        if (node.parent_id != -1) {
            
            output << "    " << node.parent_id << " -> " << node.id << ";\n";
        }
                
    }

    output << "}\n";

    return output.str();
}
  
