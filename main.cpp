#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <regex>


using namespace std;

char playing_as = '-';

size_t time_for_move = 3;
bool time_out = false;

struct Node;
Node* local_best;
Node* global_best;

const int max_depth = 4;

const int surround[8][2] = {{-1, -1}, {-1, 0}, {-1, 1},
                                  {0, -1}, {0, 1},
                                  {1, -1}, {1, 0}, {1, 1}};

const int board_weight[8][8] = {{10, -5, 2, 2, 2, 2 ,-5, 10},
                          {-5, -6, -1, -1, -1, -1, -6, -5},
                          {2, -1, 1, 0, 0, 1, -1, 2},
                          {2, -1 ,0, 1 ,1, 0 ,-1 ,2},
                          {2 ,-1 ,0 ,1, 1, 0, -1, 2},
                          {2 ,-1 ,1 ,0 ,0, 1, -1, 2},
                          {-5 ,-6 ,-1 ,-1 ,-1 ,-1,-6 ,-5},
                          {10, -5, 2, 2, 2, 2, -5, 10}};

enum operation {
    START,
    STOP,
    MOVE,
    invalid
};

operation return_opertation(const string& str) {
    if (str == "START") return START;
    if (str == "MOVE") return MOVE;
    if (str == "STOP") return STOP;
    else return invalid;
}


vector<string> split(string str, char separator) {
    vector<string> temp;
    vector<size_t> separator_pos;
    size_t pos = 0;
    string s;
    size_t i = 0;
    bool separator_before = false;
    for (size_t j = 0; j < str.size(); ++j) {
        if (str[j] == separator) {
            if (separator_before) {
                str.erase(str.begin() + j);
                j--;
            } else {
                separator_before = true;
            }
        } else {
            separator_before = false;
        }
    }
    while (i < str.size()) {
        for (; i < str.size(); ++i) {
            if (str[i] == separator) {
                separator_pos.push_back(i);
                i++;
                break;
            }
        }
    }
    if (separator_pos.empty()) {
        temp.push_back(str);
        return temp;
    } else {
        i = 0;
        while (pos < str.size()) {
            if (i >= separator_pos.size()) {
                while (pos < str.size()) {
                    s += str[pos];
                    pos++;
                }
            } else {
                while (pos < separator_pos[i]) {
                    s += str[pos];
                    pos++;
                }
            }
            temp.push_back(s);
            s.clear();
            pos++;
            i++;
        }
    }
    return temp;
}




vector<vector<char>> load_board (string board){

    vector<vector<char>> board_vector;
    vector<char> temp;
    for (size_t i = 0; i < board.size(); ++i) {
        /*if(board[i] == '-' || board[i] == 'X' || board[i] == 'O')
            exit(1);*/
        temp.push_back(board[i]);
        if(((i+1)%8) == 0){
            board_vector.push_back(temp);
            temp.clear();
        }
    }
    return board_vector;
}


bool is_flipable(vector<vector<char>>  board, vector<int> move, char player){
    char other_player = (player == 'X') ? 'O' : 'X';

    for(auto pos : surround){
        if(move[0]+pos[0] > 7 || move[0]+pos[0] < 0 || move[1]+pos[1] > 7 || move[1]+pos[1] < 0){
            continue;
        }
        char char_in_pos = board[move[0]+pos[0]][move[1]+pos[1]];
        if(char_in_pos == other_player){

            int curr_row = move[0] + pos[0];
            int curr_col = move[1] + pos[1];
            while(char_in_pos == other_player){
                curr_row += pos[0];
                curr_col += pos[1];


                if((curr_row > 7) || (curr_row < 0) || (curr_col > 7) || (curr_row < 0))
                    break;


                char_in_pos = board[curr_row][curr_col];
            }
            if(char_in_pos == player)
                return true;
        }
    }
    return false;

}


bool is_legal(vector<vector<char>> board, vector<int> move, vector<vector<int>> moves, char player){
    //std::vector<int> move_pair = {move[0], move[1]};

    if(move[0] > 7 || move[0] < 0 || move[1] > 7 || move[1] < 0) {
        exit(3);

    }

    if (is_flipable(board,move,player)){
        return true;
    }
    else {
        return false;
    }
}


