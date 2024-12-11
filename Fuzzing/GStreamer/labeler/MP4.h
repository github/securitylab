#pragma once

#include <string>
#include <iostream>

#include <tree.h>

#include "fourcc.h"
#include "labeler.h"


class MP4_labeler : public Labeler{

    private:

        RandomTree *tree;

        std::string traverse(Node &node);

    public:

        MP4_labeler(RandomTree *in_tree);
                
        std::string serialize();
};
