#include "parser.h"
#include <algorithm>
#include <sstream>

namespace trieannosaurus {

//Adapted from https://stackoverflow.com/questions/446296/where-can-i-get-a-useful-c-binary-search-algorithm
template<class Iter, class T>
Iter binarySearch(Iter begin, Iter end, T& val) {
    Iter i = std::lower_bound(begin, end, val);

    if (i != end && *i == val)
        return i; // found
    else
        return end; // not found
}


class Node {
public:
    uint16_t id_;
    std::vector<size_t> lines;
    std::vector<Node> next_level;

    explicit Node(uint16_t id) : id_(id) {
        //
    }

    bool operator<(const Node& other) const {
        return id_ < other.id_;
    }
    bool operator<(const uint16_t& other) const {
        return id_ < other;
    }

    bool operator==(const Node& other) const {
        return id_ == other.id_;
    }

    bool operator==(const uint16_t& other) const {
        return id_ == other;
    }

    friend void swap(Node& lhs, Node& rhs) {
        std::swap(lhs.id_, rhs.id_);
        lhs.next_level.swap(rhs.next_level);
    }
};

class trieMeARiver {
private:
    std::vector<Node> trie_;
    std::unordered_map<std::string, uint16_t> dict_;
    std::unordered_map<uint16_t, std::string> vocab_;
public:
    trieMeARiver(std::unordered_map<std::string, uint16_t> dict,
        std::unordered_map<uint16_t, std::string> vocab) : dict_(dict), vocab_(vocab) {
        //This should be done in order in order to avoid the sort at the end
        /*
        for (auto&& item : vocab_) {
            Node node;
            node.id_ = item.first;
            trie_.push_back(node);
        }
        std::sort(trie_.begin(), trie_.end());*/
    }
    void operator()(const std::string& line) {
        std::vector<std::string> tokens;
        tokenizeSentence(line, tokens, true);

        Node* curr_level = &trie_;

        for (auto&& item : tokens) {
            uint16_t id = dict_.at(item);
                auto it = binarySearch(curr_level->next_level.begin(), curr_level->next_level.end(), id);
                if (it == curr_level->next_level.end()) {
                    curr_level->next_level.emplace_back(Node(id));
                    std::sort(curr_level->next_level.begin(), curr_level->next_level.end());
                    //std::sort changes the address of the vector in the node so we can't save a pointer to it and save oursaves a search
                    //We can definitely do it better.
                    //std::vector<Node>* nxt_lvl = &curr_level->back().next_level;
                    //std::sort(curr_level->begin(), curr_level->end());
                    //curr_level = nxt_lvl; produces wrong results
                    curr_level = &*binarySearch(curr_level->next_level.begin(), curr_level->next_level.end(), id);
                } else {
                    curr_level = &*it;
                }
            }
        }

        curr_level->lines.push_back(++line_count);
    }
    
    Node* getTrie() {
        return &trie_;
    }

    /*The rest of the find functions are for testing/debugging purposes*/
    std::string find(std::string input) {
        std::vector<Node>* curr_level = &trie_;

        std::vector<std::string> tokens;
        tokenizeSentence(input, tokens);

        for (auto&& token : tokens) {
            uint16_t id = dict_.at(token);
            curr_level = this->find(id, curr_level);
            if (curr_level == nullptr || curr_level->size() == 0) {
                return std::string("No continuations found");
            }
        }
        std::string ret("");
        for (auto&& node : *curr_level) {
            if (ret != "") {
                ret = ret + " ";
            }
            ret += vocab_.at(node.id_);
        }
        return ret;
    }

};

inline const Node* find(uint16_t id, const Node* root) {
  if(!root) {
    return nullptr;
  }

  auto it = binarySearch(root->next_level.begin(), root->next_level.end(), id);

  if(it == root->next_level.end()) {
    return nullptr;
  } else {
    return &*it;
  }
}

} //Namespace
