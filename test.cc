#include <iostream>
#include <memory>
#include <string>
using namespace std;

shared_ptr<string> factory(const char* p){
    return make_shared<string>(p);
}

void use_factory(){
    shared_ptr<string> p = factory("helloworld"); 
    cout<<*p<<endl;

} 

shared_ptr<string> return_share_ptr()
{ 
    shared_ptr<string> p = factory("helloworld");;
    cout<<*p<<endl; 
    return p;
}


int main() 
{ 
    use_factory(); 
    auto p = return_share_ptr(); 
    cout<<p.use_count()<<endl; 
}