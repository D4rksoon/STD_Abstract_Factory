#include <QCoreApplication>
#include "Unit.h"
#include <iostream>

std::string generateProgram() {
    ClassUnit myClass( "MyClass" );
    myClass.add(
        std::make_shared< MethodUnit >( "testFunc1", "void",  0),
        ClassUnit::PUBLIC
        );
    myClass.add(
        std::make_shared< MethodUnit >( "testFunc2", "void", MethodUnit::STATIC ),
        ClassUnit::PRIVATE
        );
    myClass.add(
        std::make_shared< MethodUnit >( "testFunc3", "void", MethodUnit::VIRTUAL |
                                                              MethodUnit::CONST ),
        ClassUnit::PUBLIC
        );
    auto method = std::make_shared< MethodUnit >( "testFunc4", "void",
                                               MethodUnit::STATIC );
    method->add( std::make_shared< PrintOperatorUnit >( R"(Hello, world!\n)" ) );
    myClass.add( method, ClassUnit::PROTECTED );


    auto method2 = std::make_shared< MethodUnit >( "TTTT1", "void", 0);
    method2->add( std::make_shared< MethodUnit >( "TTTT2", "void", 1 )/*, 2*/); // Указывать модификацию необязательно
    method2->add( std::make_shared< MethodUnit >( "TTTT3", "void", 2 )/*, 2*/);
    auto method3 = std::make_shared< MethodUnit >( "TTTT4", "void", 0);
    method3->add( std::make_shared< MethodUnit >( "TTTT5", "void", 0 ));
    method2->add(method3);
    myClass.add( method2, ClassUnit::PROTECTED );


    return myClass.compile();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::cout << generateProgram() << std::endl;
    return a.exec();
}
