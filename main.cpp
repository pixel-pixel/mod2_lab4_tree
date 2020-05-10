#include <iostream>
#include <cmath>

using namespace std;
//Пробіли не враховуються, змінна - одна ЛАТИНСЬКА буква окрім 's' та 'c' (в обох регістрах)
//Підтримує числа з комою (може бути кома або крапка), пробіли в числі - недопустимі
//Синус, косінус - sin, cos / SIN, COS (можна як "sin(5)" так і "sin 5" але "sin(5 + x)" і "sin 5 + x" - різне

char *expr = "cos 0 / 2.5 + (sin(x) * 3 - b)"; // <<< Вираз для обчислення

enum OPERATION{
    NON, NUM, VAL, PLS, MNS, MUL, DIV, SIN, COS, LFT, RGT
};

struct node{
    OPERATION operation = NON;
    float num = 0;
    char ch;
    node* up = nullptr;
    node* left = nullptr;
    node* right = nullptr;

    const char* msg_for_val = "Введіть, будь ласка, змінну ";

    node(node* parent){
        up = parent;
    }

    float result(){
        switch (operation) {
            case NUM: return num;
            case PLS: return left->result() + right->result();
            case MNS: return left->result() - right->result();
            case MUL: return left->result() * right->result();
            case DIV: return left->result() / right->result();
            case SIN: return sin(right->result());
            case COS: return cos(right->result());
            case VAL:
                cout << msg_for_val << ch << endl;

                cin >> num;
                return num;
            default:
                float res = 0;
                if(left != nullptr) res += left->result();
                if(right != nullptr) res += right->result();
                return res;
        }
    }
};

struct syntax_tree{
    node* head = new node(nullptr);

    void put(OPERATION operation){
        if(operation == LFT) left_bracket();
        else if(operation == RGT) right_bracket();
        else if(head->operation == NON) {

            if (head->left == nullptr) {
                head->left = new node(head);
                head->left->operation = NUM;
            }

            head->operation = operation;
            head->right = new node(head);
            head = head->right;
        }else{
            if((operation == MUL || operation == DIV) && (head->operation == PLS || head->operation == MNS)){
                head->right->up = new node(head);
                head->right->up->left = head->right;
                head->right = head->right->up;
                head = head->right;
            }else{
                if(head->up == nullptr){
                    head->up = new node(nullptr);
                    head->up->left = head;
                    head = head->up;
                } else{
                    if(head->up->left == head){
                        head->up = new node(head->up);
                        head->up->left = head;
                        head->up->up->left = head->up;
                        head = head->up;
                    }else{
                        head->up = new node(head->up);
                        head->up->left = head;
                        head->up->up->right = head->up;
                        head = head->up;
                    }
                }
            }
            head->operation = operation;
            head->right = new node(head);
            head = head->right;
        }
    }

    void put(float num){
        number(num);
    }

    void put(char ch){
        val(ch);
    }

    float result(){
        while (head->up != nullptr) head = head->up;
        return head->result();
    }

private:
    void left_bracket(){
        head->left = new node(head);
        head = head->left;
    }

    void right_bracket(){
        if(head->up != nullptr) head = head->up;
    }

    void number(float num){
        head->operation = NUM;
        head->num = num;
        head = head->up;
    }

    void val(char ch){
        head->operation = VAL;
        head->ch = ch;
        head = head->up;
    }
};

class parser{
    syntax_tree tree;
public:
    void parse(char arr[]){
        int i = 0;

        tree.put(LFT);
        tree.put(LFT);

        while(arr[i] != '\0'){
            if(arr[i] == ' '){}
            else if(arr[i] == '+') tree.put(PLS);
            else if(arr[i] == '-') tree.put(MNS);
            else if(arr[i] == '*') tree.put(MUL);
            else if(arr[i] == '/') tree.put(DIV);
            else if(arr[i] == '(') tree.put(LFT);
            else if(arr[i] == ')') tree.put(RGT);
            else if(arr[i] == 'S' || arr[i] == 's'){
                tree.put(SIN);
                i += 2;
            }else if(arr[i] == 'C' || arr[i] == 'c'){
                tree.put(COS);
                i += 2;
            } else if((arr[i] >= 48 && arr[i] <= 57) || arr[i] == 44 || arr[i] == 46){
                int k = 0;
                float num = 0;

                while (arr[i + k] >= 48 && arr[i + k] <= 57){
                    k++;
                }

                while ((arr[i] >= 48 && arr[i] <= 57) || arr[i] == 44 || arr[i] == 46){
                    if(k > 0) num += (arr[i] - 48) * pow(10, k-1);
                    else if(k < 0) num += (arr[i] - 48) * pow(10, k);
                    k--;
                    i++;
                }
                tree.put(num);
                i--;
            }else{
                tree.put(arr[i]);
            }
            i++;
        }
        tree.put(RGT);
    }

    float result(){
        return tree.result();
    }
};


int main() {
    parser p;
    p.parse(expr);
    cout << p.result();

    return 0;
}
