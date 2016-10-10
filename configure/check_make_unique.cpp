//
// Created by ingvord on 10/10/16.
//
#include <memory>


using std;

struct Hello {
    string str {"World"};
};

void main(){
    unique_ptr<Hello>  ptr = make_unique<Hello>();
    cout << "Hello World" << ptr->str << endl;
    return 0;
}