vector<vector<int>> get_moves(vector<vector<char>> board, char player) {
    std::vector<std::vector<int>> moves;

    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 8; ++j){
            if(board[i][j] == '-'){
                if(is_legal(board, {i, j}, moves, player)){

                    vector<int> move = {i, j};
                    moves.push_back(move);
                }
            }
        }
    }
    return moves;
}


vector<vector<char>> make_move(vector<vector<char>> board, int row, int col, char player){

    board[row][col] = player;

    std::vector<std::vector<int>> discs_to_flip;

    char otherPlayer = (player == 'X') ? 'O' : 'X';

    for(auto deltas : surround){

        int curr_row = row + deltas[0];
        int curr_col = col + deltas[1];


        if(curr_row > 7 || curr_row < 0 || curr_col > 7 || curr_col < 0)
            continue;

        char char_in_pos = board[curr_row][curr_col];


        bool flip_this_direction = false;


        if(char_in_pos == otherPlayer){

            while(char_in_pos == otherPlayer){
                curr_row += deltas[0];
                curr_col += deltas[1];

                if(curr_row > 7 || curr_row < 0 || curr_col > 7 || curr_col < 0)
                    break;

                char_in_pos = board[curr_row][curr_col];
            }

            if(char_in_pos == player)
                flip_this_direction = true;

            if(flip_this_direction){
                curr_row = row + deltas[0];
                curr_col = col + deltas[1];
                char_in_pos = board[curr_row][curr_col];

                while(char_in_pos == otherPlayer){
                    std::vector<int> disc = {curr_row, curr_col};
                    discs_to_flip.push_back(disc);
                    curr_row += deltas[0];
                    curr_col += deltas[1];

                    char_in_pos = board[curr_row][curr_col];
                }

            }
        }
    }

    for(auto pos : discs_to_flip)
        board[pos[0]][pos[1]] = player;
    return board;
}


int get_fitness(vector<vector<char>> board, char player){
    int fitness = 0;
    char otherPlayer = (player == 'X') ? 'O' : 'X';

    for (size_t i = 0; i < board.size(); ++i) {
        for (size_t j = 0; j < board[i].size(); ++j) {
            if(board[i][j] == '-')
                continue;
            else if (board[i][j] == player ) {
                fitness += board_weight[i][j];
                fitness ++;
            }
            else if (board[i][j] == otherPlayer ){
                fitness -= board_weight[i][j];
                fitness--;
            }
        }
    }
    return fitness;
}


