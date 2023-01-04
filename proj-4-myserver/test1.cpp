#include <iostream>
#include <string>

class PrivateCopy {
private:
    // copy control is private and so is inaccessible to ordinary user code    
    PrivateCopy(const PrivateCopy&);    
    PrivateCopy &operator=(const PrivateCopy&);
public:
    PrivateCopy() = default; // use the synthesized default constructor    
    ~PrivateCopy(); // users can define objects of this type but not copy them
};

class Human {
    public:
        std::string _name;
        int _age;
        float _height;
    public:
        Human() = default;
        // equivalent to the copy constructor that would be synthesized
        Human(const Human &orig) : 
            _name(orig._name), _age(orig._age), _height(orig._height) {}
        // 成员会被自动销毁, 除此之外不会做任何事情
        ~Human() {}
        
        Human &operator=(const Human &rhs)
        {
            _name = rhs._name;
            _age = rhs._age;
            _height = rhs._height;
            return *this;
        }
        Human(std::string name, int age, float height)
        {
            this->_name = name;
            this->_age = age;
            this->_height = height;
        }
};

using namespace std;

int main()
{
    Human man;
    man._name = "Chris Li";
    man._age = 18;
    man._height = 1.7;

Human man;
Human anotherMan = man; // 使用了Human的拷贝赋值运算符

    return 0;
}
