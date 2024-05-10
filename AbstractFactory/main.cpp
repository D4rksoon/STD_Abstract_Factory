#include <QCoreApplication>
#include "Unit.h"
#include <iostream>

std::string generateProgram(IFactory* factory) {
    std::shared_ptr< IClassUnit > myClass = factory->getClass("MyClass");
    // C++  | static, const, virtual
    // Java | static, final, abstract, public, protected, private
    // C#   | const, virtual, static, public, private, protected private, protected, internal, protected internal
    myClass->add(
        factory->getMethod( "testFunc1", "void", 0 ),
        IClassUnit::PUBLIC
        );
    myClass->add(
        factory->getMethod( "testFunc2", "void", IMethodUnit::STATIC),
        IClassUnit::PRIVATE
        );
    myClass->add(
        factory->getMethod( "testFunc3", "void", IMethodUnit::CONST),
        IClassUnit::PROTECTED
        );
    std::shared_ptr< IMethodUnit > method = factory->getMethod( "testFunc4", "void", IMethodUnit::STATIC );
    method->add( factory->getPrintUnit( R"(Hello, world!\n)") );
    myClass->add( method, IClassUnit::PROTECTED );


    std::shared_ptr< IMethodUnit > method2 = factory->getMethod( "TTTT1", "void", 0);
    method2->add(factory->getMethod( "TTTT2", "void", 1 )/*, 2*/); // Указывать модификацию необязательно
    method2->add(factory->getMethod( "TTTT3", "void", 2 ), 2);
    std::shared_ptr< IMethodUnit > method3 = factory->getMethod( "TTTT4", "void", 0);
    method3->add(factory->getMethod( "TTTT5", "void", 0 ));
    method2->add(method3);
    myClass->add( method2, IClassUnit::PROTECTED );


    return myClass->compile();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CFactory Cplusplus;



    std::cout << generateProgram(&Cplusplus) << std::endl;
    return a.exec();
}