/*
struct Node
{
    vector<int> parent_move;
    int depth;
    int fitness;
    vector<vector<char>> board;
    //----------
    vector<int> move = {-1,-1};
    //---------

    size_t child_count;
    Node ** children;
    std::vector<std::vector<int>> move_list;
};*/
/*
void expand_node(struct Node *node){
    if (node->depth % 2 == 0){ // our move
        node->move_list = get_moves(node->board, playing_as);
        node->child_count = node -> move_list.size();
        if (node->depth > 0 && node->child_count > 0) {
            node->children = new Node * [node->child_count];


            for (size_t i = 0; i < node->child_count; ++i){
                vector<vector<char>> temp_board;

                temp_board = make_move(node->board, node->move_list[i][0],node->move_list[i][1],playing_as );
                node->children[i] = new Node;
                node->children[i]->board = temp_board;
                node->children[i]->depth = node->depth + 1;
                node->children[i]->fitness = get_fitness(node->children[i]->board, playing_as);
                if(node->depth == 0) {
                    node->children[i]->parent_move = node->move_list[i];
                    node->children[i]->move = node->move_list[i];
                }
                else
                    node->children[i]->parent_move = node->parent_move;
                if(node->children[i]->fitness > local_best->fitness)
                    local_best = node->children[i];

            }
        } else {
            node->children = nullptr;
        }
    }else {
        char other_player = (playing_as == 'X') ? 'O' : 'X';

        node->move_list = get_moves(node->board, other_player);
        if (node->depth > 0 && node->child_count > 0) {

            size_t best_index = 0;
            int best_fitness = 0;
            vector<Node *> temp_nodes;
            for (size_t i = 0; i < node->child_count; ++i) {
                vector<vector<char>> temp_board;
                temp_board = make_move(node->board, node->move_list[i][0],node->move_list[i][1],playing_as );
                Node* temp_node = new Node;
                temp_node->board = temp_board;
                temp_node->depth = node->depth + 1;
                temp_node->fitness = get_fitness(node->children[i]->board, other_player);
                temp_nodes.push_back(temp_node);
                if(temp_nodes[i]->fitness < best_fitness)
                    best_index = i;
            }

            node->child_count = 1;
            node->children = new Node *[node->child_count];
            node->children[0] = temp_nodes[best_index];

        }else {
            node->children = nullptr;
        }

    }
}*/
/*
void expand_depth(int depth, Node* root){

    if(root->depth == depth ){
        cout << " expanding " << endl;
        expand_node(root);
    }
    else if(root -> children != nullptr and root->depth < depth){
        for (size_t i = 0; i < root->child_count; ++i) {
            cout << " next depth " << endl;
            expand_depth(depth, root->children[i]);
        }
    }
    else
        return;
}*/

/*void find_move( vector<vector<char>> board ){
    Node* root = new Node();
    root->depth = 0;
    root->board = std::move(board);
    root->fitness = 0;
    root->parent_move = {-1,-1};

    expand_node(root);
    cout << "after epand root" << endl;
    for (size_t i = 0; i < root->child_count; ++i) {
        cout << root->children[i]-> fitness << endl;
    }

    global_best = local_best;
    int curr_depth = 1;
    while (curr_depth < max_depth and time_out == false){
        cout << "start depth " <<  curr_depth << endl;

        expand_depth(curr_depth, root);

        if(curr_depth % 2 == 0)
            global_best = local_best;
        curr_depth++;



    }
    return;
}*/

vector<int> find_best(const vector<vector<char>>& board) {
    vector<vector<int>> moves = get_moves(board, playing_as);
    vector<vector<vector<char>>> move_boards;
    vector<int> fitness;
    size_t max = 0;
    if (moves.size() <= 0) {
        exit(2);
    }

    for (size_t i = 0; i < moves.size(); ++i) {
        vector<vector<char>> temp = board;
        temp = make_move(temp,moves[i][0],moves[i][1],playing_as);
        fitness.push_back(get_fitness(temp,playing_as));
        if(i == 0 ){
            max = 0;
        }else if(fitness[i] > fitness[max])
            max = i;
    }
    return {moves[max][0], moves[max][1]};
}


int main(){
    string command;
    while (std::getline(std::cin, command))
    {
        if(command.empty())
            return 1;
        vector<string> inputs = split(command, ' ');
        switch (return_opertation(inputs[0])) {
            case START: {
                if (inputs.size() != 3)
                    return 1;
                if (inputs[1][0] == 'W' )
                    playing_as = 'O';
                if (inputs[1][0] == 'B' )
                    playing_as = 'X';

                if (playing_as != 'X' and playing_as != 'O')
                    return 1;
                int temp = stoi(inputs[2]);

                if (temp < 1) {
                    return 1;
                }
                time_for_move = stoull(inputs[2]);
                cout << "1" << endl;
                break;
            }
            case MOVE: {
                if (inputs.size() != 2)
                    return 1;
                if (playing_as == '-' or (playing_as != 'X' and playing_as != 'O'))
                    return 2;

                vector<vector<char>> board = load_board(inputs[1]);


                vector<int> move = find_best(board);
                std::string result;
                result += move[1] + '0' + 17;
                result += (move[0] + 1) + '0';
                std::cout << result << std::endl;



                break;
            }
            case STOP: {
                return 0;
                break;
            }
            case invalid:
            default:
                return 1;
        }



    }
}